#include "search.h"

using namespace std;

namespace Clovis {

    namespace Search {

        int lmr_table[MAX_PLY + 1][64];
		int lmr_depth = 2;
		int lmr_history_min = 2;
		int lmr_history_max = 2;
		int lmr_history_divisor = 4000;
		int lmr_reduction = 2;

		int iid_table[MAX_PLY + 1][2];
		int iid_depth[2] = { 5, 7 };
		int iid_reduction[2] = { 5, 4 };
		int iid_factor[2] = { 1, 4 };
		int iid_divisor[2] = { 2, 4 };

		int futility_reduction[MAX_PLY + 1];
		int futility_factor = 75;
		int futility_depth = 8;

        int null_depth = 3;
		int null_reduction = 3;

        int asp_depth = 3;
		int delta = 45;

        bool stop;

        TimePoint allocated_time;

        // initialize starting search conditions
        void init_search()
        {
            clear();
            init_values();
        }

        // initialize search values
        void init_values()
        {
            for (int depth = 1; depth <= MAX_PLY; ++depth)
			{
				futility_reduction[depth] = depth * futility_factor;

				for (int i = 0; i < 2; ++i)
					iid_table[depth][i] = (iid_factor[i] * depth - iid_reduction[i]) / (iid_divisor[i] + 1);

                for (int ordered = 1; ordered < 64; ++ordered)
                    lmr_table[depth][ordered] = int(0.75 + log(depth) * log(ordered) / 2.25);
			}
        }

        // reset transposition table, set search to standard conditions
        void clear()
        {
            MovePick::clear();
            tt.clear();
        }

        // begin search
        void start_search(Position& pos, SearchLimits& lim, Move& best_move, Move& ponder_move, int& score, U64& nodes)
        {
            stop = false;
            allocated_time = lim.depth ? LLONG_MAX : 5 * lim.time[pos.side] / (lim.moves_left + 5);
            tm.set();

            int alpha = -CHECKMATE_SCORE;
            int beta = CHECKMATE_SCORE;
			Line pline;

			TTEntry* tte;
            MoveGen::MoveList ml = MoveGen::MoveList(pos);

            if (ml.size() == 1)
            {
                best_move = *ml.begin();
                goto end;
            }

            MovePick::reset_killers();
            MovePick::age_history();

            nodes = 0;

            for (int depth = 1; depth <= MAX_PLY  && (lim.depth == 0 || depth <= lim.depth); ++depth)
            {
                score = negamax<NODE_ROOT>(pos, alpha, beta, depth, 0, false, MOVE_NONE, nodes, pline);
				
                if (stop)
                    break;
				
                if (score <= alpha)
				{
					assert(depth > asp_depth);
					alpha = -CHECKMATE_SCORE;
					--depth;
					continue;
				}
				if (score >= beta)
				{
					assert(depth > asp_depth);
					beta = CHECKMATE_SCORE;
					--depth;
					continue;
				}

				auto time = tm.get_time_elapsed();

                cout << "info depth "	<< setw(2) << depth
                    << " score cp "		<< setw(4) << score
                    << " nodes "		<< setw(8) << nodes
                    << " time "			<< setw(6) << tm.get_time_elapsed()
					<< " nps "			<< setw(8) << 1000ULL * nodes / (time + 1) 
                    << " pv "			<< pline.moves[0];

				cout << endl;

                if (time > allocated_time / 3)
                    break;
                
				if (depth > asp_depth)
				{
					alpha = score - delta;
					beta = score + delta;
				}
			}

			best_move = pline.moves[0];

        end:

			// extract ponder move if one exists
			pos.do_move(best_move);
			tte = tt.probe(pos.get_key());
			ponder_move = tte ? tte->move : MOVE_NONE;
			pos.undo_move(best_move);

            cout << "bestmove " << best_move << endl;
        }

