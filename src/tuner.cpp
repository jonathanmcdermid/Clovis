#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {

		vector<Position> positions;
		vector<double> results;
		long double best_mse;
		long double k;

		constexpr int n_cores = 8;
		long double answers = 0;
	
		void tune_eval()
		{
			// load positions and results from file
			string file_name = "src/tuner.epd";
			ifstream ifs;
			ifs.open(file_name.c_str(), ifstream::in);
			string line;

			while (true)
			{
				if (ifs.eof())
					break;
				getline(ifs, line);
				if (line.length())
				{
					size_t idx = line.find("\"");
					size_t idx_end = line.find("\"", idx + 1);
					string res = line.substr(idx + 1, idx_end - idx - 1);
					if (res == "1-0")
						results.push_back(1.0);
					else if (res == "0-1")
						results.push_back(0.0);
					else if (res == "1/2-1/2")
						results.push_back(0.5);
					else return;
					positions.push_back(Position(line.substr(0, idx).c_str()));
				}
			}

			ifs.close();

			cout << "positions done loading " << positions.size() << endl;

			vector<short*> weights = map_weights_to_params();

			bool improved;

			// compute scaling constant k
			k = 1.68;//find_k();

			do {
				improved = false;

				best_mse = mean_squared_error(k);

				long double mse = tune_loop(weights);

				for (short step = 1; step < 5; ++step)
				{
					cout << "step: " << step << endl;
					for (size_t index = 0; index < weights.size() - 1; index += 2)
					{
						short best_val_mg = *weights[index];
						short best_val_eg = *weights[index + 1];

						*weights[index] += step;

						for (int i = 0; i < 5 && mse >= best_mse && *weights[index + 1] > 0; ++i)
						{
							*weights[index + 1] -= 1;
							*weights[index + 1] = max(short(0), *weights[index + 1]);

							mse = mean_squared_error(k);

							// if a thread fails we catch it here
							while (best_mse - mse > best_mse / (n_cores + 1))
								mse = mean_squared_error(k);
						}

						if (mse < best_mse)
						{
							best_val_mg = *weights[index];
							best_val_eg = *weights[index + 1];
							best_mse = mse;
							print_params();
							cout << mse << endl;
							improved = true;
						}
						else
						{
							*weights[index]     = best_val_mg;
							*weights[index + 1] = best_val_eg;
						}

						if (*weights[index] == 0)
							continue;

						*weights[index] -= step;
						*weights[index] = max(short(0), *weights[index]);

						for (int i = 0; i < 10 && mse >= best_mse; ++i)
						{
							*weights[index + 1] += 1;
							mse = mean_squared_error(k);

							// if a thread fails we catch it here
							while (best_mse - mse > best_mse / (n_cores + 1))
								mse = mean_squared_error(k);
						}

						if (mse < best_mse)
						{
							best_mse = mse;
							print_params();
							cout << mse << endl;
							improved = true;
						}
						else
						{
							*weights[index]     = best_val_mg;
							*weights[index + 1] = best_val_eg;
						}
					}
				}
			} while (improved);

			print_params();

		}

		long double tune_loop(vector<short*> weights)
		{
			long double mse;

			vector<int> direction(weights.size(), 1);

			bool improved = true;

			for (unsigned iterations = 0; improved; ++iterations)
			{
				improved = false;
				for (size_t index = 0; index < weights.size(); ++index)
				{
					short old_val = *weights[index];

					// increase weight
					*weights[index] += direction[index];

					*weights[index] = max(short(0), *weights[index]);

					mse = mean_squared_error(k);

					// if a thread fails we catch it here
					while (best_mse - mse > best_mse / (n_cores + 1))
						mse = mean_squared_error(k);

					if (mse < best_mse)
					{
						best_mse = mse;
						improved = true;
					}
					else
					{
						*weights[index] = old_val;

						// decrease weight
						*weights[index] -= direction[index];

						*weights[index] = max(short(0), *weights[index]);

						mse = mean_squared_error(k);

						// if a thread fails we catch it here
						while (best_mse - mse > best_mse / (n_cores + 1))
							mse = mean_squared_error(k);

						if (mse < best_mse)
						{
							direction[index] = -direction[index];
							best_mse = mse;
							improved = true;
						}
						else
							// reset weight, no improvements
							*weights[index] = old_val;
					}
					cout << "weight " << index << " mse: " << best_mse << endl;
				}
				
				print_params();
				cout << "iteration " << iterations << " complete" << endl;
			}
			cout << "\ndone!\n";

			return best_mse;
		}

		long double mean_squared_error(long double K)
		{
			// compute mean squared error for all positions in the set
			// multithreading solution

			answers = 0;

			int batch_size = positions.size() / n_cores;

			vector<thread> thread_pool;
			int start = 0;
			int end = 0;

			for (int i = 0; i < n_cores; ++i)
			{
				start = end;
				end = start + batch_size;
				thread_pool.push_back(thread(processor, start, end, K));
			}

			for (int i = 0; i < n_cores; ++i)
				thread_pool[i].join();

			return answers / positions.size();
		}

		void processor(int start, int end, long double K)
		{
			// thread process for calculating the mse of the divided set

			long double error_sum = 0;
			long double sigmoid = 0;
			int eval;

			for (int i = start; i < end; ++i)
			{
				eval = Eval::evaluate<false>(positions[i]);
				// all evaluations need to be from white perspective
				if (positions[i].side == BLACK)
					eval = -eval;
				sigmoid = 1 / (1 + pow(10, -K * eval / 400));
				error_sum += pow(results[i] - sigmoid, 2);
			}

			answers += error_sum;
		}

		long double find_k()
		{
			int k_precision = 10;
			long double start = -10;
			long double end = 10;
			long double step = 1;
			long double curr = start;
			long double error;
			long double best = mean_squared_error(start);

			for (int i = 0; i < k_precision; ++i)
			{
				curr = start - step;

				while (curr < end)
				{
					curr += step;
					error = mean_squared_error(curr);
					if (error < best)
					{
						best = error;
						start = curr;
					}
				}

				cout.precision(17);
				cout << "epoch " << i << " K = " << start << " E = " << best << endl;

				end = start + step;
				start -= step;
				step /= 10.0;
			}

			return start;
		}

		void add_weight(vector<short*>& weights, Score& s) 
		{
			weights.push_back(&s.mg);
			weights.push_back(&s.eg);
		}

		vector<short*> map_weights_to_params()
		{
			// point weights to the variables in the evaluation function
			
			vector<short*> weights;
			add_weight(weights, Eval::virtual_king_m);
			add_weight(weights, Eval::virtual_king_b);

			add_weight(weights, Eval::king_full_open_penalty);
			add_weight(weights, Eval::king_semi_open_penalty);
			
			add_weight(weights, Eval::king_adjacent_full_open_penalty);
			add_weight(weights, Eval::king_adjacent_semi_open_penalty);

			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				if ((Rank(sq / 4) != RANK_1 && Rank(sq / 4) != RANK_8))
				{
					add_weight(weights, Eval::pawn_table[sq]);
					
					if(Rank(sq / 4) != RANK_2)
						add_weight(weights, Eval::passed_pawn_bonus[sq]);
				}

				add_weight(weights, Eval::queen_table[sq]);
				if (sq < 16)
				{
					if (sq / 4 >= (sq & 3))
					{
						add_weight(weights, Eval::bishop_table[sq]);
						add_weight(weights, Eval::rook_table[sq]);
						add_weight(weights, Eval::king_table[sq]);
						add_weight(weights, Eval::knight_table[sq]);
					}
				}
			}

			add_weight(weights, Eval::tempo_bonus);
			add_weight(weights, Eval::double_pawn_penalty);
			add_weight(weights, Eval::isolated_pawn_penalty);
			add_weight(weights, Eval::bishop_pair_bonus);
			add_weight(weights, Eval::rook_open_file_bonus);
			add_weight(weights, Eval::rook_semi_open_file_bonus);
			add_weight(weights, Eval::knight_outpost_bonus);
			add_weight(weights, Eval::bishop_outpost_bonus);
			add_weight(weights, Eval::rook_closed_file_penalty);


			for (int i = PAWN; i < KING; ++i)
			{
				if (i > PAWN)
					add_weight(weights, Eval::mobility[i]);

				add_weight(weights, Eval::inner_ring_attack[i]);
				add_weight(weights, Eval::outer_ring_attack[i]);
			}

			return weights;
		}

		void print_score_table(string name, Score* begin, int size, int cols) 
		{
			cout << "Score " << name << "[] = {" << endl;

			for (int i = 0; i < size; ++i) {
				if (!(i % cols))
					cout << '\t';
				cout << *begin++ << "," << ((i % cols == (cols - 1)) ? "\n" : " ");
			}

			cout << "};" << endl;
		}

		void print_params()
		{
			// print the tuned weights so they can be copy pasted into evaluation file
			
			print_score_table("pawn_table",   Eval::pawn_table,   32, 4);
			print_score_table("knight_table", Eval::knight_table, 16, 4);
			print_score_table("bishop_table", Eval::bishop_table, 16, 4);
			print_score_table("rook_table",   Eval::rook_table,   16, 4);
			print_score_table("queen_table",  Eval::queen_table,  32, 4);
			print_score_table("king_table",   Eval::king_table,   16, 4);
			print_score_table("passed_pawn_bonus", Eval::passed_pawn_bonus, 32, 4);
			print_score_table("mobility", Eval::mobility, PIECETYPE_N, PIECETYPE_N);
			print_score_table("inner_ring_attack", Eval::inner_ring_attack, PIECETYPE_N, PIECETYPE_N);
			print_score_table("outer_ring_attack", Eval::outer_ring_attack, PIECETYPE_N, PIECETYPE_N);

			cout << "Score double_pawn_penalty = "        << Eval::double_pawn_penalty             << ";" << endl
			<< "Score isolated_pawn_penalty = "           << Eval::isolated_pawn_penalty           << ";" << endl
			<< "Score bishop_pair_bonus = "               << Eval::bishop_pair_bonus               << ";" << endl
			<< "Score rook_open_file_bonus = "	      << Eval::rook_open_file_bonus            << ";" << endl
			<< "Score rook_semi_open_file_bonus = "       << Eval::rook_semi_open_file_bonus       << ";" << endl
			<< "Score tempo_bonus = "                     << Eval::tempo_bonus                     << ";" << endl
			<< "Score king_full_open_penalty = "          << Eval::king_full_open_penalty          << ";" << endl
			<< "Score king_semi_open_penalty = "          << Eval::king_semi_open_penalty          << ";" << endl
			<< "Score king_adjacent_full_open_penalty = " << Eval::king_adjacent_full_open_penalty << ";" << endl
			<< "Score king_adjacent_semi_open_penalty = " << Eval::king_adjacent_semi_open_penalty << ";" << endl
			<< "Score knight_outpost_bonus = "            << Eval::knight_outpost_bonus            << ";" << endl
			<< "Score bishop_outpost_bonus = "            << Eval::bishop_outpost_bonus            << ";" << endl
			<< "Score virtual_king_m = "                  << Eval::virtual_king_m                  << ";" << endl
			<< "Score virtual_king_b = "                  << Eval::virtual_king_b                  << ";" << endl
			<< "Score rook_closed_file_penalty = "        << Eval::rook_closed_file_penalty        << ";" << endl;			
		}

	} // namespace Tuner

} // namespace Clovis
