#include "search.h"

namespace Clovis {

    namespace Search {

        Move killers[2 * MAX_PLY];

        int history[15 * SQ_N];

        int nodes;

        int pv_length[MAX_PLY];

        Move pv_table[MAX_PLY][MAX_PLY];

        int lmr_table[MAX_PLY + 1][64];

        int null_pruning_depth = 2;

        int asp_window = 25;

        int asp_threshold_depth = 5;

        TTable tt;

        // initialize LMR lookup table values
        void init_search()
        {
            tt.set_size(220000000);
            clear();
            init_lmr_tables();
        }

        void init_lmr_tables()
        {
            for (int depth = 1; depth <= MAX_PLY; ++depth)
                for (int ordered = 1; ordered < 64; ++ordered)
                    lmr_table[depth][ordered] = int(0.75 + log(depth) * log(ordered) / 2.25);
        }

        // reset transposition table, set search to standard conditions
        void clear()
        {
            memset(killers, 0, sizeof(killers));
            memset(history, 0, sizeof(history));
            memset(pv_table, 0, sizeof(pv_table));
            memset(pv_length, 0, sizeof(pv_length));
            tt.clear();
        }

        // begin search
        Move start_search(Position& pos, SearchLimits& lim)
        {

            TimePoint allocated_time = lim.time[pos.side_to_move()] / 20;
            TimeManager tm;
            tm.set();

            MoveGen::MoveList ml = MoveGen::MoveList(pos);
            if (ml.size() == 1)
            {
                pv_table[0][0] = ml.begin()->m;
                goto end;
            }

            memset(killers, 0, sizeof(killers));
            memset(history, 0, sizeof(history));
            memset(pv_table, 0, sizeof(pv_table));
            memset(pv_length, 0, sizeof(pv_length));

            nodes = 0;

            int alpha = NEG_INF;
            int beta = POS_INF;
            int score;

            for (int depth = 1; depth <= MAX_PLY; ++depth) 
            {
                score = negamax(pos, alpha, beta, depth, 0, false);
                std::cout << "info depth " << depth
                    << " score cp " << score
                    << " nodes " << nodes
                    << " pv ";
                for (int i = 0; i < pv_length[0]; ++i) 
                    std::cout << UCI::move2str(pv_table[0][i]) << " ";
                std::cout << std::endl;

                if (tm.get_time_elapsed() > allocated_time)
                    break;

                if (score <= alpha || score >= beta) 
                {
                    alpha = NEG_INF;
                    beta = POS_INF;
                    --depth;
                }
                else 
                {
                    if (score >= CHECKMATE_SCORE - MAX_PLY || 
                        score <= -CHECKMATE_SCORE + MAX_PLY || 
                        (score == DRAW_SCORE && pv_length[0] < depth)) 
                    {
                        break;
                    }
                    alpha = depth > asp_threshold_depth ? score - asp_window : NEG_INF;
                    beta = depth > asp_threshold_depth ? score + asp_window : POS_INF;
                }
            }

        end:

            std::cout << "time " << tm.get_time_elapsed() << std::endl
                << "bestmove " << UCI::move2str(pv_table[0][0]) 
                << std::endl;

            return pv_table[0][0];
        }

        int negamax(Position& pos, int alpha, int beta, int depth, int ply, bool is_null)
        {
            bool pv_node = beta - alpha != 1;
            bool root_node = ply == 0;

            pv_length[ply] = ply;

            if (depth <= 0)
                return quiescent(pos, alpha, beta, ply);

            // avoid overflow
            if (ply >= MAX_PLY)
                return Eval::evaluate(pos);

            ++nodes;

            // Mate distance pruning
            int mating_score = CHECKMATE_SCORE - ply;
            if (mating_score < beta) {
                beta = mating_score;
                if (alpha >= mating_score)
                    return alpha;
            }

            int mated_score = -CHECKMATE_SCORE + ply;
            if (mated_score > alpha) {
                alpha = mated_score;
                if (beta <= mated_score)
                    return beta;
            }

            bool tt_hit;
            TTEntry* tte = tt.probe(pos.get_key(), tt_hit);
            if (tt_hit) 
            {
                pv_table[ply][ply] = tte->move;
                pv_length[ply] = 1;
                if (pv_node == false &&
                    tte->depth >= depth &&
                    (tte->flags == HASH_EXACT ||
                    (tte->flags == HASH_BETA && tte->eval >= beta) ||
                    (tte->flags == HASH_ALPHA && tte->eval <= alpha))) 
                {
                    return tte->eval;
                }
            }

            bool king_in_check = pos.is_king_in_check(pos.side_to_move());

            int score;

            if (king_in_check)
                goto loop;

            // null move pruning
            if (pv_node == false &&
                is_null == false &&
                depth >= 3 &&
                pos.has_promoted(pos.side_to_move())) 
            {
                pos.do_move(MOVE_NULL);
                score = -negamax(pos, -beta, -beta + 1, depth - 3, ply + 1, true);
                pos.undo_move(MOVE_NULL);
                if (score >= beta)
                    return beta;
            }

            // internal iterative deepening
            if (tt_hit == false &&
                ((pv_node && depth >= 6) || (pv_node == false && depth >= 8))) {
                int iid_depth = pv_node ? depth - depth / 4 - 1 : (depth - 5) / 2;
                negamax(pos, alpha, beta, iid_depth, ply, false);
                tte = tt.probe(pos.get_key(), tt_hit);
                if (tt_hit) {
                    pv_table[ply][ply] = tte->move;
                    pv_length[ply] = 1;
                }
            }

        loop:

            Move tt_move = (tt_hit) ? tte->move : MOVE_NONE; 

            MovePick::MovePicker mp = MovePick::MovePicker(pos, killers, history, ply, tt_move);

            ScoredMove curr_move;

            Move best_move = MOVE_NONE;

            if (king_in_check) 
                ++depth;

            int moves_searched = 0;

            HashFlag eval_type = HASH_ALPHA;

            while ((curr_move = mp.get_next(false)) != MOVE_NONE)
            {
                // illegal move
                if (pos.do_move(curr_move.m) == false) continue;
                if (pos.is_repeat() || pos.is_draw_50())
                    score = DRAW_SCORE;
                else if (moves_searched == 0)
                {
                    score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1, false);
                }
                // late move reductions
                else
                {
                    if (depth > 2 &&
                        move_capture(curr_move.m) == 0 &&
                        move_promotion_type(curr_move.m) == 0)
                    {
                        // reduction factor
                        int R = lmr_table[depth][moves_searched % 64];
                        // increase for non pv nodes
                        R += (pv_node == false);
                        // increase for check evasions with king
                        R += (king_in_check && piece_type(move_piece_type(curr_move.m)) == KING);

                        R = std::min(depth - 1, std::max(R, 1));

                        // search current move with reduced depth:
                        score = -negamax(pos, -alpha - 1, -alpha, depth - R, ply + 1, false);

                    }

                    // hack to ensure full search
                    else score = alpha + 1;

                    if (score > alpha)
                    {
                        // re-search at full depth but with narrowed alpha beta window
                        score = -negamax(pos, -alpha - 1, -alpha, depth - 1, ply + 1, false);

                        // if previous search doesnt fail, re-search at full depth and full alpha beta window
                        if ((score > alpha) && (score < beta))
                            score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1, false);
                    }
                }