        template<NodeType N>
        int negamax(Position& pos, int alpha, int beta, int depth, int ply, bool is_null, Move prev_move, U64& nodes, Line& pline)
        {
            constexpr bool ROOT_NODE = N == NODE_ROOT;
            constexpr bool PV_NODE = N != NODE_NON_PV;

            assert(PV_NODE || (alpha == beta - 1));

            if (nodes & 2047 && tm.get_time_elapsed() > allocated_time)
            {
                stop = true;
                return 0;
            }

            if (depth <= 0)
                return quiescent<PV_NODE ? NODE_PV : NODE_NON_PV>(pos, alpha, beta, nodes);

			++nodes;

            // avoid overflow
            if (ply >= MAX_PLY)
                return Eval::evaluate<true>(pos);

            // mate distance pruning
            // if me have found a mate, no point in finding a longer mate
            alpha = max(alpha, - CHECKMATE_SCORE + ply);
            beta = min(beta, CHECKMATE_SCORE - ply + 1);
            if (alpha >= beta)
                return alpha;

            TTEntry* tte = tt.probe(pos.get_key());
            Move tt_move;
            if (tte)
            {
				pline.last = pline.moves;
				*pline.last++ = tte->move;
                if (!PV_NODE
                    && tte->depth >= depth
                    && (tte->flags == HASH_EXACT
                        || (tte->flags == HASH_BETA && tte->eval >= beta)
                        || (tte->flags == HASH_ALPHA && tte->eval <= alpha)))
                {
                    return tte->eval;
                }
                tt_move = tte->move;
            }
            else
                tt_move = MOVE_NONE;

			Line lline;

            bool king_in_check = pos.is_king_in_check(pos.side);

            int score;

            if (king_in_check)
                goto loop;

            score = tte ? tte->eval : Eval::evaluate<true>(pos);
            
            // reverse futility pruning
            // if evaluation is above a certain threshold, we can trust that it will maintain it in the future
            if (!PV_NODE
                && depth <= futility_depth
                && score - futility_reduction[depth] > beta)
                return score;
            
            // null move pruning
            if (!PV_NODE
                && !is_null
                && depth >= null_depth
                && pos.stm_has_promoted()) 
            {
                pos.do_move(MOVE_NULL);
                score = -negamax<NODE_NON_PV>(pos, -beta, -beta + 1, depth - null_reduction, ply + 1, true, MOVE_NULL, nodes, lline);
                pos.undo_move(MOVE_NULL);
                if (score >= beta)
                    return beta;
            }
            
            // internal iterative deepening
            if (!tte && depth >= iid_depth[PV_NODE])
            {
                negamax<N>(pos, alpha, beta, iid_table[depth][PV_NODE], ply, false, prev_move, nodes, lline);
                tte = tt.probe(pos.get_key());
                if (tte)
                {
					pline.last = pline.moves;
					*pline.last++ = tte->move;
                    tt_move = tte->move;
                }
            }

        loop:

            MovePick::MovePicker mp = MovePick::MovePicker(pos, ply, prev_move, tt_move);

            Move curr_move;
            Move best_move = MOVE_NONE;

            int best_score = INT_MIN;

            if (king_in_check) 
                ++depth;

            int moves_searched = 0;

            HashFlag eval_type = HASH_ALPHA;

            while ((curr_move = mp.get_next<true>()) != MOVE_NONE)
            {
                // illegal move
                if (!pos.do_move(curr_move))
                    continue;

                ++moves_searched;

                if (pos.is_repeat() || pos.is_draw_50() || pos.is_material_draw())
                    score = DRAW_SCORE;
                else if (moves_searched > 1
                    && depth > lmr_depth
                    && move_capture(curr_move) == NO_PIECE
                    && move_promotion_type(curr_move) == NO_PIECE)
                {
                    int history_entry = MovePick::get_history_entry(other_side(pos.side), curr_move);
                    // reduction factor
                    int R = lmr_table[depth][min(moves_searched, 63)];
                    // reduce for pv nodes
                    R -= (PV_NODE);
                    // reduce for killers
                    R -= MovePick::is_killer(curr_move, ply);
                    // reduce based on history heuristic
                    R -= max(-lmr_history_min, min(lmr_history_max, history_entry / lmr_history_divisor));

                    R = max(0, min(R, depth - lmr_reduction));

                    // search current move with reduced depth:
                    score = -negamax<NODE_NON_PV>(pos, -alpha - 1, -alpha, depth - R - 1, ply + 1, false, curr_move, nodes, lline);

                    if (R && score > alpha)
                        score = -negamax<NODE_NON_PV>(pos, -alpha - 1, -alpha, depth - 1, ply + 1, false, curr_move, nodes, lline);
                }
                else if (!PV_NODE || moves_searched > 1)
                    score = -negamax<NODE_NON_PV>(pos, -alpha - 1, -alpha, depth - 1, ply + 1, false, curr_move, nodes, lline);
                if (PV_NODE && (moves_searched == 1 || (score > alpha && (ROOT_NODE || score < beta))))
                    score = -negamax<NODE_PV>(pos, -beta, -alpha, depth - 1, ply + 1, false, curr_move, nodes, lline);

                pos.undo_move(curr_move);

                if (stop)
                    return 0;

                // fail high
                if (score >= beta)
                {
                    if (move_capture(curr_move) == NO_PIECE)
                    {
                        mp.update_history<HASH_BETA>(curr_move, depth);
                        MovePick::update_killers(curr_move, ply);
                        MovePick::update_counter_entry(pos.side, prev_move, curr_move);
                    }
                    
                    tt.new_entry(pos.get_key(), depth, beta, HASH_BETA, curr_move);

                    return beta;
                }

                if (score > best_score)
                {
                    best_move = curr_move;
                    best_score = score;
                    if (score > alpha)
                    {
                        pline.last = pline.moves;
                        *pline.last++ = curr_move;
                        for (const auto& m : lline)
                            *pline.last++ = m;

						eval_type = HASH_EXACT;

                        // new best move found
                        alpha = score;
                    }
                }
            }

            // no legal moves
            if (moves_searched == 0)
                return king_in_check ? - CHECKMATE_SCORE + ply : - DRAW_SCORE;

            tt.new_entry(pos.get_key(), depth, best_score, eval_type, best_move);

            if(eval_type == HASH_EXACT && move_capture(best_move) == NO_PIECE)
                mp.update_history<HASH_EXACT>(best_move, depth);

            return alpha;
        }

