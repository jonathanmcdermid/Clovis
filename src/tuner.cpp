#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {
	
		typedef double TVector[TI_MISC][PHASE_N];

		vector<TEntry> entries;
		TVector params;

		constexpr int N_CORES = 8;
		constexpr int MAX_EPOCHS = 1000000;

		inline double sigmoid(double K, double E) {
			return 1.0 / (1.0 + exp(-K * E / 400.0));
		}

		template<typename T>
		void add_param(T t, TraceIndex ti)
		{
			if constexpr (is_same<T, Score>())
			{
				assert(ti < TI_SAFETY);
				params[ti][MG] = (double) t.mg;
				params[ti][EG] = (double) t.eg;
			}
			else
			{
				static_assert(is_same<T, short>());
				assert(ti >= TI_SAFETY);
				params[ti][MG] = (double) t;
				params[ti][EG] = 0.0;
			}
		}
		
		void init_params()
		{
			using namespace Eval;
			
			for (Square sq = SQ_ZERO; sq < int(sizeof(pawn_table) / sizeof(Score)); ++sq)
				add_param<Score>(pawn_table[sq], TraceIndex(PAWN_PSQT + sq));
			for (Square sq = SQ_ZERO; sq < int(sizeof(knight_table) / sizeof(Score)); ++sq)
				add_param<Score>(knight_table[sq], TraceIndex(KNIGHT_PSQT + sq));
			for (Square sq = SQ_ZERO; sq < int(sizeof(bishop_table) / sizeof(Score)); ++sq)
				add_param<Score>(bishop_table[sq], TraceIndex(BISHOP_PSQT + sq));
			for (Square sq = SQ_ZERO; sq < int(sizeof(rook_table) / sizeof(Score)); ++sq)
				add_param<Score>(rook_table[sq], TraceIndex(ROOK_PSQT + sq));
			for (Square sq = SQ_ZERO; sq < int(sizeof(queen_table) / sizeof(Score)); ++sq)
				add_param<Score>(queen_table[sq], TraceIndex(QUEEN_PSQT + sq));
			for (Square sq = SQ_ZERO; sq < int(sizeof(king_table) / sizeof(Score)); ++sq)
				add_param<Score>(king_table[sq], TraceIndex(KING_PSQT + sq));
			for (Square sq = SQ_ZERO; sq < int(sizeof(passed_pawn) / sizeof(Score)); ++sq)
				add_param<Score>(passed_pawn[sq], TraceIndex(PASSED_PAWN + sq));
			for (Rank r = RANK_1; r < int(sizeof(candidate_passer) / sizeof(Score)); ++r)
				add_param<Score>(candidate_passer[r], TraceIndex(CANDIDATE_PASSER + r));
			for (PieceType pt = PIECETYPE_NONE; pt <= KING; ++pt)
				add_param<Score>(quiet_mobility[pt], TraceIndex(QUIET_MOBILITY + pt));
			for (PieceType pt = PIECETYPE_NONE; pt <= KING; ++pt)
				add_param<Score>(capture_mobility[pt], TraceIndex(CAPTURE_MOBILITY + pt));
			
			add_param<Score>(double_pawn_penalty, DOUBLE_PAWN);
			add_param<Score>(isolated_pawn_penalty, ISOLATED_PAWN);
			add_param<Score>(bishop_pair_bonus, BISHOP_PAIR);
			add_param<Score>(rook_open_file_bonus, ROOK_FULL);
			add_param<Score>(rook_semi_open_file_bonus, ROOK_SEMI);
			add_param<Score>(rook_closed_file_penalty, ROOK_CLOSED);
			add_param<Score>(tempo_bonus, TEMPO);
			add_param<Score>(king_full_open_penalty, KING_FULL);
			add_param<Score>(king_adjacent_full_open_penalty, KING_ADJ_FULL);
			add_param<Score>(knight_outpost_bonus, KNIGHT_OUTPOST);
			add_param<Score>(bishop_outpost_bonus, BISHOP_OUTPOST);
			add_param<Score>(weak_queen_penalty, WEAK_QUEEN);
			add_param<Score>(rook_on_our_passer_file, ROOK_OUR_PASSER);
			add_param<Score>(rook_on_their_passer_file, ROOK_THEIR_PASSER);
			add_param<Score>(tall_pawn_penalty, TALL_PAWN);
			add_param<Score>(fianchetto_bonus, FIANCHETTO);
			add_param<Score>(rook_on_seventh, ROOK_ON_SEVENTH);
			
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
				add_param<short>(pawn_shield[sq], TraceIndex(SAFETY_PAWN_SHIELD + sq));
			for (PieceType pt = PIECETYPE_NONE; pt <= KING; ++pt)
				add_param<short>(inner_ring_attack[pt], TraceIndex(SAFETY_INNER_RING + pt));
			for (PieceType pt = PIECETYPE_NONE; pt <= KING; ++pt)
				add_param<short>(outer_ring_attack[pt], TraceIndex(SAFETY_OUTER_RING + pt));
			
			add_param<short>(virtual_mobility, SAFETY_VIRTUAL_MOBILITY);
			add_param<short>(attack_factor, SAFETY_N_ATT);
		}
		
		double linear_eval(const TEntry* entry, TGradient* tg) 
		{
			double normal[PHASE_N];
			double safety = 0.0;
			double mg[EVALTYPE_N][COLOUR_N] = {0};
			double eg[EVALTYPE_N][COLOUR_N] = {0};
			
			for (auto& it : entry->tuples)
			{
				EvalType et = it.index >= TI_SAFETY ? SAFETY : NORMAL;
				
				mg[et][WHITE] += (double) it.coefficient[WHITE] * params[it.index][MG];
				mg[et][BLACK] += (double) it.coefficient[BLACK] * params[it.index][MG];
				eg[et][WHITE] += (double) it.coefficient[WHITE] * params[it.index][EG];
				eg[et][BLACK] += (double) it.coefficient[BLACK] * params[it.index][EG];
			}

			normal[MG] = (double) mg[NORMAL][WHITE] - mg[NORMAL][BLACK];
			normal[EG] = (double) eg[NORMAL][WHITE] - eg[NORMAL][BLACK];
			
			safety += mg[SAFETY][WHITE] * mg[SAFETY][WHITE] / (720.0 - params[SAFETY_N_ATT][MG] * entry->n_att[WHITE]);
			safety -= mg[SAFETY][BLACK] * mg[SAFETY][BLACK] / (720.0 - params[SAFETY_N_ATT][MG] * entry->n_att[BLACK]);
			
			if (tg)
			{
				tg->eval = ((normal[MG] + safety) * entry->phase + normal[EG] * (MAX_GAMEPHASE - entry->phase)) / MAX_GAMEPHASE;
				tg->safety[WHITE] = mg[SAFETY][WHITE];
				tg->safety[BLACK] = mg[SAFETY][BLACK];
			}

			return ((normal[MG] + safety) * entry->phase +  normal[EG] * (MAX_GAMEPHASE - entry->phase)) / MAX_GAMEPHASE;
		}
		
		template<bool STATIC>
		double mse(long double K)
		{
			double total = 0.0;

			#pragma omp parallel shared(total)
			{
				#pragma omp for schedule(static, entries.size() / N_CORES) reduction(+:total)
				for (const auto& it : entries)
					total += pow(it.result - sigmoid(K, (STATIC ? it.seval : linear_eval(&it, NULL))), 2);
			}

			return total / (double) entries.size();
		}
		
		void update_single_gradient(TEntry *entry, TVector gradient, double K) 
		{
			TGradient tg;
			
			double E = linear_eval(entry, &tg);
			double S = sigmoid(K, E);
			double A = (entry->result - S) * S * (1 - S);
			
			double base[PHASE_N] = { A * entry->phase, A * (MAX_GAMEPHASE - entry->phase) };

			for (auto& it : entry->tuples)
			{
				if (it.index < TI_SAFETY)
				{
					gradient[it.index][MG] += base[MG] * (it.coefficient[WHITE] - it.coefficient[BLACK]);
					gradient[it.index][EG] += base[EG] * (it.coefficient[WHITE] - it.coefficient[BLACK]);
				}
				else
				{
					gradient[it.index][MG] += 2 * base[MG] * tg.safety[WHITE] * it.coefficient[WHITE] / (720.0 - params[SAFETY_N_ATT][MG] * entry->n_att[WHITE]);
					gradient[it.index][MG] -= 2 * base[MG] * tg.safety[BLACK] * it.coefficient[BLACK] / (720.0 - params[SAFETY_N_ATT][MG] * entry->n_att[BLACK]);
				}
			}
			
			gradient[SAFETY_N_ATT][MG] += base[MG] * pow(tg.safety[WHITE], 2.0) * entry->n_att[WHITE] / pow(720.0 - params[SAFETY_N_ATT][MG] * entry->n_att[WHITE], 2.0);
			gradient[SAFETY_N_ATT][MG] -= base[MG] * pow(tg.safety[BLACK], 2.0) * entry->n_att[BLACK] / pow(720.0 - params[SAFETY_N_ATT][MG] * entry->n_att[BLACK], 2.0);
			
		}
		
		void compute_gradient(TVector gradient, double K) 
		{
			TVector local = {0};

			for (size_t i = 0; i < entries.size(); ++i)
				update_single_gradient(&entries[i], local, K);

			for (int i = 0; i < TI_MISC; ++i) 
			{
				gradient[i][MG] += local[i][MG];
				gradient[i][EG] += local[i][EG];
			}
		}
		
		void print_table(string name, int index, int size, int cols) 
		{
			cout << "\t\tconstexpr" << ((index < TI_SAFETY) ? " Score " : " short ") << name << "[] = {" << endl << "\t\t";

			for (int i = 0; i < size; ++i) 
			{
				if (!(i % cols))
					cout << '\t';
				if (index < TI_SAFETY)
					cout << Score(params[index + i]) << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
				else
					cout << round(params[index + i][MG]) << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
			}

			cout << "};" << endl << endl;
		}
		
		void print_params()
		{
			using namespace Eval;
			
			print_table("pawn_table",       PAWN_PSQT,        sizeof(pawn_table)       / sizeof(Score), 4);
			print_table("knight_table",     KNIGHT_PSQT,      sizeof(knight_table)     / sizeof(Score), 4);
			print_table("bishop_table",     BISHOP_PSQT,      sizeof(bishop_table)     / sizeof(Score), 4);
			print_table("rook_table",       ROOK_PSQT,        sizeof(rook_table)       / sizeof(Score), 4);
			print_table("queen_table",      QUEEN_PSQT,       sizeof(queen_table)      / sizeof(Score), 4);
			print_table("king_table",       KING_PSQT,        sizeof(king_table)       / sizeof(Score), 4);
			print_table("passed_pawn",      PASSED_PAWN,      sizeof(passed_pawn)      / sizeof(Score), 4);
			print_table("candidate_passer", CANDIDATE_PASSER, sizeof(candidate_passer) / sizeof(Score), 8);
			print_table("quiet_mobility",   QUIET_MOBILITY,   7, 7);
			print_table("capture_mobility", CAPTURE_MOBILITY, 7, 7);
			
			cout << "\t\tconstexpr Score double_pawn_penalty = "        << Score(params[DOUBLE_PAWN])       << ";" << endl
			<< "\t\tconstexpr Score isolated_pawn_penalty = "           << Score(params[ISOLATED_PAWN])     << ";" << endl
			<< "\t\tconstexpr Score bishop_pair_bonus = "               << Score(params[BISHOP_PAIR])       << ";" << endl
			<< "\t\tconstexpr Score rook_open_file_bonus = "	    << Score(params[ROOK_FULL])         << ";" << endl
			<< "\t\tconstexpr Score rook_semi_open_file_bonus = "       << Score(params[ROOK_SEMI])         << ";" << endl
			<< "\t\tconstexpr Score rook_closed_file_penalty = "        << Score(params[ROOK_CLOSED])       << ";" << endl
			<< "\t\tconstexpr Score tempo_bonus = "                     << Score(params[TEMPO])             << ";" << endl
			<< "\t\tconstexpr Score king_full_open_penalty = "          << Score(params[KING_FULL])         << ";" << endl
			<< "\t\tconstexpr Score king_adjacent_full_open_penalty = " << Score(params[KING_ADJ_FULL])     << ";" << endl
			<< "\t\tconstexpr Score knight_outpost_bonus = "            << Score(params[KNIGHT_OUTPOST])    << ";" << endl
			<< "\t\tconstexpr Score bishop_outpost_bonus = "            << Score(params[BISHOP_OUTPOST])    << ";" << endl
			<< "\t\tconstexpr Score weak_queen_penalty = "              << Score(params[WEAK_QUEEN])        << ";" << endl
			<< "\t\tconstexpr Score rook_on_our_passer_file = "         << Score(params[ROOK_OUR_PASSER])   << ";" << endl
			<< "\t\tconstexpr Score rook_on_their_passer_file = "       << Score(params[ROOK_THEIR_PASSER]) << ";" << endl
			<< "\t\tconstexpr Score tall_pawn_penalty = "               << Score(params[TALL_PAWN])         << ";" << endl
			<< "\t\tconstexpr Score fianchetto_bonus = "                << Score(params[FIANCHETTO])        << ";" << endl
			<< "\t\tconstexpr Score rook_on_seventh = "                 << Score(params[ROOK_ON_SEVENTH])   << ";" << endl << endl;
			
			print_table("pawn_shield",   SAFETY_PAWN_SHIELD,   sizeof(pawn_shield)   / sizeof(short), 4);
			print_table("inner_ring_attack", SAFETY_INNER_RING, 7, 7);
			print_table("outer_ring_attack", SAFETY_OUTER_RING, 7, 7);
			
			cout << "\t\tconstexpr short virtual_mobility = " << round(params[SAFETY_VIRTUAL_MOBILITY][MG]) << ";" << endl
			<< "\t\tconstexpr short attack_factor = "         << round(params[SAFETY_N_ATT][MG])            << ";" << endl;
		}
		
		double find_k()
		{
			double start = -10, end = 10, step = 1;
			double curr = start, error, best = mse<true>(start);

			for (int epoch = 0; epoch < 10; ++epoch) 
			{
				curr = start - step;
				
				while (curr < end) 
				{
					curr = curr + step;
					error = mse<true>(curr);
					
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

			while (!ifs.eof()) 
			{
				getline(ifs, line);
				
				if (line.length())
				{
					TEntry entry;
					memset(Eval::T, 0, sizeof(Eval::T));
					size_t idx = line.find("\"");
					size_t idx_end = line.find("\"", idx + 1);
					string res = line.substr(idx + 1, idx_end - idx - 1);

					if (res == "1-0")
						entry.result = 1.0;
					else if (res == "0-1")
						entry.result = 0.0;
					else if (res == "1/2-1/2")
						entry.result = 0.5;
					else exit(EXIT_FAILURE);
					
					Position pos(line.substr(0, idx).c_str());
					
					entry.phase = pos.get_game_phase();
					
					entry.seval = Eval::evaluate<true>(pos);
					if (pos.side == BLACK) entry.seval = -entry.seval;
					
					entry.stm = pos.side;

					entry.n_att[WHITE] = Eval::T[SAFETY_N_ATT][WHITE];
					entry.n_att[BLACK] = Eval::T[SAFETY_N_ATT][BLACK];
					
					for (int j = 0; j < TI_N; ++j)
						if ((j < TI_SAFETY && Eval::T[j][WHITE] - Eval::T[j][BLACK] != 0)
							|| (j >= TI_SAFETY && (Eval::T[j][WHITE] != 0 || Eval::T[j][BLACK] != 0)))
							entry.tuples.push_back(TTuple(j, Eval::T[j][WHITE], Eval::T[j][BLACK]));

					entries.push_back(entry);
				}
			}

			ifs.close();
			
			double K = find_k();
			double rate = 1.0;
			
			cout << mse<true>(K) << endl;
			cout << mse<false>(K) << endl;
			
			for (int epoch = 1; epoch < MAX_EPOCHS; ++epoch) 
			{
				TVector gradient = {0};
				compute_gradient(gradient, K);

				for (int i = 0; i < TI_MISC; ++i) 
				{
					adagrad[i][MG] += pow((K / 200.0) * gradient[i][MG] / 16384, 2.0);
					adagrad[i][EG] += pow((K / 200.0) * gradient[i][EG] / 16384, 2.0);
					
					params[i][MG] += (K / 200.0) * (gradient[i][MG] / 16384) * (rate / sqrt(1e-8 + adagrad[i][MG]));
					params[i][EG] += (K / 200.0) * (gradient[i][EG] / 16384) * (rate / sqrt(1e-8 + adagrad[i][EG]));
					
					params[i][MG] = max(0.0, params[i][MG]);
					params[i][EG] = max(0.0, params[i][EG]);
				}
				
				if (epoch && epoch % 250 == 0) 
					rate = rate / 1.01;
				if (epoch % 100 == 0)
					print_params();

				cout << "Epoch [" << epoch << "] Error = [" << mse<false>(K) << "], Rate = [" << rate << "]" << endl;
			}
		}

	} // namespace Tuner

} // namespace Clovis
