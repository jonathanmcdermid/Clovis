#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {

		vector<TEntry> entries;
		long double answers;

		constexpr int N_CORES = 8;
		constexpr int N_POSITIONS = 75000;
		
		int n_terms;
		int psqt_index;
		int passers_index;
		int double_pawn_penalty_index;
		int isolated_pawn_penalty_index;
		int bishop_pair_bonus_index;
		int rook_open_file_bonus_index;
		int rook_semi_open_file_bonus_index;
		int tempo_bonus_index;
		int king_full_open_penalty_index;
		int king_semi_open_penalty_index;
		int king_adjacent_full_open_penalty_index;
		int king_adjacent_semi_open_penalty_index;
		int knight_outpost_bonus_index;
		int bishop_outpost_bonus_index;
		int rook_closed_file_penalty_index;
		int weak_queen_penalty_index;
		int rook_on_our_passer_file_index;
		int rook_on_their_passer_file_index;
		int tall_pawn_penalty_index;
		int fianchetto_bonus_index;

		double sigmoid(double K, double E) {
			return 1.0 / (1.0 + exp(-K * E / 400.0));
		}
		
		void initTuples() 
		{
			
		}
	
		void tune_eval()
		{
			// load positions and results from file
			string file_name = "src/tuner.epd";
			ifstream ifs;
			ifs.open(file_name.c_str(), ifstream::in);
			string line;

			for(int i = 0; i < N_POSITIONS; ++i)
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
						entries[i].result = 1.0;
					else if (res == "0-1")
						entries[i].result = 0.0;
					else if (res == "1/2-1/2")
						entries[i].result = 0.5;
					else exit(EXIT_FAILURE);
					
					Position pos = Position(line.substr(0, idx).c_str());
					
					Eval::T.clear();
					
					entries[i].rho[MG] = 1.0 - pos.get_game_phase() / MAX_GAMEPHASE;
					entries[i].rho[EG] = 0.0 + pos.get_game_phase() / MAX_GAMEPHASE;
					entries[i].seval = Eval::evaluate<true>(pos);
					if (pos.side == BLACK)
						entries[i].seval = -entries[i].seval;
					entries[i].stm = pos.side;
					
					
				}
			}

			ifs.close();
		}

		long double mean_squared_error(long double K)
		{
			// compute mean squared error for all positions in the set
			// multithreading solution

			answers = 0;

			int batch_size = entries.size() / N_CORES;

			vector<thread> thread_pool;
			int start = 0;
			int end = 0;

			for (int i = 0; i < N_CORES; ++i)
			{
				start = end;
				end = start + batch_size;
				K = K;
				//thread_pool.push_back(thread(processor, start, end, K));
			}

			for (int i = 0; i < N_CORES; ++i)
				thread_pool[i].join();

			return answers / entries.size();
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

	} // namespace Tuner

} // namespace Clovis