        template<NodeType N>
        int quiescent(Position& pos, int alpha, int beta, U64& nodes)
        {
            constexpr bool PV_NODE = N != NODE_NON_PV;

            assert(PV_NODE || (alpha == beta - 1));

			++nodes;

            TTEntry* tte = tt.probe(pos.get_key());

            if (!PV_NODE && tte &&
                (tte->flags == HASH_EXACT
                    || (tte->flags == HASH_BETA && tte->eval >= beta)
                    || (tte->flags == HASH_ALPHA && tte->eval <= alpha)))
                return tte->eval;

            int eval = Eval::evaluate<true>(pos);

            // use TT score instead of static eval if conditions are right
            // conditions: valid TTE and either 
            // 1. alpha flag + lower hash score than static eval
            // 2. beta flag + higher hash score than static eval
            if(tte && ((tte->flags == HASH_ALPHA) == (tte->eval < eval)))
                eval = tte->eval;

            if (eval >= beta)
                return beta;

            int old_alpha = alpha;

            if (eval > alpha)
                alpha = eval;

            MovePick::MovePicker mp = MovePick::MovePicker(pos, 0, MOVE_NONE, (tte) ? tte->move : MOVE_NONE);
            Move curr_move;
            Move best_move = MOVE_NONE;
            int best_eval = INT_MIN;

            while ((curr_move = mp.get_next<false>()) != MOVE_NONE && mp.get_stage() != LOSING_CAPTURES)
            {
                // illegal move or non capture
                if (!pos.do_move(curr_move))
                    continue;

                eval = -quiescent<N>(pos, -beta, -alpha, nodes);

                pos.undo_move(curr_move);

                // fail high
                if (eval >= beta)
                {
                    tt.new_entry(pos.get_key(), 0, beta, HASH_BETA, curr_move);

                    return beta;
                }
                if (eval > best_eval)
                {
                    best_eval = eval;
                    best_move = curr_move;
                    if (eval > alpha)
                    {
                        // new best move found
                        alpha = eval;
                    }
                }
            }

            tt.new_entry(pos.get_key(), 0, alpha, alpha > old_alpha ? HASH_EXACT : HASH_ALPHA, best_move);

            return alpha;
        }

    } // namespace Search

} // namespace Clovis
