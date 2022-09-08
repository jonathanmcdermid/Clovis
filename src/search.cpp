#include "search.h"

namespace Clovis {

    namespace Search {

        U64 nodes;

        int lmr_table[MAX_PLY + 1][64];
        int null_pruning_depth = 3;
        int asp_window = 50;
        int asp_threshold_depth = 3;

        bool stop;

        TTable tt;

        TimePoint allocated_time;
        TimeManager tm;

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
            MovePick::clear();
            tt.clear();
        }

        // begin search
        Move start_search(Position& pos, SearchLimits& lim)
        {
            stop = false;
            allocated_time = 5 * lim.time[pos.side_to_move()] / (lim.moves_left + 5);
            tm.set();
            Line pline;

            int alpha = NEG_INF;
            int beta = POS_INF;
            int score;

            MoveGen::MoveList ml = MoveGen::MoveList(pos);
            if (ml.size() == 1)
            {
                *pline.last++ = ml.begin()->m;
                goto end;
            }

            MovePick::clear();

            nodes = 0;

            for (int depth = 1; depth <= MAX_PLY  && (lim.depth == 0 || depth <= lim.depth); ++depth)
            {
                score = negamax(pos, pline, alpha, beta, depth, 0, false);

                if (stop)
                    break;

                std::cout << "info depth " << depth
                    << " score cp " << score
                    << " nodes " << nodes
                    << " time " << tm.get_time_elapsed()
                    << " pv ";
                for(const auto& m : pline)
                    std::cout << UCI::move2str(m) << " ";
                std::cout << std::endl;

                if ((tm.get_time_elapsed() > allocated_time / 3 && depth >= lim.depth)
                    || score >= CHECKMATE_SCORE - MAX_PLY
                    || score <= -CHECKMATE_SCORE + MAX_PLY
                    || score == DRAW_SCORE && pline.move_count() < depth)
                {
                    break;
                }

                if (score <= alpha || score >= beta) 
                {
                    alpha = NEG_INF;
                    beta = POS_INF;
                    --depth;
                    continue;
                }
                alpha = depth > asp_threshold_depth ? score - asp_window : NEG_INF;
                beta = depth > asp_threshold_depth ? score + asp_window : POS_INF;
            }

        end:

            std::cout << "bestmove " << UCI::move2str(*pline.begin()) << std::endl;

            return *pline.begin();
        }

