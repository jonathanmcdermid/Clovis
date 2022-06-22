#include "search.h"

namespace Clovis {

    namespace Search {

        Move killers[2 * MAX_PLY];

        U64 nodes;

        double tt_hits;

        double total_moves;

        int lmr_table[MAX_PLY + 1][64];

        int null_pruning_depth = 2;

        int asp_window = 50;

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
            MovePick::clear();
            tt.clear();
        }

        // begin search
        Move start_search(Position& pos, SearchLimits& lim)
        {

            TimePoint allocated_time = lim.time[pos.side_to_move()] / 20;
            TimeManager tm;
            tm.set();
            Line pline;

            MoveGen::MoveList ml = MoveGen::MoveList(pos);
            if (ml.size() == 1)
            {
                *pline.last++ = ml.begin()->m;
                goto end;
            }

            memset(killers, 0, sizeof(killers));
            MovePick::clear();

            nodes = 0;
            tt_hits = 0;
            total_moves = 0;

            int alpha = NEG_INF;
            int beta = POS_INF;
            int score;

            for (int depth = 1; depth <= MAX_PLY; ++depth) 
            {
                score = negamax(pos, pline, alpha, beta, depth, 0, false);
                std::cout << "info depth " << depth
                    << " score cp " << score
                    << " nodes " << nodes
                    << " hit% " << (tt_hits / nodes * 100.0) 
                    << " mpn " << (total_moves / nodes)
                    << " pv ";
                for(const auto& m : pline)
                    std::cout << UCI::move2str(m) << " ";
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
                    if (score >= CHECKMATE_SCORE - MAX_PLY 
                        || score <= -CHECKMATE_SCORE + MAX_PLY
                        || score == DRAW_SCORE && pline.move_count() < depth) 
                    {
                        break;
                    }
                    alpha = depth > asp_threshold_depth ? score - asp_window : NEG_INF;
                    beta = depth > asp_threshold_depth ? score + asp_window : POS_INF;
                }
            }

        end:

            std::cout << "time " << tm.get_time_elapsed() << std::endl
                << "bestmove " << UCI::move2str(*pline.begin()) 
                << std::endl;

