#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {
	
		typedef double TVector[TI_N][PHASE_N];

		vector<TEntry> entries;
		TVector params;

		constexpr int N_CORES = 8;
		constexpr int N_POSITIONS = 725000;
		constexpr int MAX_EPOCHS = 100000;

		inline double sigmoid(double K, double E) {
			return 1.0 / (1.0 + exp(-K * E / 400.0));
		}
		
		void init_params()
		{
			using namespace Eval;
			
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				params[PAWN_PSQT + sq][MG] = (double) pawn_table[sq].mg;
				params[PAWN_PSQT + sq][EG] = (double) pawn_table[sq].eg;
			}
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				params[KNIGHT_PSQT + sq][MG] = (double) knight_table[sq].mg;
				params[KNIGHT_PSQT + sq][EG] = (double) knight_table[sq].eg;
			}
			for (Square sq = SQ_ZERO; sq < 16; ++sq)
			{
				params[BISHOP_PSQT + sq][MG] = (double) bishop_table[sq].mg;
				params[BISHOP_PSQT + sq][EG] = (double) bishop_table[sq].eg;
			}
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				params[ROOK_PSQT + sq][MG] = (double) rook_table[sq].mg;
				params[ROOK_PSQT + sq][EG] = (double) rook_table[sq].eg;
			}
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				params[QUEEN_PSQT + sq][MG] = (double) queen_table[sq].mg;
				params[QUEEN_PSQT + sq][EG] = (double) queen_table[sq].eg;
			}
			for (Square sq = SQ_ZERO; sq < 16; ++sq)
			{
				params[KING_PSQT + sq][MG] = (double) king_table[sq].mg;
				params[KING_PSQT + sq][EG] = (double) king_table[sq].eg;
			}
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				params[PASSED_PAWN + sq][MG] = (double) passed_pawn[sq].mg;
				params[PASSED_PAWN + sq][EG] = (double) passed_pawn[sq].eg;
			}
			for (PieceType pt = PIECETYPE_NONE; pt <= KING; ++pt)
			{
				params[MOBILITY + pt][MG] = (double) mobility[pt].mg;
				params[MOBILITY + pt][EG] = (double) mobility[pt].eg;
			}
			
			params[DOUBLE_PAWN][MG] = double_pawn_penalty.mg;
			params[DOUBLE_PAWN][EG] = double_pawn_penalty.eg;
			params[ISOLATED_PAWN][MG] = isolated_pawn_penalty.mg;
			params[ISOLATED_PAWN][EG] = isolated_pawn_penalty.eg;
			params[BISHOP_PAIR][MG] = bishop_pair_bonus.mg;
			params[BISHOP_PAIR][EG] = bishop_pair_bonus.eg;
			params[ROOK_FULL][MG] = rook_open_file_bonus.mg;
			params[ROOK_FULL][EG] = rook_open_file_bonus.eg;
			params[ROOK_SEMI][MG] = rook_semi_open_file_bonus.mg;
			params[ROOK_SEMI][EG] = rook_semi_open_file_bonus.eg;
			params[ROOK_CLOSED][MG] = rook_closed_file_penalty.mg;
			params[ROOK_CLOSED][EG] = rook_closed_file_penalty.eg;
			params[TEMPO][MG] = tempo_bonus.mg;
			params[TEMPO][EG] = tempo_bonus.eg;
			params[KING_FULL][MG] = king_full_open_penalty.mg;
			params[KING_FULL][EG] = king_full_open_penalty.eg;
			params[KING_SEMI][MG] = king_semi_open_penalty.mg;
			params[KING_SEMI][EG] = king_semi_open_penalty.eg;
			params[KING_ADJ_FULL][MG] = king_adjacent_full_open_penalty.mg;
			params[KING_ADJ_FULL][EG] = king_adjacent_full_open_penalty.eg;
			params[KING_ADJ_SEMI][MG] = king_adjacent_semi_open_penalty.mg;
			params[KING_ADJ_SEMI][EG] = king_adjacent_semi_open_penalty.eg;
			params[KNIGHT_OUTPOST][MG] = knight_outpost_bonus.mg;
			params[KNIGHT_OUTPOST][EG] = knight_outpost_bonus.eg;
			params[BISHOP_OUTPOST][MG] = bishop_outpost_bonus.mg;
			params[BISHOP_OUTPOST][EG] = bishop_outpost_bonus.eg;
			params[WEAK_QUEEN][MG] = weak_queen_penalty.mg;
			params[WEAK_QUEEN][EG] = weak_queen_penalty.eg;
			params[ROOK_OUR_PASSER][MG] = rook_on_our_passer_file.mg;
			params[ROOK_OUR_PASSER][EG] = rook_on_our_passer_file.eg;
			params[ROOK_THEIR_PASSER][MG] = rook_on_their_passer_file.mg;
			params[ROOK_THEIR_PASSER][EG] = rook_on_their_passer_file.eg;
			params[TALL_PAWN][MG] = tall_pawn_penalty.mg;
			params[TALL_PAWN][EG] = tall_pawn_penalty.eg;
			params[FIANCHETTO][MG] = fianchetto_bonus.mg;
			params[FIANCHETTO][EG] = fianchetto_bonus.eg;
			
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				params[PAWN_SHIELD + sq][MG] = (double) pawn_shield[sq];
				params[PAWN_SHIELD + sq][EG] = 0.0;
			}
			for (PieceType pt = PIECETYPE_NONE; pt <= KING; ++pt)
			{
				params[INNER_RING + pt][MG] = (double) inner_ring_attack[pt];
				params[INNER_RING + pt][EG] = 0.0;
			}
			for (PieceType pt = PIECETYPE_NONE; pt <= KING; ++pt)
			{
				params[OUTER_RING + pt][MG] = (double) outer_ring_attack[pt];
				params[OUTER_RING + pt][EG] = 0.0;
			}
			
			params[VIRTUAL_KING_M][MG] = virtual_king_m;
			params[VIRTUAL_KING_M][EG] = 0.0;
			params[VIRTUAL_KING_B][MG] = virtual_king_b;
			params[VIRTUAL_KING_B][EG] = 0.0;
			params[SAFETY_THRESHOLD][MG] = safety_threshold;
			params[SAFETY_THRESHOLD][EG] = 0.0;
		}
		
		double linear_eval(TEntry* entry, TGradient* tg) 
		{
			double normal[PHASE_N];
			double mg[EVALTYPE_N][COLOUR_N] = {0};
			double eg[EVALTYPE_N][COLOUR_N] = {0};

			int count = 0;
			
			for (auto& it : entry->tuples)
			{
				EvalType et = count++ >= TI_SAFETY ? SAFETY : NORMAL;
				
				mg[et][WHITE] += (double) it.coefficient[WHITE] * params[it.index][MG];
				mg[et][BLACK] += (double) it.coefficient[BLACK] * params[it.index][MG];
				eg[et][WHITE] += (double) it.coefficient[WHITE] * params[it.index][EG];
				eg[et][BLACK] += (double) it.coefficient[BLACK] * params[it.index][EG];
			}

			normal[MG] = (double) mg[NORMAL][WHITE] - mg[NORMAL][BLACK];
			normal[EG] = (double) eg[NORMAL][WHITE] - eg[NORMAL][BLACK];
			
			if (tg)
			{
				tg->eval = (normal[MG] * entry->phase + normal[EG] * (MAX_GAMEPHASE - entry->phase)) / MAX_GAMEPHASE;
				tg->safety[WHITE] = mg[SAFETY][WHITE];
				tg->safety[BLACK] = mg[SAFETY][BLACK];
			}

			return (normal[MG] * entry->phase +  normal[EG] * (MAX_GAMEPHASE - entry->phase)) / MAX_GAMEPHASE;
		}
		
		double mse(long double K)
		{
			double total = 0.0;

			for (int i = 0; i < N_POSITIONS; ++i)
				total += pow(entries[i].result - sigmoid(K, linear_eval(&entries[i], NULL)), 2);

			return total / (double) entries.size();
		}
		
		double static_mse(double K) 
		{
			double total = 0.0;
		
			for (int i = 0; i < N_POSITIONS; ++i)
				total += pow(entries[i].result - sigmoid(K, entries[i].seval), 2);
			
			return total / (double) N_POSITIONS;
		}
		
		void update_single_gradient(TEntry *entry, TVector gradient, double K) 
		{
			TGradient tg;
			
			double E = linear_eval(entry, &tg);
			double S = sigmoid(K, E);
			double A = (entry->result - S) * S * (1 - S);

			for (auto& it : entry->tuples)
			{
				if (it.index < TI_SAFETY)
				{
					gradient[it.index][MG] += A * entry->phase * (it.coefficient[WHITE] - it.coefficient[BLACK]);
					gradient[it.index][EG] += A * (MAX_GAMEPHASE - entry->phase) * (it.coefficient[WHITE] - it.coefficient[BLACK]);
				}
			}
		}
		
		void compute_gradient(TVector gradient, double K) 
		{
			TVector local = {0};

			for (int i = 0; i < N_POSITIONS; ++i)
				update_single_gradient(&entries[i], local, K);

			for (int i = 0; i < TI_N; ++i) {
				gradient[i][MG] += local[i][MG];
				gradient[i][EG] += local[i][EG];
			}
		}
		
		void print_table(string name, int index, int size, int cols) 
		{
			cout << "\t\tconstexpr" << ((index < TI_SAFETY) ? " Score " : " short ") << name << "[] = {" << endl << "\t\t";

			for (int i = 0; i < size; ++i) {
				if (!(i % cols))
					cout << '\t';
				if (index < TI_SAFETY)
					cout << Score(params[index + i]) << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
				else
					cout << params[index + i][MG] << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
			}

			cout << "};" << endl << endl;
		}
		
		void print_params()
		{
			using namespace Eval;
			
			print_table("pawn_table",   PAWN_PSQT,   sizeof(pawn_table)   / sizeof(Score), 4);
			print_table("knight_table", KNIGHT_PSQT, sizeof(knight_table) / sizeof(Score), 4);
			print_table("bishop_table", BISHOP_PSQT, sizeof(bishop_table) / sizeof(Score), 4);
			print_table("rook_table",   ROOK_PSQT,   sizeof(rook_table)   / sizeof(Score), 4);
			print_table("queen_table",  QUEEN_PSQT,  sizeof(queen_table)  / sizeof(Score), 4);
			print_table("king_table",   KING_PSQT,   sizeof(king_table)   / sizeof(Score), 4);
			print_table("passed_pawn",  PASSED_PAWN, sizeof(passed_pawn)  / sizeof(Score), 4);
			print_table("mobility", MOBILITY, 7, 7);
			
			cout << "\t\tconstexpr Score double_pawn_penalty = "        << Score(params[DOUBLE_PAWN])       << ";" << endl
			<< "\t\tconstexpr Score isolated_pawn_penalty = "           << Score(params[ISOLATED_PAWN])     << ";" << endl
			<< "\t\tconstexpr Score bishop_pair_bonus = "               << Score(params[BISHOP_PAIR])       << ";" << endl
			<< "\t\tconstexpr Score rook_open_file_bonus = "	    << Score(params[ROOK_FULL])         << ";" << endl
			<< "\t\tconstexpr Score rook_semi_open_file_bonus = "       << Score(params[ROOK_SEMI])         << ";" << endl
			<< "\t\tconstexpr Score rook_closed_file_penalty = "        << Score(params[ROOK_CLOSED])       << ";" << endl
			<< "\t\tconstexpr Score tempo_bonus = "                     << Score(params[TEMPO])             << ";" << endl
			<< "\t\tconstexpr Score king_full_open_penalty = "          << Score(params[KING_FULL])         << ";" << endl
			<< "\t\tconstexpr Score king_semi_open_penalty = "          << Score(params[KING_SEMI])         << ";" << endl
			<< "\t\tconstexpr Score king_adjacent_full_open_penalty = " << Score(params[KING_ADJ_FULL])     << ";" << endl
			<< "\t\tconstexpr Score king_adjacent_semi_open_penalty = " << Score(params[KING_ADJ_SEMI])     << ";" << endl
			<< "\t\tconstexpr Score knight_outpost_bonus = "            << Score(params[KNIGHT_OUTPOST])    << ";" << endl
			<< "\t\tconstexpr Score bishop_outpost_bonus = "            << Score(params[BISHOP_OUTPOST])    << ";" << endl
			<< "\t\tconstexpr Score weak_queen_penalty = "              << Score(params[WEAK_QUEEN])        << ";" << endl
			<< "\t\tconstexpr Score rook_on_our_passer_file = "         << Score(params[ROOK_OUR_PASSER])   << ";" << endl
			<< "\t\tconstexpr Score rook_on_their_passer_file = "       << Score(params[ROOK_THEIR_PASSER]) << ";" << endl
			<< "\t\tconstexpr Score tall_pawn_penalty = "               << Score(params[TALL_PAWN])         << ";" << endl
			<< "\t\tconstexpr Score fianchetto_bonus = "                << Score(params[FIANCHETTO])        << ";" << endl << endl;
			
			print_table("pawn_shield",   PAWN_SHIELD,   sizeof(pawn_shield)   / sizeof(short), 4);
			print_table("inner_ring_attack", INNER_RING, 7, 7);
			print_table("outer_ring_attack", OUTER_RING, 7, 7);
			
			cout << "\t\tconstexpr virtual_king_m = "   << params[VIRTUAL_KING_M][MG]   << ";" << endl
			<< "\t\tconstexpr virtual_king_b = "        << params[VIRTUAL_KING_B][MG]   << ";" << endl
			<< "\t\tconstexpr safety_threshold = "      << params[SAFETY_THRESHOLD][MG] << ";" << endl;
		}
		
		double find_k()
		{
			double start = -10, end = 10, step = 1;
			double curr = start, error, best = static_mse(start);

			for (int epoch = 0; epoch < 10; ++epoch) 
			{
				curr = start - step;
				
				while (curr < end) 
				{
					curr = curr + step;
					error = static_mse(curr);
					
					if (error <= best)
						best = error, start = curr;
				}
				cout.precision(17);
				cout << "Epoch [" << epoch << "] Error = [" << best << "], K = [" << start << "]" << endl;

				end = start + step;
				start -= step;
				step /= 10.0;
			}

			return start;
		}
		
		void tune_eval()
		{
			TVector adagrad = {0};
			init_params();
			
			string file_name = "src/tuner.epd";
			ifstream ifs;
			ifs.open(file_name.c_str(), ifstream::in);
			string line;
			
			entries.resize(N_POSITIONS);

			for(int i = 0; i < N_POSITIONS; ++i)
			{
				if (ifs.eof())
					break;
				getline(ifs, line);
				if (line.length())
				{
					for(int j=0;j<TI_N;++j)
					{
						Eval::T[j][0] = 0;
						Eval::T[j][1] = 0;
					}
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
					
					entries[i].phase = pos.get_game_phase();
					
					entries[i].seval = Eval::evaluate<true>(pos);
					if (pos.side == BLACK) entries[i].seval = -entries[i].seval;
					
					entries[i].stm = pos.side;
					
					for (int j = 0; j < TI_N; ++j)
						if ((j < TI_SAFETY && Eval::T[j][WHITE] - Eval::T[j][BLACK] != 0)
							|| (j >= TI_SAFETY && (Eval::T[j][WHITE] != 0 || Eval::T[j][BLACK] != 0)))
							entries[i].tuples.push_back(TTuple(j, Eval::T[j][WHITE], Eval::T[j][BLACK]));
					//cout << "static " << entries[i].seval << endl;
					//cout << "linear " << (int) linear_eval(&entries[i], NULL) << endl;
					//if(entries[i].seval != (int) linear_eval(&entries[i], NULL))
					//{
					//	cout << i << endl;
					//	exit(1);
					//}
				}
			}

			ifs.close();
			
			double K = find_k();
			double rate = 0.10;
			
			cout << mse(K) << endl;
			cout << static_mse(K) << endl;
			
			for (int epoch = 0; epoch < MAX_EPOCHS; ++epoch) 
			{
				TVector gradient = {0};
				compute_gradient(gradient, K);

				for (int i = 0; i < TI_N; ++i) 
				{
					adagrad[i][MG] += pow((K / 200.0) * gradient[i][MG] / 16384, 2.0);
					adagrad[i][EG] += pow((K / 200.0) * gradient[i][EG] / 16384, 2.0);
					
					params[i][MG] += (K / 200.0) * (gradient[i][MG] / 16384) * (rate / sqrt(1e-8 + adagrad[i][MG]));
					params[i][EG] += (K / 200.0) * (gradient[i][EG] / 16384) * (rate / sqrt(1e-8 + adagrad[i][EG]));
					
					params[i][MG] = max(0.0, params[i][MG]);
					params[i][EG] = max(0.0, params[i][EG]);
				}

				double error = mse(K);
				
				if (epoch && epoch % 250 == 0) 
					rate = rate / 1.00;
				if (epoch % 100 == 0)
					print_params();

				cout << "Epoch [" << epoch << "] Error = [" << error << "], Rate = [" << rate << "]" << endl;
			}
		}

	} // namespace Tuner

} // namespace Clovis
