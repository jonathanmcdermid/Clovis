#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {
	
		typedef double TVector[TI_N][PHASE_N];

		vector<TEntry> entries;
		vector<array<double, 2>> params;

		constexpr int N_CORES = 8;
		constexpr int N_POSITIONS = 725000;
		constexpr int MAX_EPOCHS = 100000;

		inline double sigmoid(double K, double E) {
			return 1.0 / (1.0 + exp(-K * E / 400.0));
		}
		
		void init_params()
		{
			params.resize(TI_N);
			
			int i = 0;
			
			using namespace Eval;
			
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
				params[i++] = { (double) pawn_table[sq].mg, (double) pawn_table[sq].eg };
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
				params[i++] = { (double) knight_table[sq].mg, (double) knight_table[sq].eg };
			for (Square sq = SQ_ZERO; sq < 16; ++sq)
				params[i++] = { (double) bishop_table[sq].mg, (double) bishop_table[sq].eg };
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
				params[i++] = { (double) rook_table[sq].mg, (double) rook_table[sq].eg };
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
				params[i++] = { (double) queen_table[sq].mg, (double) queen_table[sq].eg };
			for (Square sq = SQ_ZERO; sq < 16; ++sq)
				params[i++] = { (double) king_table[sq].mg, (double) king_table[sq].eg };
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
				params[i++] = { (double) passed_pawn[sq].mg, (double) passed_pawn[sq].eg };
			for (PieceType pt = KNIGHT; pt < KING; ++pt)
				params[i++] = { (double) mobility[pt].mg, (double) mobility[pt].eg };
			
			params[i++] = { -double_pawn_penalty.mg, -double_pawn_penalty.eg };
			params[i++] = { -isolated_pawn_penalty.mg, -isolated_pawn_penalty.eg };
			params[i++] = { bishop_pair_bonus.mg, bishop_pair_bonus.eg };
			params[i++] = { rook_open_file_bonus.mg, rook_open_file_bonus.eg };
			params[i++] = { rook_semi_open_file_bonus.mg, rook_semi_open_file_bonus.eg };
			params[i++] = { -rook_closed_file_penalty.mg, -rook_closed_file_penalty.eg };
			params[i++] = { tempo_bonus.mg, tempo_bonus.eg };
			params[i++] = { -king_full_open_penalty.mg, -king_full_open_penalty.eg };
			params[i++] = { -king_semi_open_penalty.mg, -king_semi_open_penalty.eg };
			params[i++] = { -king_adjacent_full_open_penalty.mg, -king_adjacent_full_open_penalty.eg };
			params[i++] = { -king_adjacent_semi_open_penalty.mg, -king_adjacent_semi_open_penalty.eg };
			params[i++] = { knight_outpost_bonus.mg, knight_outpost_bonus.eg };
			params[i++] = { bishop_outpost_bonus.mg, bishop_outpost_bonus.eg };
			params[i++] = { -weak_queen_penalty.mg, -weak_queen_penalty.eg };
			params[i++] = { rook_on_our_passer_file.mg, rook_on_our_passer_file.eg };
			params[i++] = { rook_on_their_passer_file.mg, rook_on_their_passer_file.eg };
			params[i++] = { -tall_pawn_penalty.mg, -tall_pawn_penalty.eg };
			params[i++] = { fianchetto_bonus.mg, fianchetto_bonus.eg };
			
			assert(i == TI_SAFETY);
			
			for (Square sq = Square(16); sq < 32; ++sq)
				params[i++] = { (double) pawn_shield[sq], 0 };
			for (PieceType pt = PAWN; pt < KING; ++pt)
				params[i++] = { (double) inner_ring_attack[pt], 0 };
			for (PieceType pt = PAWN; pt < KING; ++pt)
				params[i++] = { (double) inner_ring_attack[pt], 0 };
			
			params[i++] = { virtual_king_m, 0 };
			params[i++] = { virtual_king_b, 0 };
			params[i++] = { safety_threshold, 0 };
			
			assert(i == TI_N);
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
		
		long double mse(long double K)
		{
			long double total = 0;
			int start = 0;
			int end = entries.size();

			for (int i = start; i < end; ++i)
				total += pow(entries[i].result - sigmoid(K, linear_eval(&entries[i], NULL)), 2);

			return total / (double) entries.size();
		}
		
		double static_mse(double K) 
		{
			double total = 0.0;
		
			for (int i = 0; i < N_POSITIONS; i++)
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
				}
			}

			ifs.close();
			
			long double K = find_k();
			double rate = 0.10;
			
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
				}

				long double error = mse(K);
				
				if (epoch && epoch % 250 == 0) 
					rate = rate / 1.00;

				cout << "Epoch [" << epoch << "] Error = [" << error << "], Rate = [" << rate << "]" << endl;
			}
		}

		long double find_k()
		{
			int k_precision = 10;
			long double start = -10;
			long double end = 10;
			long double step = 1;
			long double curr = start;
			long double error;
			long double best = static_mse(start);
			
			for (int epoch = 0; epoch < k_precision; ++epoch)
			{
				curr = start - step;

				while (curr < end)
				{
					curr += step;
					error = static_mse(curr);
					
					if (error < best)
					{
						best = error;
						start = curr;
					}
				}

				cout.precision(17);
				cout << "Epoch [" << epoch << "] Error = [" << best << "], K = [" << start << "]" << endl;

				end = start + step;
				start -= step;
				step /= 10.0;
			}
			
			return start;
		}

	} // namespace Tuner

} // namespace Clovis
