#include <thread>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>

#include "tuner.h"

using namespace std;

namespace clovis {

	namespace tuner {

		using TVector = array<array<double, PHASE_N>, TI_MISC>;
		
		vector<TEntry> entries;
		TVector params;

		constexpr int N_CORES = 8;
		constexpr int MAX_EPOCHS = 1000000;

		inline double sigmoid(const double K, const double E) {
			return 1.0 / (1.0 + exp(-K * E / 400.0));
		}

		template<typename T>
		void add_param(T t, const TraceIndex ti) {

			if constexpr (is_same<T, Score>()) {
				assert(ti < TI_SAFETY);
				params[ti][MG] = t.mg;
				params[ti][EG] = t.eg;
			} else {
				static_assert(is_same<T, short>());
				assert(ti >= TI_SAFETY);
				params[ti][MG] = t;
				params[ti][EG] = 0.0;
			}
		}
		
		void init_params() {

			using namespace eval;
			
			for (auto& it : pawn_source)
				add_param<Score>(it, TraceIndex(&it + PAWN_PSQT - pawn_source));
			for (auto& it : knight_source)
				add_param<Score>(it, TraceIndex(&it + KNIGHT_PSQT - knight_source));
			for (auto& it : bishop_source)
				add_param<Score>(it, TraceIndex(&it + BISHOP_PSQT - bishop_source));
			for (auto& it : rook_source)
				add_param<Score>(it, TraceIndex(&it + ROOK_PSQT - rook_source));
			for (auto& it : queen_source)
				add_param<Score>(it, TraceIndex(&it + QUEEN_PSQT - queen_source));
			for (auto& it : king_source)
				add_param<Score>(it, TraceIndex(&it + KING_PSQT - king_source));
			for (auto& it : passed_pawn)
				add_param<Score>(it, TraceIndex(&it + PASSED_PAWN - passed_pawn));
			for (auto& it : candidate_passer)
				add_param<Score>(it, TraceIndex(&it + CANDIDATE_PASSER - candidate_passer));
			for (auto& it : quiet_mobility)
				add_param<Score>(it, TraceIndex(&it + QUIET_MOBILITY - quiet_mobility));
			for (auto& it : capture_mobility)
				add_param<Score>(it, TraceIndex(&it + CAPTURE_MOBILITY - capture_mobility));
			
			add_param<Score>(double_pawn_penalty, DOUBLE_PAWN);
			add_param<Score>(isolated_pawn_penalty, ISOLATED_PAWN);
			add_param<Score>(bishop_pair_bonus, BISHOP_PAIR);
			add_param<Score>(rook_open_file_bonus, ROOK_FULL);
			add_param<Score>(rook_semi_open_file_bonus, ROOK_SEMI);
			add_param<Score>(rook_closed_file_penalty, ROOK_CLOSED);
			add_param<Score>(tempo_bonus, TEMPO);
			add_param<Score>(king_open_penalty, KING_OPEN);
			add_param<Score>(king_adjacent_open_penalty, KING_ADJ_OPEN);
			add_param<Score>(knight_outpost_bonus, KNIGHT_OUTPOST);
			add_param<Score>(bishop_outpost_bonus, BISHOP_OUTPOST);
			add_param<Score>(weak_queen_penalty, WEAK_QUEEN);
			add_param<Score>(rook_on_our_passer_file, ROOK_OUR_PASSER);
			add_param<Score>(rook_on_their_passer_file, ROOK_THEIR_PASSER);
			add_param<Score>(tall_pawn_penalty, TALL_PAWN);
			add_param<Score>(fianchetto_bonus, FIANCHETTO);
			add_param<Score>(rook_on_seventh, ROOK_ON_SEVENTH);
			
			for (auto& it : pawn_shield)
				add_param<short>(it, TraceIndex(&it + SAFETY_PAWN_SHIELD - pawn_shield));
			for (auto& it : inner_ring_attack)
				add_param<short>(it, TraceIndex(&it + SAFETY_INNER_RING - inner_ring_attack));
			for (auto& it : outer_ring_attack)
				add_param<short>(it, TraceIndex(&it + SAFETY_OUTER_RING - outer_ring_attack));
			
			add_param<short>(virtual_mobility, SAFETY_VIRTUAL_MOBILITY);
			add_param<short>(attack_factor, SAFETY_N_ATT);
		}
		
		double linear_eval(const TEntry& entry, TGradient* tg) {

			array<double, PHASE_N> normal{};
			double safety = 0.0;
			array<array<double, COLOUR_N>, EVALTYPE_N> mg{};
			array<array<double, COLOUR_N>, EVALTYPE_N> eg{};
			
			for (auto& it : entry.tuples) {

				EvalType et = it.index >= TI_SAFETY ? SAFETY : NORMAL;
				
				mg[et][WHITE] += it.coefficient[WHITE] * params[it.index][MG];
				mg[et][BLACK] += it.coefficient[BLACK] * params[it.index][MG];
				eg[et][WHITE] += it.coefficient[WHITE] * params[it.index][EG];
				eg[et][BLACK] += it.coefficient[BLACK] * params[it.index][EG];
			}

			normal[MG] = mg[NORMAL][WHITE] - mg[NORMAL][BLACK];
			normal[EG] = eg[NORMAL][WHITE] - eg[NORMAL][BLACK];
			
			safety += mg[SAFETY][WHITE] * mg[SAFETY][WHITE] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[WHITE]);
			safety -= mg[SAFETY][BLACK] * mg[SAFETY][BLACK] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[BLACK]);