        int negamax(Position& pos, Line& pline, int alpha, int beta, int depth, int ply, bool is_null)
        {
            if (nodes & 2047 && tm.get_time_elapsed() > allocated_time)
            {
                stop = true;
                return 0;
            }

            bool pv_node = beta - alpha != 1;
            bool root_node = ply == 0;

            if (depth <= 0)
                return quiescent(pos, alpha, beta);

            // avoid overflow
            if (ply >= MAX_PLY)
                return (Eval::evaluate(pos) + Eval::evaluate_pawns(pos)).get_score(pos.get_game_phase(), pos.side_to_move());

            ++nodes;

            // mate distance pruning
            // if me have found a mate, no point in finding a longer mate
            alpha = std::max(alpha, - CHECKMATE_SCORE + ply);
            beta = std::min(beta, CHECKMATE_SCORE - ply + 1);
            if (alpha >= beta)
                return alpha;

            TTEntry* tte = tt.probe(pos.get_key());
            if (tte != nullptr) 
            {
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

            int eval = king_in_check ? 0 : tte != nullptr ? tte->eval : (Eval::evaluate(pos) + Eval::evaluate_pawns(pos)).get_score(pos.get_game_phase(), pos.side_to_move());

            if (king_in_check)
                goto loop;

            // reverse futility pruning
            // if evaluation is above a certain threshold, we can trust that it will maintain it in the future
            // should this be allowed at root nodes? 
            if (pv_node == false
                && king_in_check == false
                && depth <= 8
                && eval - 75 * depth > beta)
                return eval;

            // null move pruning
            if (root_node == false
                && pv_node == false
                && is_null == false
                && depth >= null_pruning_depth
                && pos.has_promoted(pos.side_to_move())) 
            {
                pos.do_move(MOVE_NULL);
                score = -negamax(pos, local_line, -beta, -beta + 1, depth - 3, ply + 1, true);
                pos.undo_move(MOVE_NULL);
                if (score >= beta)
                    return beta;
            }

            // internal iterative deepening
            if (tte == nullptr
                && ((pv_node && depth >= 6)
                    || (pv_node == false && depth >= 8))) 
            {
                negamax(pos, local_line, alpha, beta, pv_node ? depth - depth / 4 - 1 : (depth - 5) / 2, ply, false);
                tte = tt.probe(pos.get_key());
                if (tte != nullptr) 
                {
                    pline.last = pline.moves;
                    *pline.last++ = tte->move;
                }
            }

        loop:

            Move tt_move = (tte != nullptr) ? tte->move : MOVE_NONE; 

            MovePick::MovePicker mp = MovePick::MovePicker(pos, ply, tt_move);

            ScoredMove curr_move;

            Move best_move = MOVE_NONE;

            int best_score = NEG_INF;

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
                        R -= MovePick::is_killer(curr_move.m, ply);
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

                if (stop)
                    return 0;

                ++moves_searched;

                // fail high
                if (score >= beta)
                {
                    if (move_capture(curr_move.m) == NO_PIECE)
                    {
                        mp.update_history(curr_move.m, depth);
                        MovePick::update_killers(curr_move.m, ply);
                    }

                    tt.new_entry(pos.get_key(), depth, beta, HASH_BETA, curr_move.m);

                    return beta;

                }

                if (score > best_score)
                {
                    best_move = curr_move.m;
                    best_score = score;
                    if (score > alpha)
                    {
                        pline.last = pline.moves;
                        *pline.last++ = curr_move.m;
                        for (const auto& m : local_line)
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

            if (eval_type == HASH_EXACT)
                tt.new_entry(pos.get_key(), depth, alpha, HASH_EXACT, best_move);
            else
                tt.new_entry(pos.get_key(), depth, alpha, HASH_ALPHA, pv_node ? MOVE_NONE : best_move);

            if(eval_type == HASH_EXACT && move_capture(best_move) == NO_PIECE)
                mp.update_history(best_move, depth);

            return alpha;
        }

        int quiescent(Position& pos, int alpha, int beta)
        {

            TTEntry* tte = tt.probe(pos.get_key());

            if (tte != nullptr)
            {
                if (tte->flags == HASH_EXACT
                    || (tte->flags == HASH_BETA && tte->eval >= beta)
                    || (tte->flags == HASH_ALPHA && tte->eval <= alpha))
                {
                    return tte->eval;
                }
            }
            
            PTEntry* pte = tt.probe_pawn(pos.get_pawn_key());

            if (pte == nullptr)
            {
                tt.new_pawn_entry(pos.get_pawn_key(), Eval::evaluate_pawns(pos));
                pte = tt.probe_pawn(pos.get_pawn_key());
                assert(pte != nullptr);
            }

            int eval = (Eval::evaluate(pos) + pte->eval).get_score(pos.get_game_phase(), pos.side_to_move());

            if (pos.is_insufficient(pos.side_to_move()))
            {
                if (pos.is_insufficient(other_side(pos.side_to_move())))
                    return DRAW_SCORE;
                else
                    eval = std::min(DRAW_SCORE, eval);
            }
            else if (pos.is_insufficient(other_side(pos.side_to_move())))
                eval = std::max(DRAW_SCORE, eval);

            if (eval >= beta)
                return beta;

            int old_alpha = alpha;

            if (eval > alpha)
                alpha = eval;

            MovePick::MovePicker mp = MovePick::MovePicker(pos, 0, (tte != nullptr) ? tte->move : MOVE_NONE);
            ScoredMove curr_move;
            Move best_move = MOVE_NONE;
            int best_eval = NEG_INF;

            while ((curr_move = mp.get_next(true)) != MOVE_NONE) // only do winning caps once see is fixed
            {
                // illegal move or non capture
                if (pos.do_move(curr_move.m) == false)
                    continue;

                eval = -quiescent(pos, -beta, -alpha);

                pos.undo_move(curr_move.m);

                // fail high
                if (eval >= beta)
                {
                    tt.new_entry(pos.get_key(), 0, beta, HASH_BETA, curr_move.m);

                    return beta;
                }
                if (eval > best_eval)
                {
                    best_eval = eval;
                    best_move = curr_move.m;
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