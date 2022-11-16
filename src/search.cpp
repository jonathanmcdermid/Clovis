#include "search.h"

using namespace std;

namespace Clovis {

    namespace Search {

        U64 nodes;

        int pv_length[MAX_PLY];

        Move pv_table[MAX_PLY][MAX_PLY];

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
            MovePick::reset_killers();
            MovePick::reset_history();
            tt.clear();
        }

        // begin search
        Move start_search(Position& pos, SearchLimits& lim)
        {
            stop = false;
            allocated_time = lim.depth ? LLONG_MAX : 5 * lim.time[pos.side_to_move()] / (lim.moves_left + 5);
            tm.set();

            int alpha = INT_MIN;
            int beta = INT_MAX;
            int score;

            MoveGen::MoveList ml = MoveGen::MoveList(pos);
            if (ml.size() == 1)
            {
                **pv_table = *ml.begin();
                goto end;
            }

            MovePick::reset_killers();
            MovePick::age_history();

            nodes = 0;

            for (int depth = 1; depth <= MAX_PLY  && (lim.depth == 0 || depth <= lim.depth); ++depth)
            {
                score = negamax(pos, alpha, beta, depth, 0, false);

                if (stop)
                    break;

                cout << "info depth " << depth
                    << " score cp " << score
                    << " nodes " << nodes
                    << " time " << tm.get_time_elapsed()
                    << " pv ";
                for (int i = 0; i < *pv_length; ++i)
                    cout << move2str(pv_table[0][i]) << " ";
                cout << endl;

                if (tm.get_time_elapsed() > allocated_time / 3)
                    break;

                if (score <= alpha || score >= beta) 
                {
                    alpha = INT_MIN;
                    beta = INT_MAX;
                    --depth;
                    continue;
                }
                alpha = depth > asp_threshold_depth ? score - asp_window : INT_MIN;
                beta = depth > asp_threshold_depth ? score + asp_window : INT_MAX;
            }

        end:

            cout << "bestmove " << move2str(**pv_table) << endl;

            return **pv_table;
        }

        int negamax(Position& pos, int alpha, int beta, int depth, int ply, bool is_null)
        {
            if (nodes & 2047 && tm.get_time_elapsed() > allocated_time)
            {
                stop = true;
                return 0;
            }

            bool pv_node = beta - alpha != 1;

            pv_length[ply] = ply;

            if (depth <= 0)
                return quiescent(pos, alpha, beta);

            // avoid overflow
            if (ply >= MAX_PLY)
                return (Eval::evaluate(pos) + Eval::evaluate_pawns(pos)).get_score(pos.get_game_phase(), pos.side_to_move());

            ++nodes;

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
                pv_table[ply][ply] = tte->move;
                pv_length[ply] = 1;
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

            bool king_in_check = pos.is_king_in_check(pos.side_to_move());

            int score;

            if (king_in_check)
                goto loop;

            score = tte ? tte->eval : (Eval::evaluate(pos) + Eval::evaluate_pawns(pos)).get_score(pos.get_game_phase(), pos.side_to_move());

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
                && pos.has_promoted(pos.side_to_move())) 
            {
                pos.do_move(MOVE_NULL);
                score = -negamax(pos, -beta, -beta + 1, depth - 3, ply + 1, true);
                pos.undo_move(MOVE_NULL);
                if (score >= beta)
                    return beta;
            }

            // internal iterative deepening
            if (!tte
                && ((pv_node && depth >= 6)
                    || (!pv_node && depth >= 8))) 
            {
                negamax(pos, alpha, beta, pv_node ? depth - depth / 4 - 1 : (depth - 5) / 2, ply, false);
                tte = tt.probe(pos.get_key());
                if (tte)
                {
                    tt_move = tte->move;
                    pv_table[ply][ply] = tte->move;
                    pv_length[ply] = 1;
                }
            }

        loop:

            MovePick::MovePicker mp = MovePick::MovePicker(pos, ply, tt_move);

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
                    score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1, false);
                // late move reductions
                else
                {
                    if (depth > 2
                        && move_capture(curr_move) == NO_PIECE
                        && move_promotion_type(curr_move) == NO_PIECE)
                    {
                        int history_entry = MovePick::get_history_entry(other_side(pos.side_to_move()), curr_move);
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
                        score = -negamax(pos, -alpha - 1, -alpha, depth - R - 1, ply + 1, false);

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

                pos.undo_move(curr_move);

                if (stop)
                    return 0;

                // fail high
                if (score >= beta)
                {
                    if (move_capture(curr_move) == NO_PIECE)
                    {
                        mp.update_history(curr_move, depth);
                        MovePick::update_killers(curr_move, ply);
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
                        pv_table[ply][ply] = curr_move;

                        // record pv line
                        for (int i = ply + 1; i < pv_length[ply + 1]; ++i)
                            pv_table[ply][i] = pv_table[ply + 1][i];
                        
                        pv_length[ply] = pv_length[ply + 1];

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
                mp.update_history(best_move, depth);

            return alpha;
        }

        int quiescent(Position& pos, int alpha, int beta)
        {
            TTEntry* tte = tt.probe(pos.get_key());

            if (tte &&
                (tte->flags == HASH_EXACT
                    || (tte->flags == HASH_BETA && tte->eval >= beta)
                    || (tte->flags == HASH_ALPHA && tte->eval <= alpha)))
                return tte->eval;

            PTEntry* pte = tt.probe_pawn(pos.get_pawn_key());

            if (!pte)
            {
                tt.new_pawn_entry(pos.get_pawn_key(), Eval::evaluate_pawns(pos));
                pte = tt.probe_pawn(pos.get_pawn_key());
                assert(pte);
            }

            // this should maybe take place in evaluation function instead of here
            int eval = (Eval::evaluate(pos) + pte->eval).get_score(pos.get_game_phase(), pos.side_to_move());

            if (pos.is_insufficient(pos.side_to_move()))
            {
                if (pos.is_insufficient(other_side(pos.side_to_move())))
                    return DRAW_SCORE;
                eval = min(DRAW_SCORE, eval);
            }
            else if (pos.is_insufficient(other_side(pos.side_to_move())))
                eval = max(DRAW_SCORE, eval);

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

            MovePick::MovePicker mp = MovePick::MovePicker(pos, 0, (tte) ? tte->move : MOVE_NONE);
            Move curr_move;
            Move best_move = MOVE_NONE;
            int best_eval = INT_MIN;

            while ((curr_move = mp.get_next(true)) != MOVE_NONE) // only do winning caps once see is fixed
            {
                // illegal move or non capture
                if (!pos.do_move(curr_move))
                    continue;

                eval = -quiescent(pos, -beta, -alpha);

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