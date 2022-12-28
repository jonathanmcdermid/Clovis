#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {

		vector<Position> positions;
		vector<double> results;
		long double best_mse;
		long double k;

		constexpr int n_cores = 6;
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

		repeat:

			improved = false;

			best_mse = mean_squared_error(k);

			long double mse = tune_loop(weights);

			for (short step = 1; step < 5; ++step)
			{
				cout << "step: " << step << endl;
				for (size_t index = 0; index < weights.size() - 1; index += 2)
				{
					short best_val1 = *weights[index];
					short best_val2 = *weights[index + 1];

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
						best_val1 = *weights[index];
						best_val2 = *weights[index + 1];
						best_mse = mse;
						print_params();
						cout << mse << endl;
						improved = true;
					}
					else
					{
						*weights[index] = best_val1;
						*weights[index + 1] = best_val2;
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
						*weights[index] = best_val1;
						*weights[index + 1] = best_val2;
					}
				}
			}

			if (improved)
				goto repeat;

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

		vector<short*> map_weights_to_params()
		{
			// point weights to the variables in the evaluation function
			
			vector<short*> weights;
			
			weights.push_back(&Eval::king_full_open_penalty.mg);
			weights.push_back(&Eval::king_full_open_penalty.eg);
			weights.push_back(&Eval::king_semi_open_penalty.mg);
			weights.push_back(&Eval::king_semi_open_penalty.eg);

			weights.push_back(&Eval::king_adjacent_full_open_penalty.mg);
			weights.push_back(&Eval::king_adjacent_full_open_penalty.eg);
			weights.push_back(&Eval::king_adjacent_semi_open_penalty.mg);
			weights.push_back(&Eval::king_adjacent_semi_open_penalty.eg);

			weights.push_back(&Eval::outpost_bonus[0].mg);
			weights.push_back(&Eval::outpost_bonus[0].eg);
			weights.push_back(&Eval::outpost_bonus[1].mg);
			weights.push_back(&Eval::outpost_bonus[1].eg);

			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				if ((sq / 4 != RANK_1 && sq / 4 != RANK_8))
				{
					weights.push_back(&Eval::pawn_table[sq].mg);
					weights.push_back(&Eval::pawn_table[sq].eg);
					if(sq / 4 != RANK_2)
					{
						weights.push_back(&Eval::passed_pawn_bonus[sq].mg);
						weights.push_back(&Eval::passed_pawn_bonus[sq].eg);
					}
				}

				weights.push_back(&Eval::knight_table[sq].mg);
				weights.push_back(&Eval::knight_table[sq].eg);

				weights.push_back(&Eval::bishop_table[sq].mg);
				weights.push_back(&Eval::bishop_table[sq].eg);

				weights.push_back(&Eval::rook_table[sq].mg);
				weights.push_back(&Eval::rook_table[sq].eg);

				weights.push_back(&Eval::queen_table[sq].mg);
				weights.push_back(&Eval::queen_table[sq].eg);

				weights.push_back(&Eval::king_table[sq].mg);
				weights.push_back(&Eval::king_table[sq].eg);
			}

			weights.push_back(&Eval::tempo_bonus.mg);
			weights.push_back(&Eval::tempo_bonus.eg);
			weights.push_back(&Eval::double_pawn_penalty.mg);
			weights.push_back(&Eval::double_pawn_penalty.eg);
			weights.push_back(&Eval::isolated_pawn_penalty.mg);
			weights.push_back(&Eval::isolated_pawn_penalty.eg);
			weights.push_back(&Eval::bishop_pair_bonus.mg);
			weights.push_back(&Eval::bishop_pair_bonus.eg);
			weights.push_back(&Eval::rook_open_file_bonus.mg);
			weights.push_back(&Eval::rook_open_file_bonus.eg);
			weights.push_back(&Eval::rook_semi_open_file_bonus.mg);
			weights.push_back(&Eval::rook_semi_open_file_bonus.eg);

			for (int j = KNIGHT; j < KING; ++j)
			{
				weights.push_back(&Eval::mobility[j].mg);
				weights.push_back(&Eval::mobility[j].eg);

				weights.push_back(&Eval::inner_ring_attack[j].mg);
				weights.push_back(&Eval::inner_ring_attack[j].eg);
				weights.push_back(&Eval::outer_ring_attack[j].mg);
				weights.push_back(&Eval::outer_ring_attack[j].eg);
			}

			return weights;
		}

		void print_params()
		{
			// print the tuned weights so they can be copy pasted into evaluation file

			cout << "Score pawn_table[] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::pawn_table[sq].mg << ","
					<< Eval::pawn_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score knight_table[] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::knight_table[sq].mg << ","
					<< Eval::knight_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score bishop_table[] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::bishop_table[sq].mg << ","
					<< Eval::bishop_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score rook_table[] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::rook_table[sq].mg << ","
					<< Eval::rook_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score queen_table[] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::queen_table[sq].mg << ","
					<< Eval::queen_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score king_table[] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::king_table[sq].mg << ","
					<< Eval::king_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score passed_pawn_bonus[] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << " Score("
					<< Eval::passed_pawn_bonus[sq].mg << ", "
					<< Eval::passed_pawn_bonus[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score double_pawn_penalty = Score("
				<< Eval::double_pawn_penalty.mg << ", "
				<< Eval::double_pawn_penalty.eg << ");\n";
			cout << "Score isolated_pawn_penalty = Score("
				<< Eval::isolated_pawn_penalty.mg << ", "
				<< Eval::isolated_pawn_penalty.eg << ");\n";
			cout << "Score bishop_pair_bonus = Score("
				<< Eval::bishop_pair_bonus.mg << ", "
				<< Eval::bishop_pair_bonus.eg << ");\n";
			cout << "Score rook_open_file_bonus = Score("
				<< Eval::rook_open_file_bonus.mg << ", "
				<< Eval::rook_open_file_bonus.eg << ");\n";
			cout << "Score rook_semi_open_file_bonus = Score("
				<< Eval::rook_semi_open_file_bonus.mg << ", "
				<< Eval::rook_semi_open_file_bonus.eg << ");\n";
			cout << "Score tempo_bonus = Score("
				<< Eval::tempo_bonus.mg << ", "
				<< Eval::tempo_bonus.eg << ");\n";
			cout << "Score king_full_open_penalty = Score("
				<< Eval::king_full_open_penalty.mg << ", "
				<< Eval::king_full_open_penalty.eg << ");\n";
			cout << "Score king_semi_open_penalty = Score("
				<< Eval::king_semi_open_penalty.mg << ", "
				<< Eval::king_semi_open_penalty.eg << ");\n";
			cout << "Score king_adjacent_full_open_penalty = Score("
				<< Eval::king_adjacent_full_open_penalty.mg << ", "
				<< Eval::king_adjacent_full_open_penalty.eg << ");\n";
			cout << "Score king_adjacent_semi_open_penalty = Score("
				<< Eval::king_adjacent_semi_open_penalty.mg << ", "
				<< Eval::king_adjacent_semi_open_penalty.eg << ");\n";

			cout << "Score mobility[7] = {";
			for (int i = NO_PIECE; i <= KING; ++i) {
				cout << " Score("
					<< Eval::mobility[i].mg << ", "
					<< Eval::mobility[i].eg << "),";
			}
			cout << "};\n";
			cout << "Score inner_ring_attack[7] = {";
			for (int i = NO_PIECE; i <= KING; ++i) {
				cout << " Score("
					<< Eval::inner_ring_attack[i].mg << ", "
					<< Eval::inner_ring_attack[i].eg << "),";
			}
			cout << "};\n";
			cout << "Score outer_ring_attack[7] = {";
			for (int i = NO_PIECE; i <= KING; ++i) {
				cout << " Score("
					<< Eval::outer_ring_attack[i].mg << ", "
					<< Eval::outer_ring_attack[i].eg << "),";
			}
			cout << "};\n";
			cout << "Score outpost_bonus[2] = {";
			for (int i = KNIGHT; i <= BISHOP; ++i) {
				cout << " Score("
					<< Eval::outpost_bonus[i - KNIGHT].mg << ", "
					<< Eval::outpost_bonus[i - KNIGHT].eg << "),";
			}
			cout << "};\n";
		}

	} // Tuner

} // Clovis
