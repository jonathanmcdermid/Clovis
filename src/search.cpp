#include "search.h"

namespace Clovis {

    namespace Search {

        Move killers[2 * MAX_PLY];

        int history[15 * SQ_N];

        int nodes;

        int pv_length[MAX_PLY];

        Move pv_table[MAX_PLY][MAX_PLY];

        int lmr_table[MAX_PLY + 1][64];

        // initialize LMR lookup table values
        void init_search()
        {
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
        }

        // begin search
        Move start_search(Position& pos, SearchLimits& lim)
        {
            TimePoint allocated_time = lim.time[pos.side_to_move()] / 20;
            TimeManager tm;
            tm.set();

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
                score = negamax(pos, alpha, beta, depth, 0);
                std::cout << "info depth " << depth
                    << " score cp " << score
                    << " nodes " << nodes
                    << " pv ";
                for (int i = 0; i < pv_length[0]; ++i) 
                    std::cout << UCI::move2str(pv_table[0][i]) << " ";
                std::cout << std::endl;
                if (tm.get_time_elapsed() > allocated_time)
                    break;
            }

            std::cout << "time " << tm.get_time_elapsed() << std::endl
                << "bestmove " << UCI::move2str(pv_table[0][0]) 
                << std::endl;

            return pv_table[0][0];
        }

        int negamax(Position& pos, int alpha, int beta, int depth, int ply)
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
            int score = NEG_INF;

            Move pv_move = (pv_node) ? pv_table[ply][ply] : MOVE_NONE;

            MovePick::MovePicker mp = MovePick::MovePicker(pos, killers, history, ply, pv_move);
            mp.sm_sort();

            ScoredMove curr_move;

            bool king_in_check = pos.is_king_in_check(pos.side_to_move());

            if (king_in_check) {
                ++depth;
            }

            bool found_pv = false;

            int moves_searched = 0;

            while ((curr_move = mp.get_next()) != MOVE_NONE) 
            {
                // illegal move
                if (pos.do_move(curr_move.m) == false) continue;


                if (found_pv) 
                {
                    score = -negamax(pos, -alpha - 1, -alpha, depth - 1, ply + 1);

                    if (score > alpha && score < beta)
                        score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1);
                }
                else
                {
                    if (moves_searched == 0)
                    {
                        score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1);
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
                            score = -negamax(pos, -alpha - 1, -alpha, depth - R, ply + 1);

                        }

                        // hack to ensure full search
                        else score = alpha + 1;

                        if (score > alpha)
                        {
                            // re-search at full depth but with narrowed alpha beta window
                            score = -negamax(pos, -alpha - 1, -alpha, depth - 1, ply + 1);

                            // if previous search doesnt fail, re-search at full depth and full alpha beta window
                            if ((score > alpha) && (score < beta))
                                score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1);
                        }
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

                    if(found_pv == false) 
                        found_pv = true;

                    // new best move found
                    alpha = score;
                }

                ++moves_searched;
            }

            // no legal moves
            if (moves_searched == 0)
                return king_in_check ? - CHECKMATE_SCORE + ply : - DRAW_SCORE;

            return alpha;
        }

        int quiescent(Position& pos, int alpha, int beta, int ply)
        {
            ++nodes;

            int score = Eval::evaluate(pos);

            if (score >= beta)
                return beta;

            if (score > alpha)
                alpha = score;

            MovePick::MovePicker mp = MovePick::MovePicker(pos, killers, history, ply);
            mp.sm_sort();
            
            ScoredMove curr_move;

            while ((curr_move = mp.get_next()) != MOVE_NONE)
            {
                // illegal move or non capture
                if (pos.do_move(curr_move.m, true) == false)
                    continue;

                score = -quiescent(pos, -beta, -alpha, ply + 1);

                pos.undo_move(curr_move.m);

                // fail high
                if (score >= beta)
                {
                    return beta;
                }
                if (score > alpha)
                {
                    // new best move found
                    alpha = score;
                }
            }

            return alpha;
        }

    } // namespace Search

} // namespace Clovis