                pos.undo_move(curr_move.m);

                if (score > alpha)
                {
                    // fail high
                    if (score >= beta)
                    {
                        // update killers
                        if (move_capture(curr_move.m) == false && killers[ply] != curr_move.m)
                        {
                            killers[MAX_PLY + ply] = killers[ply];
                            killers[ply] = curr_move.m;
                        }
                        if (tte->depth <= depth)
                            *tte = TTEntry(pos.get_key(), depth, score, HASH_BETA, curr_move.m);
                        return beta;
                    }

                    // not sure if this should go before or after fail high
                    if (move_capture(curr_move.m) == false)
                    {
                        history[move_piece_type(curr_move.m) * SQ_N + move_to_sq(curr_move.m)] = std::min(history[move_piece_type(curr_move.m) * SQ_N + move_to_sq(curr_move.m)] + depth, 5000);
                    }

                    pv_table[ply][ply] = curr_move.m;

                    // record pv line
                    for (int i = ply + 1; i < pv_length[ply + 1]; ++i) {
                        pv_table[ply][i] = pv_table[ply + 1][i];
                    }

                    pv_length[ply] = pv_length[ply + 1];

                    eval_type = HASH_EXACT;

                    // new best move found
                    alpha = score;

                    best_move = curr_move.m;
                }

                ++moves_searched;
            }

            // no legal moves
            if (moves_searched == 0)
                return king_in_check ? - CHECKMATE_SCORE + ply : - DRAW_SCORE;

            if (tte->depth <= depth)
                *tte = TTEntry(pos.get_key(), depth, alpha, eval_type, best_move);

            return alpha;
        }

        int quiescent(Position& pos, int alpha, int beta, int ply)
        {
            ++nodes;

            bool tt_hit;
            TTEntry* tte = tt.probe(pos.get_key(), tt_hit);
            if (tt_hit) 
            {
                if (tte->flags == HASH_EXACT ||
                   (tte->flags == HASH_BETA && tte->eval >= beta) ||
                   (tte->flags == HASH_ALPHA && tte->eval <= alpha)) 
                {
                    return tte->eval;
                }
            }

            int score = Eval::evaluate(pos);

            if (tt.get_pawn_key(pos.get_pawn_key()) != pos.get_key())
                tt.new_pawn_entry(pos.get_pawn_key(), Eval::evaluate_pawns(pos));

            score += (pos.side_to_move() == BLACK) ? -tt.get_pawn_eval(pos.get_pawn_key()) : tt.get_pawn_eval(pos.get_pawn_key());

            if (score >= beta)
                return beta;

            int old_alpha = alpha;

            if (score > alpha)
                alpha = score;

            Move tt_move = (tt_hit) ? tte->move : MOVE_NONE;

            MovePick::MovePicker mp = MovePick::MovePicker(pos, killers, history, ply, tt_move);
            
            ScoredMove curr_move;

            bool once = false;

            ScoredMove best_move;
            best_move.score = NEG_INF;

            while ((curr_move = mp.get_next(true)) != MOVE_NONE)
            {
                // illegal move or non capture
                if (pos.do_move(curr_move.m, true) == false)
                    continue;

                score = -quiescent(pos, -beta, -alpha, ply + 1);

                pos.undo_move(curr_move.m);

                // fail high
                if (score >= beta)
                {
                    if (tte->depth == 0)
                        *tte = TTEntry(pos.get_key(), 0, score, HASH_BETA, curr_move.m);
                    return beta;
                }
                if (score > best_move.score)
                {
                    best_move.score = score;
                    best_move.m = curr_move.m;
                    if (score > alpha)
                    {
                        // new best move found
                        alpha = score;
                    }
                }
            }

            if (best_move.score != NEG_INF && tte->depth == 0)
                *tte = TTEntry(pos.get_key(), 0, best_move.score, best_move.score > old_alpha ? HASH_EXACT : HASH_ALPHA, best_move.m);

            return alpha;
        }

    } // namespace Search

} // namespace Clovis