			const double eval =  ((normal[MG] + safety) * entry.phase + normal[EG] * (MAX_GAME_PHASE - entry.phase)) / MAX_GAME_PHASE;
			
			if (tg) {
				tg->eval = eval;
				tg->safety[WHITE] = mg[SAFETY][WHITE];
				tg->safety[BLACK] = mg[SAFETY][BLACK];
			}

			return eval;
		}
		
		template<bool STATIC>
		double mse(const double K) {
			
			double total = 0.0;

			#pragma omp parallel shared(total)
			{
				#pragma omp for schedule(static, entries.size() / N_CORES) reduction(+:total)
				for (const auto& it : entries)
					total += pow(it.result - sigmoid(K, (STATIC ? it.static_eval : linear_eval(it, nullptr))), 2);
			}

			return total / entries.size();
		}
		
		void update_single_gradient(const TEntry& entry, TVector gradient, double K) {
			
			TGradient tg;
			
			const double E = linear_eval(entry, &tg);
			const double S = sigmoid(K, E);
			const double A = (entry.result - S) * S * (1 - S);
			
			const double base[PHASE_N] = { A * entry.phase, A * (MAX_GAME_PHASE - entry.phase) };

			for (auto& it : entry.tuples) {
				if (it.index < TI_SAFETY) {
					gradient[it.index][MG] += base[MG] * (it.coefficient[WHITE] - it.coefficient[BLACK]);
					gradient[it.index][EG] += base[EG] * (it.coefficient[WHITE] - it.coefficient[BLACK]);
				} else {
					gradient[it.index][MG] += 2 * base[MG] * tg.safety[WHITE] * it.coefficient[WHITE] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[WHITE]);
					gradient[it.index][MG] -= 2 * base[MG] * tg.safety[BLACK] * it.coefficient[BLACK] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[BLACK]);
				}
			}
			
			gradient[SAFETY_N_ATT][MG] += base[MG] * pow(tg.safety[WHITE], 2.0) * entry.n_att[WHITE] / pow(720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[WHITE], 2.0);
			gradient[SAFETY_N_ATT][MG] -= base[MG] * pow(tg.safety[BLACK], 2.0) * entry.n_att[BLACK] / pow(720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[BLACK], 2.0);
			
		}
		
		void compute_gradient(TVector gradient, double K) {

			TVector local{};

			for (auto& entry : entries)
				update_single_gradient(entry, local, K);

			for (int i = 0; i < TI_MISC; ++i) {
				gradient[i][MG] += local[i][MG];
				gradient[i][EG] += local[i][EG];
			}
		}
		
		void print_table(const string& name, const int index, const int size, const int cols) {
			
			cout << "\t\tconstexpr" << ((index < TI_SAFETY) ? " Score " : " short ") << name << "[] = {" << endl << "\t\t";

			for (int i = 0; i < size; ++i) {
				if (!(i % cols))
					cout << '\t';
				if (index < TI_SAFETY)
					cout << Score(params[index + i])     << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
				else
					cout << round(params[index + i][MG]) << "," << ((i % cols == (cols - 1)) ? "\n\t\t" : " ");
			}

			cout << "};" << endl << endl;
		}
		
		void print_params() {

			using namespace eval;
			
			print_table("pawn_source",      PAWN_PSQT,        sizeof(pawn_source)      / sizeof(Score), 4);
			print_table("knight_source",    KNIGHT_PSQT,      sizeof(knight_source)    / sizeof(Score), 4);
			print_table("bishop_source",    BISHOP_PSQT,      sizeof(bishop_source)    / sizeof(Score), 4);
			print_table("rook_source",      ROOK_PSQT,        sizeof(rook_source)      / sizeof(Score), 4);
			print_table("queen_source",     QUEEN_PSQT,       sizeof(queen_source)     / sizeof(Score), 4);
			print_table("king_source",      KING_PSQT,        sizeof(king_source)      / sizeof(Score), 4);
			print_table("passed_pawn",      PASSED_PAWN,      sizeof(passed_pawn)      / sizeof(Score), 4);
			print_table("candidate_passer", CANDIDATE_PASSER, sizeof(candidate_passer) / sizeof(Score), 8);
			print_table("quiet_mobility",   QUIET_MOBILITY,   7, 7);
			print_table("capture_mobility", CAPTURE_MOBILITY, 7, 7);
			
			cout << "\t\tconstexpr Score double_pawn_penalty = "   << Score(params[DOUBLE_PAWN])       << ";" << endl
			<< "\t\tconstexpr Score isolated_pawn_penalty = "      << Score(params[ISOLATED_PAWN])     << ";" << endl
			<< "\t\tconstexpr Score bishop_pair_bonus = "          << Score(params[BISHOP_PAIR])       << ";" << endl
			<< "\t\tconstexpr Score rook_open_file_bonus = "       << Score(params[ROOK_FULL])         << ";" << endl
			<< "\t\tconstexpr Score rook_semi_open_file_bonus = "  << Score(params[ROOK_SEMI])         << ";" << endl
			<< "\t\tconstexpr Score rook_closed_file_penalty = "   << Score(params[ROOK_CLOSED])       << ";" << endl
			<< "\t\tconstexpr Score tempo_bonus = "                << Score(params[TEMPO])             << ";" << endl
			<< "\t\tconstexpr Score king_open_penalty = "          << Score(params[KING_OPEN])         << ";" << endl
			<< "\t\tconstexpr Score king_adjacent_open_penalty = " << Score(params[KING_ADJ_OPEN])     << ";" << endl
			<< "\t\tconstexpr Score knight_outpost_bonus = "       << Score(params[KNIGHT_OUTPOST])    << ";" << endl
			<< "\t\tconstexpr Score bishop_outpost_bonus = "       << Score(params[BISHOP_OUTPOST])    << ";" << endl
			<< "\t\tconstexpr Score weak_queen_penalty = "         << Score(params[WEAK_QUEEN])        << ";" << endl
			<< "\t\tconstexpr Score rook_on_our_passer_file = "    << Score(params[ROOK_OUR_PASSER])   << ";" << endl
			<< "\t\tconstexpr Score rook_on_their_passer_file = "  << Score(params[ROOK_THEIR_PASSER]) << ";" << endl
			<< "\t\tconstexpr Score tall_pawn_penalty = "          << Score(params[TALL_PAWN])         << ";" << endl
			<< "\t\tconstexpr Score fianchetto_bonus = "           << Score(params[FIANCHETTO])        << ";" << endl
			<< "\t\tconstexpr Score rook_on_seventh = "            << Score(params[ROOK_ON_SEVENTH])   << ";" << endl << endl;
			
			print_table("pawn_shield", SAFETY_PAWN_SHIELD, sizeof(pawn_shield) / sizeof(short), 4);
			print_table("inner_ring_attack", SAFETY_INNER_RING, 7, 7);
			print_table("outer_ring_attack", SAFETY_OUTER_RING, 7, 7);
			
			cout << "\t\tconstexpr short attack_factor = " << round(params[SAFETY_N_ATT][MG])            << ";" << endl 
			<< "\t\tconstexpr short virtual_mobility = "   << round(params[SAFETY_VIRTUAL_MOBILITY][MG]) << ";" << endl;
		}
		
		double find_k() {

			double start = -10, end = 10, step = 1, best = mse<true>(start);

			for (int epoch = 0; epoch < 10; ++epoch) {
				double curr = start - step;
				
				while (curr < end) {
					curr = curr + step;
					if (const double error = mse<true>(curr); error <= best)
						best = codecvt_base::error, start = curr;
				}

				cout.precision(17);
				cout << "Epoch [" << epoch << "] Error = [" << best << "], K = [" << start << "]" << endl;

				end = start + step;
				start -= step;
				step /= 10.0;
			}

			return start;
		}
		
		void tune_eval() {

			init_params();
			
			TVector adagrad = {0};
			ifstream ifs("src/tuner.epd");
			string line;

			while (getline(ifs, line)) {

				if (line.empty()) continue;

				TEntry entry;
				memset(eval::T.data(), 0, sizeof(eval::T));
				const size_t idx = line.find('\"');
				const size_t idx_end = line.find('\"', idx + 1);

				if (const string res = line.substr(idx + 1, idx_end - idx - 1); res == "1-0")
					entry.result = 1.0;
				else if (res == "0-1")
					entry.result = 0.0;
				else if (res == "1/2-1/2")
					entry.result = 0.5;
				else exit(EXIT_FAILURE);

				Position pos(line.substr(0, idx).c_str());

				entry.phase = pos.get_game_phase();

				entry.static_eval = eval::evaluate<true>(pos);
				if (pos.side == BLACK) entry.static_eval = -entry.static_eval;

				entry.stm = pos.side;

				entry.n_att[WHITE] = eval::T[SAFETY_N_ATT][WHITE];
				entry.n_att[BLACK] = eval::T[SAFETY_N_ATT][BLACK];

				for (int j = 0; j < TI_N; ++j)
					if ((j < TI_SAFETY && eval::T[j][WHITE] - eval::T[j][BLACK] != 0)
						|| (j >= TI_SAFETY && (eval::T[j][WHITE] != 0 || eval::T[j][BLACK] != 0)))
						entry.tuples.emplace_back(j, eval::T[j][WHITE], eval::T[j][BLACK]);

				entries.push_back(entry);
			}

			ifs.close();
			
			const double K = find_k();
			double rate = 1.0;
			
			cout << mse<true> (K) << endl;
			cout << mse<false>(K) << endl;
			
			for (int epoch = 1; epoch < MAX_EPOCHS; ++epoch) {

				TVector gradient = {0};
				compute_gradient(gradient, K);

				for (int i = 0; i < TI_MISC; ++i) {

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

	} // namespace tuner

} // namespace clovis
