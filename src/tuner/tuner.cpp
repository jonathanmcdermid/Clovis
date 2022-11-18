#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {

		vector<Position> positions;
		vector<double> results;
		vector<Weight> weights;

		constexpr int n_cores = 4;
		long double answers = 0;

		inline long double sigmoid(long double K, long double E) {
			return 1.0 / (1.0 + exp(-K * E / 400.0));
		}

		void tune()
		{
			// load positions and results from file
			string file_name = "src/tuner/quiet-labeled.epd";
			ifstream log_file;
			log_file.open(file_name.c_str(), ifstream::in);
			string line;
			while (true)
			{
				if (log_file.eof())
					break;
				getline(log_file, line);
				if (line != "")
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
			log_file.close();

			cout << "positions done loading " << positions.size() << "\n";

			map_weights_to_params();

			// compute scaling constant k
			long double mse;
			long double k = find_k();
			long double best_mse = mean_squared_error(k);

			vector<Weight> best_weights;
			vector<int> direction(weights.size(), 1);

			bool improved = true;

			for (unsigned iterations = 0; improved; ++iterations)
			{
				improved = false;
				for (int index = 0; index < weights.size(); ++index)
				{
					int old_val = *weights[index].value;

					// increase weight
					*weights[index].value += direction[index];

					if (weights[index].natural)
						*weights[index].value = max(short(0), *weights[index].value);

					mse = mean_squared_error(k);

					// if a thread fails we catch it here
					while (best_mse - mse > best_mse / (n_cores + 1))
						mse = mean_squared_error(k);

					if (mse < best_mse)
					{
						direction[index] += direction[index];
						best_mse = mse;
						improved = true;
						best_weights = weights;
					}
					else
					{

						*weights[index].value = old_val;

						direction[index] = direction[index] / abs(direction[index]);
						// decrease weight
						*weights[index].value -= direction[index];

						if (weights[index].natural)
							*weights[index].value = max(short(0), *weights[index].value);

						mse = mean_squared_error(k);

						// if a thread fails we catch it here
						while (best_mse - mse > best_mse / (n_cores + 1))
							mse = mean_squared_error(k);

						if (mse < best_mse)
						{
							direction[index] = -direction[index];
							best_mse = mse;
							improved = true;

							best_weights = weights;
						}
						else
							// reset weight, no improvements
							*weights[index].value = old_val;
					}
					cout << "weight " << index << " mse: " << best_mse << " adjustment: " << direction[index] << "\n";
				}

				// this probably doesnt need to be done
				weights = best_weights;

				cout << "iteration " << iterations << " complete" << endl;
				print_params();
			}
			cout << "\ndone!\n";
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
				eval = (Eval::evaluate(positions[i]) + Eval::evaluate_pawns(positions[i])).get_score(positions[i].get_game_phase(), WHITE);
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

				cout << "epoch " << i << " K = " << start << " E = " << best << endl;

				end = start + step;
				start -= step;
				step /= 10.0;
			}

			return start;
		}

		void map_weights_to_params()
		{
			// point weights to the variables in the evaluation function

			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				if ((sq / 4 != RANK_1 && sq / 4 != RANK_8))
				{
					weights.push_back(Weight(&Eval::pawn_table[sq].mg, false));
					weights.push_back(Weight(&Eval::pawn_table[sq].eg, false));

					weights.push_back(Weight(&Eval::passed_pawn_bonus[sq].mg, true));
					weights.push_back(Weight(&Eval::passed_pawn_bonus[sq].eg, true));
				}
				weights.push_back(Weight(&Eval::knight_table[sq].mg, false));
				weights.push_back(Weight(&Eval::knight_table[sq].eg, false));

				weights.push_back(Weight(&Eval::bishop_table[sq].mg, false));
				weights.push_back(Weight(&Eval::bishop_table[sq].eg, false));

				weights.push_back(Weight(&Eval::rook_table[sq].mg, false));
				weights.push_back(Weight(&Eval::rook_table[sq].eg, false));

				weights.push_back(Weight(&Eval::queen_table[sq].mg, false));
				weights.push_back(Weight(&Eval::queen_table[sq].eg, false));

				weights.push_back(Weight(&Eval::king_table[sq].mg, false));
				weights.push_back(Weight(&Eval::king_table[sq].eg, false));
			}

			weights.push_back(Weight(&Eval::double_pawn_penalty.mg, true));
			weights.push_back(Weight(&Eval::double_pawn_penalty.eg, true));
			weights.push_back(Weight(&Eval::isolated_pawn_penalty.mg, true));
			weights.push_back(Weight(&Eval::isolated_pawn_penalty.eg, true));
			weights.push_back(Weight(&Eval::bishop_pair_bonus.mg, true));
			weights.push_back(Weight(&Eval::bishop_pair_bonus.eg, true));
			weights.push_back(Weight(&Eval::rook_open_file_bonus.mg, true));
			weights.push_back(Weight(&Eval::rook_open_file_bonus.eg, true));
			weights.push_back(Weight(&Eval::rook_semi_open_file_bonus.mg, true));
			weights.push_back(Weight(&Eval::rook_semi_open_file_bonus.eg, true));
			weights.push_back(Weight(&Eval::king_safety_reduction_factor.mg, true));
			weights.push_back(Weight(&Eval::king_safety_reduction_factor.eg, true));

			for (int j = KNIGHT; j < KING; ++j) {
				weights.push_back(Weight(&Eval::mobility[j].mg, true));
				weights.push_back(Weight(&Eval::mobility[j].eg, true));

				weights.push_back(Weight(&Eval::inner_ring_attack[j].mg, true));
				weights.push_back(Weight(&Eval::inner_ring_attack[j].eg, true));
				weights.push_back(Weight(&Eval::outer_ring_attack[j].mg, true));
				weights.push_back(Weight(&Eval::outer_ring_attack[j].eg, true));
			}
		}

		void print_params()
		{
			// print the tuned weights so they can be copy pasted into evaluation file

			cout << "Score pawn_table[32] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::pawn_table[sq].mg << ","
					<< Eval::pawn_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score knight_table[32] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::knight_table[sq].mg << ","
					<< Eval::knight_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score bishop_table[32] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::bishop_table[sq].mg << ","
					<< Eval::bishop_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score rook_table[32] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::rook_table[sq].mg << ","
					<< Eval::rook_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score queen_table[32] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::queen_table[sq].mg << ","
					<< Eval::queen_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score king_table[32] = {\n\t";
			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				cout << "Score("
					<< Eval::king_table[sq].mg << ","
					<< Eval::king_table[sq].eg << "),";
				(sq % 4 == 3) ? cout << "\n\t" : cout << " ";
			}
			cout << "};\n";

			cout << "Score passed_pawn_bonus[32] = {\n\t";
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
			cout << "Score king_safety_reduction_factor = Score("
				<< Eval::king_safety_reduction_factor.mg << ", "
				<< Eval::king_safety_reduction_factor.eg << ");\n";

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
		}

	} // Tuner

} // Clovis