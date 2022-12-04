#include "search.h"

using namespace std;

namespace Clovis {

    namespace Search {

        int lmr_table[MAX_PLY + 1][64];
        int null_pruning_depth = 3;
        int asp_window = 50;
        int asp_threshold_depth = 3;

        bool stop;

        TimePoint allocated_time;

        // initialize starting search conditions
        void init_search()
        {
            clear();
            init_lmr_tables();
        }

        // initialize LMR lookup table values
        void init_lmr_tables()
        {
            for (int depth = 1; depth <= MAX_PLY; ++depth)
                for (int ordered = 1; ordered < 64; ++ordered)
                    lmr_table[depth][ordered] = int(0.75 + log(depth) * log(ordered) / 2.25);
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
            allocated_time = lim.depth ? LLONG_MAX : 5 * lim.time[pos.stm()] / (lim.moves_left + 5);
            tm.set();

            int alpha = INT_MIN;
            int beta = INT_MAX;

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
                score = negamax(pos, alpha, beta, depth, 0, false, MOVE_NONE, nodes);

				best_move = tt.probe(pos.get_key())->move;
				
                if (stop)
                    break;

                if (score <= alpha || score >= beta)
                {
                    alpha = INT_MIN;
                    beta = INT_MAX;
                    --depth;
                    continue;
                }

				auto time = tm.get_time_elapsed();

                cout << "info depth "	<< setw(2) << depth
                    << " score cp "		<< setw(4) << score
                    << " nodes "		<< setw(8) << nodes
                    << " time "			<< setw(6) << tm.get_time_elapsed()
					<< " nps "			<< setw(6) << 1000ULL * nodes / (time + 1) 
                    << " pv "			<< move2str(best_move);

				cout << endl;

                if (tm.get_time_elapsed() > allocated_time / 3)
                    break;
                
                alpha = depth > asp_threshold_depth ? score - asp_window : INT_MIN;
                beta = depth > asp_threshold_depth ? score + asp_window : INT_MAX;
            }

        end:

			// engine does not support ponder yet
			ponder_move = MOVE_NONE;

            cout << "bestmove " << move2str(best_move) << endl;
        }

        int negamax(Position& pos, int alpha, int beta, int depth, int ply, bool is_null, Move prev_move, U64& nodes)
        {
            if (nodes & 2047 && tm.get_time_elapsed() > allocated_time)
            {
                stop = true;
                return 0;
            }

            bool pv_node = beta - alpha != 1;

            if (depth <= 0)
                return quiescent(pos, alpha, beta, nodes);

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
                if (!pv_node
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

            bool king_in_check = pos.is_king_in_check(pos.stm());

            int score;

            if (king_in_check)
                goto loop;

            score = tte ? tte->eval : Eval::evaluate<true>(pos);
            
            // reverse futility pruning
            // if evaluation is above a certain threshold, we can trust that it will maintain it in the future
            if (!pv_node
                && depth <= 8
                && score - 75 * depth > beta)
                return score;
            
            // null move pruning
            if (!pv_node
                && !is_null
                && depth >= null_pruning_depth
                && pos.stm_has_promoted()) 
            {
                pos.do_move(MOVE_NULL);
                score = -negamax(pos, -beta, -beta + 1, depth - 3, ply + 1, true, MOVE_NULL, nodes);
                pos.undo_move(MOVE_NULL);
                if (score >= beta)
                    return beta;
            }
            
            // internal iterative deepening
            if (!tte
                && ((pv_node && depth >= 6)
                    || (!pv_node && depth >= 8))) 
            {
                negamax(pos, alpha, beta, pv_node ? depth - depth / 4 - 1 : (depth - 5) / 2, ply, false, prev_move, nodes);
                tte = tt.probe(pos.get_key());
                if (tte)
                {
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

            while ((curr_move = mp.get_next(false)) != MOVE_NONE)
            {
                // illegal move
                if (!pos.do_move(curr_move))
                    continue;

                ++moves_searched;

                if (pos.is_repeat() || pos.is_draw_50() || pos.is_material_draw())
                    score = DRAW_SCORE;
                else if (moves_searched == 1)
                    score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1, false, curr_move, nodes);
                // late move reductions
                else
                {
                    if (depth > 2
                        && move_capture(curr_move) == NO_PIECE
                        && move_promotion_type(curr_move) == NO_PIECE)
                    {
                        int history_entry = MovePick::get_history_entry(other_side(pos.stm()), curr_move);
                        // reduction factor
                        int R = lmr_table[depth][min(moves_searched, MAX_PLY - 1)];
                        // reduce for pv nodes
                        R -= (pv_node);
                        // reduce for killers
                        R -= MovePick::is_killer(curr_move, ply);
                        // reduce based on history heuristic
                        R -= max(-2, min(2, history_entry / 4000));
                
                        R = max(0, min(R, depth - 2));
                
                        // search current move with reduced depth:
                        score = -negamax(pos, -alpha - 1, -alpha, depth - R - 1, ply + 1, false, curr_move, nodes);
                
                    }
                
                    // hack to ensure full search
                    else score = alpha + 1;
                
                    if (score > alpha)
                    {
                        // re-search at full depth but with narrowed alpha beta window
                        score = -negamax(pos, -alpha - 1, -alpha, depth - 1, ply + 1, false, curr_move, nodes);
                
                        // if previous search doesnt fail, re-search at full depth and full alpha beta window
                        if ((score > alpha) && (score < beta))
                            score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1, false, curr_move, nodes);
                    }
                }

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
                        MovePick::update_counter_entry(pos.stm(), prev_move, curr_move);
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

        int quiescent(Position& pos, int alpha, int beta, U64& nodes)
        {
			++nodes;

            TTEntry* tte = tt.probe(pos.get_key());

            if (tte &&
                (tte->flags == HASH_EXACT
                    || (tte->flags == HASH_BETA && tte->eval >= beta)
                    || (tte->flags == HASH_ALPHA && tte->eval <= alpha)))
                return tte->eval;

            int eval = Eval::evaluate<true>(pos);

            // use TT score instead of static eval if conditions are right
            // conditions: valid TTE and either 
            // 1. alpha flag + lower hash score than static eval
            // 2. beta flag + higher hash score than static eval
            if(tte && ((tte->flags == HASH_ALPHA) == tte->eval < eval))
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

            while ((curr_move = mp.get_next(true)) != MOVE_NONE && mp.get_stage() != LOSING_CAPTURES)
            {
                // illegal move or non capture
                if (!pos.do_move(curr_move))
                    continue;

                eval = -quiescent(pos, -beta, -alpha, nodes);

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
