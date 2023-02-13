#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {

		vector<TEntry> entries;

		constexpr int N_CORES = 8;
		constexpr int N_POSITIONS = 75000;

		double sigmoid(double K, double E) {
			return 1.0 / (1.0 + exp(-K * E / 400.0));
		}
	
		void tune_eval(bool safety_only)
		{
			// load positions and results from file
			string file_name = "src/tuner.epd";
			ifstream ifs;
			ifs.open(file_name.c_str(), ifstream::in);
			string line;
			Position pos;

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
					
					pos.set(line.substr(0, idx).c_str())));
					
					entries[i].rho[MG] = 1.0 - pos.get_game_phase() / MAX_GAMEPHASE;
					entries[i].rho[EG] = 0.0 + pos.get_game_phase() / MAX_GAMEPHASE;
					entries[i].seval = Eval::Evaluate<false>(pos);
					if (pos.stm == BLACK)
						entries[i].seval = -entries[i].seval;
					entries[i].stm = pos.stm;
				}
			}

			ifs.close();
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

			for (int i = start; i < end; ++i)
			{
				int E = (positions[i].side == WHITE)
					? Eval::evaluate<false>(positions[i])
					:-Eval::evaluate<false>(positions[i]);

				error_sum += pow(results[i] - sigmoid(K, E), 2);
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
                /*
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
                */
			}

            /*
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
            */

			return weights;		
		}

		void print_score_table(string name, const Score* begin, int size, int cols) 
		{
			cout << "\t\tScore " << name << "[] = {" << endl << "\t\t";

			for (int i = 0; i < size; ++i) {
				if (!(i % cols))
					cout << '\t';
				cout << *begin++ << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
			}

			cout << "};" << endl << endl;
		}
		
		void print_short_table(string name, const short* begin, int size, int cols) 
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