            return *pline.begin();
        }

        int negamax(Position& pos, Line& pline, int alpha, int beta, int depth, int ply, bool is_null)
        {
            bool pv_node = beta - alpha != 1;
            bool root_node = ply == 0;

            if (depth <= 0)
                return quiescent(pos, alpha, beta, ply);

            // avoid overflow
            if (ply >= MAX_PLY)
                return Eval::evaluate(pos);

            ++nodes;

            // mate distance pruning
            // if me have found a mate, no point in finding a longer mate
            alpha = std::max(alpha, - CHECKMATE_SCORE + ply - 1);
            beta = std::min(beta, CHECKMATE_SCORE - ply);
            if (alpha >= beta)
                return alpha;

            bool tt_hit;
            TTEntry* tte = tt.probe(pos.get_key(), tt_hit);
            if (tt_hit) 
            {
                ++tt_hits;
                pline.last = pline.moves;
                *pline.last++ = tte->move;
                if (pv_node == false 
                    && tte->depth >= depth 
                    && (tte->flags == HASH_EXACT 
                        || (tte->flags == HASH_BETA && tte->eval >= beta) 
                        || (tte->flags == HASH_ALPHA && tte->eval <= alpha))) 
                {
                    return tte->eval;
                }
            }

            bool king_in_check = pos.is_king_in_check(pos.side_to_move());

            Line local_line;

            int score;

            if (king_in_check)
                goto loop;

            // null move pruning
            if (root_node == false
                && pv_node == false
                && is_null == false
                && depth >= 3
                && pos.has_promoted(pos.side_to_move())) 
            {
                pos.do_move(MOVE_NULL);
                score = -negamax(pos, local_line, -beta, -beta + 1, depth - 3, ply + 1, true);
                pos.undo_move(MOVE_NULL);
                if (score >= beta)
                    return beta;
            }

            // internal iterative deepening
            if (tt_hit == false
                && ((pv_node && depth >= 6)
                    || (pv_node == false && depth >= 8))) 
            {
                int iid_depth = pv_node ? depth - depth / 4 - 1 : (depth - 5) / 2;
                negamax(pos, local_line, alpha, beta, iid_depth, ply, false);
                tte = tt.probe(pos.get_key(), tt_hit);
                if (tt_hit) 
                {
                    pline.last = pline.moves;
                    *pline.last++ = tte->move;
                }
            }

        loop:

            Move tt_move = (tt_hit) ? tte->move : MOVE_NONE; 

            MovePick::MovePicker mp = MovePick::MovePicker(pos, killers, ply, tt_move);

            ScoredMove curr_move;

            Move best_move = MOVE_NONE;

            if (king_in_check) 
                ++depth;

            int moves_searched = 0;

            HashFlag eval_type = HASH_ALPHA;

            while ((curr_move = mp.get_next(false)) != MOVE_NONE)
            {
                // illegal move
                if (pos.do_move(curr_move.m) == false)
                    continue;
                if (pos.is_repeat() || pos.is_draw_50()) //|| pos.is_material_draw())
                    score = DRAW_SCORE;
                else if (moves_searched == 0)
                    score = -negamax(pos, local_line, -beta, -alpha, depth - 1, ply + 1, false);
                // late move reductions
                else
                {
                    if (depth > 2
                        && move_capture(curr_move.m) == NO_PIECE
                        && move_promotion_type(curr_move.m) == NO_PIECE)
                    {
                        int history_entry = MovePick::get_history_entry(other_side(pos.side_to_move()), curr_move.m);
                        // reduction factor
                        int R = lmr_table[depth][std::min(moves_searched, MAX_PLY - 1)];
                        // reduce for pv nodes
                        R -= (pv_node);
                        // reduce for killers
                        R -= (curr_move == killers[MAX_PLY + ply] || curr_move == killers[ply]);
                        // reduce based on history heuristic
                        R -= std::max(-2, std::min(2, history_entry / 4000));

                        R = std::max(0, std::min(R, depth - 2));

                        // search current move with reduced depth:
                        score = -negamax(pos, local_line, -alpha - 1, -alpha, depth - R - 1, ply + 1, false);

                    }

                    // hack to ensure full search
                    else score = alpha + 1;

                    if (score > alpha)
                    {
                        // re-search at full depth but with narrowed alpha beta window
                        score = -negamax(pos, local_line, -alpha - 1, -alpha, depth - 1, ply + 1, false);

                        // if previous search doesnt fail, re-search at full depth and full alpha beta window
                        if ((score > alpha) && (score < beta))
                            score = -negamax(pos, local_line, -beta, -alpha, depth - 1, ply + 1, false);
                    }
                }

                pos.undo_move(curr_move.m);

                ++total_moves;

                // fail high
                if (score >= beta)
                {
                    if (move_capture(curr_move.m) == NO_PIECE)
                    {
                        mp.update_history(curr_move.m, depth);
                        // update killers
                        if (killers[ply] != curr_move.m)
                        {
                            killers[MAX_PLY + ply] = killers[ply];
                            killers[ply] = curr_move.m;
                        }
                    }

                    if (tte->depth <= depth)
                        *tte = TTEntry(pos.get_key(), depth, score, HASH_BETA, curr_move.m);

                    return beta;

                }

                if (score > alpha)
                {
                    pline.last = pline.moves;
                    *pline.last++ = curr_move.m;
                    for (const auto& m : local_line)
                        *pline.last++ = m;

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

            if (eval_type == HASH_EXACT)
            {
                if(depth >= tte->depth)
                    *tte = TTEntry(pos.get_key(), depth, alpha, eval_type, best_move);

                if (move_capture(best_move) == NO_PIECE)
                    mp.update_history(best_move, depth);
            }

            return alpha;
        }

        int quiescent(Position& pos, int alpha, int beta, int ply)
        {

            bool tt_hit;
            TTEntry* tte = tt.probe(pos.get_key(), tt_hit);
            if (tt_hit) 
            {
                if (tte->flags == HASH_EXACT
                    || (tte->flags == HASH_BETA && tte->eval >= beta)
                    || (tte->flags == HASH_ALPHA && tte->eval <= alpha)) 
                {
                    return tte->eval;
                }
            }

            int score = Eval::evaluate(pos);

            bool pt_hit;
            PTEntry* pte = tt.probe_pawn(pos.get_pawn_key(), pt_hit);
            if (pt_hit == false)
                *pte = PTEntry(pos.get_pawn_key(), Eval::evaluate_pawns(pos));

            score += pte->eval.get_score(pos.get_game_phase(), pos.side_to_move());
            
            // do pawn key regen test here

            if (score >= beta)
                return beta;

            int old_alpha = alpha;

            if (score > alpha)
                alpha = score;

            Move tt_move = (tt_hit) ? tte->move : MOVE_NONE;
            MovePick::MovePicker mp = MovePick::MovePicker(pos, killers, ply, tt_move);
            ScoredMove curr_move;
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