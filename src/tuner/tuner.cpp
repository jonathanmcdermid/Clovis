#include "tuner.h"

namespace Clovis {

	namespace Tuner {

		std::vector<Position> positions;
		std::vector<double> results;
		std::vector<int> direction;
		std::vector<Weight> weights;

		constexpr int n_cores = 4;
		long double answers = 0;

		void tune()
		{
			// load positions and results from file
			std::string file_name = "src/tuner/quiet-labeled.epd";
			std::ifstream log_file;
			log_file.open(file_name.c_str(), std::ifstream::in);
			std::string line;
			while (true)
			{
				if (log_file.eof())
					break;
				getline(log_file, line);
				if (line != "")
				{
					size_t idx = line.find("\"");
					size_t idx_end = line.find("\"", idx + 1);
					std::string res = line.substr(idx + 1, idx_end - idx - 1);
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

			std::cout << "positions done loading " << positions.size() << "\n";

			map_weights_to_params();

			// compute scaling constant k
			long double mse;
			long double k = find_k();
			long double best_mse = mean_squared_error(k);

			std::vector<short> best_vals;
			best_vals.resize(weights.size());
			direction.resize(0);
			for (int i = 0; i < weights.size(); ++i)
			{
				best_vals[i] = *weights[i].val;
				direction.push_back(1);
			}

			bool improved = true;

			for (unsigned iterations = 0; improved; ++iterations)
			{
				improved = false;
				for (int index = 0; index < weights.size(); ++index)
				{
					if (weights[index].skip)
						continue;

					int old_val = *weights[index].val;

					// increase weight
					*weights[index].val += direction[index];

					if (weights[index].positive)
						*weights[index].val = std::max(short(0), *weights[index].val);

					mse = mean_squared_error(k);

					// if a thread fails we catch it here
					while (best_mse - mse > best_mse / (n_cores + 1))
						mse = mean_squared_error(k);

					if (mse < best_mse)
					{
						direction[index] += direction[index];
						best_mse = mse;
						improved = true;
						for (int i = 0; i < weights.size(); ++i)
							best_vals[i] = *weights[i].val;
					}
					else
					{

						*weights[index].val = old_val;

						direction[index] = direction[index] / std::abs(direction[index]);
						// decrease weight
						*weights[index].val -= direction[index];

						if (weights[index].positive)
							*weights[index].val = std::max(short(0), *weights[index].val);

						mse = mean_squared_error(k);

						// if a thread fails we catch it here
						while (best_mse - mse > best_mse / (n_cores + 1))
							mse = mean_squared_error(k);

						if (mse < best_mse)
						{
							direction[index] = -direction[index];
							best_mse = mse;
							improved = true;

							for (int i = 0; i < weights.size(); ++i)
								best_vals[i] = *weights[i].val;
						}
						else
							// reset weight, no improvements
							*weights[index].val = old_val;
					}
					std::cout << "weight " << index << " mse: " << best_mse << " adjustment: " << direction[index] << "\n";
				}

				// this probably doesnt need to be done
				for (int i = 0; i < weights.size(); ++i)
					*weights[i].val = best_vals[i];

				std::cout << "iteration " << iterations << " complete" << std::endl;
				print_params();
			}
			std::cout << "\ndone!\n";
		}

		long double mean_squared_error(long double K)
		{
			// compute mean squared error for all positions in the set
			// multithreading solution
			Eval::init_values();
			answers = 0;

			int batch_size = positions.size() / n_cores;

			std::vector<std::thread> myThreads;
			int start = 0;
			int end = 0;

			for (int i = 0; i < n_cores; ++i)
			{
				start = end;
				end = start + batch_size;
				myThreads.push_back(std::thread(processor, start, end, K));
			}

			for (int i = 0; i < n_cores; ++i)
				myThreads[i].join();

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
			// compute scaling constant k
			int k_precision = 10;
			long double start = 0;
			long double end = 10;
			long double step = 1;
			long double err = 0;
			long double curr = start;
			long double best = mean_squared_error(start);

			for (int i = 0; i < k_precision; ++i)
			{
				curr = start - step;

				while (curr < end)
				{
					curr = curr + step;
					err = mean_squared_error(curr);

					if (err <= best)
					{
						best = err;
						start = curr;
					}
				}

				std::cout.precision(17);
				std::cout << "Best K of " << start << " on iteration " << i << "\n";

				end = start + step;
				start = start - step;
				step = step / 10.0;
			}
			return start;
		}

		void map_weights_to_params()
		{
			// point weights to the variables in the evaluation function
			for (Square k = SQ_ZERO; k < 32; ++k) {
				weights.push_back(Weight(&Eval::pawn_table[k].mg, (k / 4 == RANK_1 || k / 4 == RANK_8), false));
				weights.push_back(Weight(&Eval::pawn_table[k].eg, (k / 4 == RANK_1 || k / 4 == RANK_8), false));
			}
			for (Square k = SQ_ZERO; k < 32; ++k) {
				weights.push_back(Weight(&Eval::knight_table[k].mg, false, false));
				weights.push_back(Weight(&Eval::knight_table[k].eg, false, false));
			}
			for (Square k = SQ_ZERO; k < 32; ++k) {
				weights.push_back(Weight(&Eval::bishop_table[k].mg, false, false));
				weights.push_back(Weight(&Eval::bishop_table[k].eg, false, false));
			}
			for (Square k = SQ_ZERO; k < 32; ++k) {
				weights.push_back(Weight(&Eval::rook_table[k].mg, false, false));
				weights.push_back(Weight(&Eval::rook_table[k].eg, false, false));
			}
			for (Square k = SQ_ZERO; k < 32; ++k) {
				weights.push_back(Weight(&Eval::queen_table[k].mg, false, false));
				weights.push_back(Weight(&Eval::queen_table[k].eg, false, false));
			}
			for (Square k = SQ_ZERO; k < 32; ++k) {
				weights.push_back(Weight(&Eval::king_table[k].mg, false, false));
				weights.push_back(Weight(&Eval::king_table[k].eg, false, false));
			}
			for (int j = PAWN; j < KING; ++j) {
				weights.push_back(Weight(&Eval::piece_value[j].mg, true, true));
				weights.push_back(Weight(&Eval::piece_value[j].eg, true, true));
			}
			for (Square k = SQ_ZERO; k < 32; ++k) {
				weights.push_back(Weight(&Eval::passed_pawn_bonus[k].mg, (k / 4 == RANK_1 || k / 4 == RANK_8), true));
				weights.push_back(Weight(&Eval::passed_pawn_bonus[k].eg, (k / 4 == RANK_1 || k / 4 == RANK_8), true));
			}

			weights.push_back(Weight(&Eval::double_pawn_penalty.mg, false, true));
			weights.push_back(Weight(&Eval::double_pawn_penalty.eg, false, true));
			weights.push_back(Weight(&Eval::isolated_pawn_penalty.mg, false, true));
			weights.push_back(Weight(&Eval::isolated_pawn_penalty.eg, false, true));
			weights.push_back(Weight(&Eval::bishop_pair_bonus.mg, false, true));
			weights.push_back(Weight(&Eval::bishop_pair_bonus.eg, false, true));
			weights.push_back(Weight(&Eval::rook_open_file_bonus.mg, false, true));
			weights.push_back(Weight(&Eval::rook_open_file_bonus.eg, false, true));
			weights.push_back(Weight(&Eval::rook_semi_open_file_bonus.mg, false, true));
			weights.push_back(Weight(&Eval::rook_semi_open_file_bonus.eg, false, true));

			for (int j = KNIGHT; j < KING; ++j) {
				weights.push_back(Weight(&Eval::mobility[j].mg, false, true));
				weights.push_back(Weight(&Eval::mobility[j].eg, false, true));
			}
		}

		void print_params()
		{
			// print the tuned weights so they can be copy pasted into evaluation file
			std::cout << "Score pawn_table[32] = {\n\t";
			for (Square j = SQ_ZERO; j < 32; ++j) {
				std::cout << "Score("
					<< Eval::pawn_table[j].mg << ","
					<< Eval::pawn_table[j].eg << "),";
				(j % 4 == 3) ? std::cout << "\n\t" : std::cout << " ";
			}
			std::cout << "};\n";
			std::cout << "Score knight_table[32] = {\n\t";
			for (Square j = SQ_ZERO; j < 32; ++j) {
				std::cout << "Score("
					<< Eval::knight_table[j].mg << ","
					<< Eval::knight_table[j].eg << "),";
				(j % 4 == 3) ? std::cout << "\n\t" : std::cout << " ";
			}
			std::cout << "};\n";
			std::cout << "Score bishop_table[32] = {\n\t";
			for (Square j = SQ_ZERO; j < 32; ++j) {
				std::cout << "Score("
					<< Eval::bishop_table[j].mg << ","
					<< Eval::bishop_table[j].eg << "),";
				(j % 4 == 3) ? std::cout << "\n\t" : std::cout << " ";
			}
			std::cout << "};\n";
			std::cout << "Score rook_table[32] = {\n\t";
			for (Square j = SQ_ZERO; j < 32; ++j) {
				std::cout << "Score("
					<< Eval::rook_table[j].mg << ","
					<< Eval::rook_table[j].eg << "),";
				(j % 4 == 3) ? std::cout << "\n\t" : std::cout << " ";
			}
			std::cout << "};\n";
			std::cout << "Score queen_table[32] = {\n\t";
			for (Square j = SQ_ZERO; j < 32; ++j) {
				std::cout << "Score("
					<< Eval::queen_table[j].mg << ","
					<< Eval::queen_table[j].eg << "),";
				(j % 4 == 3) ? std::cout << "\n\t" : std::cout << " ";
			}
			std::cout << "};\n";
			std::cout << "Score king_table[32] = {\n\t";
			for (Square j = SQ_ZERO; j < 32; ++j) {
				std::cout << "Score("
					<< Eval::king_table[j].mg << ","
					<< Eval::king_table[j].eg << "),";
				(j % 4 == 3) ? std::cout << "\n\t" : std::cout << " ";
			}
			std::cout << "};\n";
			std::cout << "Score passed_pawn_bonus[32] = {\n\t";
			for (Square j = SQ_ZERO; j < 32; ++j) {
				std::cout << " Score("
					<< Eval::passed_pawn_bonus[j].mg << ", "
					<< Eval::passed_pawn_bonus[j].eg << "),";
				(j % 4 == 3) ? std::cout << "\n\t" : std::cout << " ";
			}
			std::cout << " };\n";
			std::cout << "Score piece_value[7] = {";
			for (int j = NO_PIECE; j <= KING; ++j) {
				std::cout << " Score("
					<< Eval::piece_value[j].mg << ", "
					<< Eval::piece_value[j].eg << "),";
			}
			std::cout << "};\n";
			std::cout << "Score double_pawn_penalty = Score("
				<< Eval::double_pawn_penalty.mg << ", "
				<< Eval::double_pawn_penalty.eg << ");\n";
			std::cout << "Score isolated_pawn_penalty = Score("
				<< Eval::isolated_pawn_penalty.mg << ", "
				<< Eval::isolated_pawn_penalty.eg << ");\n";
			std::cout << "Score bishop_pair_bonus = Score("
				<< Eval::bishop_pair_bonus.mg << ", "
				<< Eval::bishop_pair_bonus.eg << ");\n";
			std::cout << "Score rook_open_file_bonus = Score("
				<< Eval::rook_open_file_bonus.mg << ", "
				<< Eval::rook_open_file_bonus.eg << ");\n";
			std::cout << "Score rook_semi_open_file_bonus = Score("
				<< Eval::rook_semi_open_file_bonus.mg << ", "
				<< Eval::rook_semi_open_file_bonus.eg << ");\n";
			std::cout << "Score mobility[7] = {";
			for (int j = NO_PIECE; j <= KING; ++j) {
				std::cout << " Score("
					<< Eval::mobility[j].mg << ", "
					<< Eval::mobility[j].eg << "),";
			}
			std::cout << "};\n";
			std::cout << "short mobility_reduction[7] = {";
			for (int j = NO_PIECE; j <= KING; ++j) {
				std::cout << Eval::mobility_reduction[j] << ", ";
			}
			std::cout << "};\n";
		}

	} // Tuner

} // Clovis