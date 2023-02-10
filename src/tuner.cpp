#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {

		vector<Position> positions;
		vector<double> results;
		long double best_mse;
		long double k;
		size_t safety_index;

		constexpr int n_cores = 8;
		long double answers = 0;
	
		void tune_eval(bool safety_only)
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

			vector<short*> weights = map_weights_to_params(safety_only);

			bool improved;

			// compute scaling constant k
			k = 1.68;//find_k();

			do {
				improved = false;

				best_mse = mean_squared_error(k);

				long double mse = tune_loop(weights);

				for (short step = 0; step < 25; ++step)
				{
					cout << "step: " << step << endl;
					for (size_t index = 0; index < safety_index; index += 2)
					{
						short best_val_mg = *weights[index];
						short best_val_eg = *weights[index + 1];

						*weights[index] += step;

						for (int i = 0; i < 25 && mse >= best_mse && *weights[index + 1] > 0; ++i)
						{
							*weights[index + 1] = best_val_eg - i;
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

						for (int i = 0; i < 25 && mse >= best_mse; ++i)
						{
							*weights[index + 1] = best_val_eg + i;
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

		vector<short*> map_weights_to_params(bool safety_only)
		{
			// point weights to the variables in the evaluation function
			
			vector<short*> weights;

			using namespace Eval;

			if (!safety_only)
			{
				//add_weight(weights, rook_on_seventh);
				//add_weight(weights, knight_behind_pawn_bonus);
				//add_weight(weights, bishop_behind_pawn_bonus);
				add_weight(weights, fianchetto_bonus);
				add_weight(weights, tall_pawn_penalty);
				add_weight(weights, rook_on_our_passer_file);
				add_weight(weights, rook_on_their_passer_file);
				add_weight(weights, weak_queen_penalty);
				add_weight(weights, tempo_bonus);
				add_weight(weights, double_pawn_penalty);
				add_weight(weights, isolated_pawn_penalty);
				add_weight(weights, bishop_pair_bonus);
				add_weight(weights, rook_open_file_bonus);
				add_weight(weights, rook_semi_open_file_bonus);
				add_weight(weights, knight_outpost_bonus);
				add_weight(weights, bishop_outpost_bonus);
				add_weight(weights, rook_closed_file_penalty);
				add_weight(weights, king_full_open_penalty);
				add_weight(weights, king_semi_open_penalty);
				add_weight(weights, king_adjacent_full_open_penalty);
				add_weight(weights, king_adjacent_semi_open_penalty);

				for (Square sq = SQ_ZERO; sq < 32; ++sq)
				{
					if ((Rank(sq / 4) != RANK_1 && Rank(sq / 4) != RANK_8))
					{
						add_weight(weights, pawn_table[sq]);
					
						if (Rank(sq / 4) != RANK_2)
							add_weight(weights, passed_pawn[sq]);
					}

					add_weight(weights, knight_table[sq]);
					//add_weight(weights, bishop_table[sq]);
					add_weight(weights, rook_table[sq]);
					add_weight(weights, queen_table[sq]);
					//add_weight(weights, king_table[sq]);
				
					if (sq < 16)
					{
						//add_weight(weights, knight_table[sq]);
						add_weight(weights, bishop_table[sq]);
						//add_weight(weights, rook_table[sq]);
						//add_weight(weights, queen_table[sq]);
						add_weight(weights, king_table[sq]);
					
						if (sq / 4 >= (sq & 3))
						{
							//add_weight(weights, knight_table[sq]);
							//add_weight(weights, bishop_table[sq]);
							//add_weight(weights, rook_table[sq]);
							//add_weight(weights, queen_table[sq]);
							//add_weight(weights, king_table[sq]);
						}
					}
				}
			
				for (int i = PAWN; i < KING; ++i)
				{
					if (i > PAWN)
						add_weight(weights, mobility[i]);
				}
			}
			
			// king safety terms only affect middlegame
            
			safety_index = weights.size();
		
			weights.push_back(&safety_threshold);
			weights.push_back(&virtual_king_m);
			weights.push_back(&virtual_king_b);
			
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				if (Rank(sq / 4) > RANK_4)
					weights.push_back(&pawn_shield[sq]);
			}

			for (int i = PAWN; i < KING; ++i)
			{
				weights.push_back(&inner_ring_attack[i]);
				weights.push_back(&outer_ring_attack[i]);
			}
			
			weights.push_back(&opposite_bishops_scaling);

			return weights;		
		}

		void print_score_table(string name, Score* begin, int size, int cols) 
		{
			cout << "\t\tScore " << name << "[] = {" << endl << "\t\t";

			for (int i = 0; i < size; ++i) {
				if (!(i % cols))
					cout << '\t';
				cout << *begin++ << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
			}

			cout << "};" << endl << endl;
		}
		
		void print_short_table(string name, short* begin, int size, int cols) 
		{
			cout << "\t\tshort " << name << "[] = {" << endl << "\t\t";

			for (int i = 0; i < size; ++i) {
				if (!(i % cols))
					cout << '\t';
				cout << *begin++ << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
			}

			cout << "};" << endl << endl;
		}

		void print_params()
		{
			// print the tuned weights so they can be copy pasted into evaluation file
            
			using namespace Eval;
			
			print_score_table("pawn_table",   pawn_table,   sizeof(pawn_table)   / sizeof(Score), 4);
			print_score_table("knight_table", knight_table, sizeof(knight_table) / sizeof(Score), 4);
			print_score_table("bishop_table", bishop_table, sizeof(bishop_table) / sizeof(Score), 4);
			print_score_table("rook_table",   rook_table,   sizeof(rook_table)   / sizeof(Score), 4);
			print_score_table("queen_table",  queen_table,  sizeof(queen_table)  / sizeof(Score), 4);
			print_score_table("king_table",   king_table,   sizeof(king_table)   / sizeof(Score), 4);
			print_score_table("passed_pawn",  passed_pawn,  sizeof(passed_pawn)  / sizeof(Score), 4);
			print_short_table("pawn_shield",  pawn_shield,  sizeof(pawn_shield)  / sizeof(short), 4);
			
			print_score_table("mobility",          mobility,          7, 7);
			print_short_table("inner_ring_attack", inner_ring_attack, 7, 7);
			print_short_table("outer_ring_attack", outer_ring_attack, 7, 7);

			cout << "\t\tScore double_pawn_penalty = "        << double_pawn_penalty             << ";" << endl
			<< "\t\tScore isolated_pawn_penalty = "           << isolated_pawn_penalty           << ";" << endl
			<< "\t\tScore bishop_pair_bonus = "               << bishop_pair_bonus               << ";" << endl
			<< "\t\tScore rook_open_file_bonus = "	          << rook_open_file_bonus            << ";" << endl
			<< "\t\tScore rook_semi_open_file_bonus = "       << rook_semi_open_file_bonus       << ";" << endl
			<< "\t\tScore tempo_bonus = "                     << tempo_bonus                     << ";" << endl
			<< "\t\tScore king_full_open_penalty = "          << king_full_open_penalty          << ";" << endl
			<< "\t\tScore king_semi_open_penalty = "          << king_semi_open_penalty          << ";" << endl
			<< "\t\tScore king_adjacent_full_open_penalty = " << king_adjacent_full_open_penalty << ";" << endl
			<< "\t\tScore king_adjacent_semi_open_penalty = " << king_adjacent_semi_open_penalty << ";" << endl
			<< "\t\tScore knight_outpost_bonus = "            << knight_outpost_bonus            << ";" << endl
			<< "\t\tScore bishop_outpost_bonus = "            << bishop_outpost_bonus            << ";" << endl
			<< "\t\tshort virtual_king_m = "                  << virtual_king_m                  << ";" << endl
			<< "\t\tshort virtual_king_b = "                  << virtual_king_b                  << ";" << endl
			<< "\t\tScore rook_closed_file_penalty = "        << rook_closed_file_penalty        << ";" << endl
			<< "\t\tScore weak_queen_penalty = "              << weak_queen_penalty              << ";" << endl
			<< "\t\tScore rook_on_our_passer_file = "         << rook_on_our_passer_file         << ";" << endl
			<< "\t\tScore rook_on_their_passer_file = "       << rook_on_their_passer_file       << ";" << endl
			<< "\t\tScore knight_behind_pawn_bonus = "        << knight_behind_pawn_bonus        << ";" << endl
			<< "\t\tScore bishop_behind_pawn_bonus = "        << bishop_behind_pawn_bonus        << ";" << endl
			<< "\t\tScore rook_on_seventh = "                 << rook_on_seventh                 << ";" << endl
			<< "\t\tScore tall_pawn_penalty = "               << tall_pawn_penalty               << ";" << endl
			<< "\t\tScore fianchetto_bonus = "                << fianchetto_bonus                << ";" << endl
			<< "\t\tshort safety_threshold = "                << safety_threshold                << ";" << endl
			<< "\t\tshort opposite_bishops_scaling = "        << opposite_bishops_scaling        << ";" << endl;		
		}

	} // namespace Tuner

} // namespace Clovis
