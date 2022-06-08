#include "search.h"

namespace Clovis {

    namespace Search {

        int nodes;

        Move best_move;

        // begin search
        Move start_search(Position& pos, SearchLimits& lim)
        {
            TimePoint allocated_time = lim.time[pos.side_to_move()] / 20;
            TimeManager tm;
            tm.set();
            nodes = 0;
            int alpha = NEG_INF;
            int beta = POS_INF;
            int score;
            for (int depth = 1; depth <= MAX_PLY; ++depth) {
                score = negamax(pos, alpha, beta, depth, 0);
                std::cout << "info depth " << depth
                    << " score cp " << score
                    << " nodes " << nodes
                    << " currmove " << UCI::move2str(best_move)
                    << std::endl;
                if (tm.get_time_elapsed() > allocated_time)
                    break;
            }
            std::cout << "time " << tm.get_time_elapsed() << std::endl;
            std::cout << "bestmove " << UCI::move2str(best_move) << std::endl;
            return best_move;
        }

        int negamax(Position& pos, int alpha, int beta, int depth, int ply)
        {
            if (depth == 0)
                return quiescent(pos, alpha, beta);

            ++nodes;
            int score = NEG_INF;

            for (const auto& m : MoveList(pos))
            {
                // illegal move
                if (pos.do_move(m) == false) continue;

                score = -negamax(pos, -beta, -alpha, depth - 1, ply + 1);

                pos.undo_move(m);

                if (score > alpha)
                {
                    // fail high
                    if (score >= beta)
                    {
                        return beta;
                    }

                    // new best move found
                    alpha = score;

                    if (ply == 0) {
                        best_move = m;
                    }
                }
            }
            if (score == NEG_INF)
                return pos.is_king_in_check(pos.side_to_move()) ? - CHECKMATE_SCORE + ply : - DRAW_SCORE;

            return alpha;
        }

        int quiescent(Position& pos, int alpha, int beta)
        {
            ++nodes;

            return Eval::evaluate(pos);


            int score = Eval::evaluate(pos);

            if (score >= beta)
                return beta;

            int best_score = score;

            for (const auto& m : MoveList(pos))
            {
                // illegal move or non capture
                if (pos.do_move(m, true) == false) continue;

                score = -quiescent(pos, -beta, -alpha);

                pos.undo_move(m);

                if (score > best_score)
                {
                    // fail high
                    if (score >= beta)
                    {
                        return beta;
                    }
                    else if (score > alpha) 
                    {
                        // new best move found
                        alpha = score;
                    }
                }
            }

            return best_score;
        }

        // initialize LMR lookup table values
        void init_search()
        {

        }

        // reset transposition table, set search to standard conditions
        void clear()
        {

        }

    } // namespace Search

} // namespace Clovis