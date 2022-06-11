#include "movepicker.h"

namespace Clovis {

	namespace MovePick {

        // MVV-LVA lookup table [attacker][victim]
        constexpr int mvv_lva[15][15] = {
              0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
              0, 105, 205, 305, 405, 505, 605,    0,   0, 105, 205, 305, 405, 505, 605,
              0, 104, 204, 304, 404, 504, 604,    0,   0, 104, 204, 304, 404, 504, 604,
              0, 103, 203, 303, 403, 503, 603,    0,   0, 103, 203, 303, 403, 503, 603,
              0, 102, 202, 302, 402, 502, 602,    0,   0, 102, 202, 302, 402, 502, 602,
              0, 101, 201, 301, 401, 501, 601,    0,   0, 101, 201, 301, 401, 501, 601,
              0, 100, 200, 300, 400, 500, 600,    0,   0, 100, 200, 300, 400, 500, 600,
              0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
              0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
              0, 105, 205, 305, 405, 505, 605,    0,   0, 105, 205, 305, 405, 505, 605,
              0, 104, 204, 304, 404, 504, 604,    0,   0, 104, 204, 304, 404, 504, 604,
              0, 103, 203, 303, 403, 503, 603,    0,   0, 103, 203, 303, 403, 503, 603,
              0, 102, 202, 302, 402, 502, 602,    0,   0, 102, 202, 302, 402, 502, 602,
              0, 101, 201, 301, 401, 501, 601,    0,   0, 101, 201, 301, 401, 501, 601,
              0, 100, 200, 300, 400, 500, 600,    0,   0, 100, 200, 300, 400, 500, 600
        };

		// return the next ordered move
		ScoredMove MovePicker::get_next(bool skip_quiets)
		{
        start:
            switch (stage)
            {
            case TT_MOVE:
                ++stage;////////////////// NEED TO AVOID TT MOVE BEING SEARCHED TWICE
                if (tt_move != MOVE_NONE)
                    return tt_move;
                else
                    goto start;
            case INIT_CAPTURES:
                curr = end_bad_caps = moves;
                last = gen_cap_moves(pos, curr);
                score_captures();
                sm_sort();
                ++stage;
                goto start;
            case WIN_CAPTURES:
                // IF SEE FAILS, ADD MOVE TO BAD CAPS HERE
                if (curr < last)
                {
                    if (curr->m == tt_move.m)
                    {
                        ++curr;
                        goto start;
                    }
                    return *curr++;
                }
                else
                    end_good_caps = curr;
                ++stage;
            case INIT_QUIETS:
                if (skip_quiets == false)
                {
                    curr = end_bad_caps;
                    last = gen_quiet_moves(pos, curr);
                    score_quiets();
                    sm_sort();
                }
                ++stage;
                goto start;
            case QUIETS:
                if (skip_quiets == false && curr < last)
                {
                    if (curr->m == tt_move.m)
                    {
                        ++curr;
                        goto start;
                    }
                    return *curr++;
                }
                curr = end_good_caps;
                last = end_bad_caps;
                ++stage;
                goto start;
            case LOSING_CAPTURES:
                if (curr < last)
                {
                    if (curr->m == tt_move.m)
                    {
                        ++curr;
                        goto start;
                    }
                    return *curr++;
                }
                break;
            default:
                break;
            }

	        return ScoredMove();
		}

        void MovePicker::score_captures()
        {
            for (ScoredMove* sm = moves; sm != last; ++sm) {
                *sm = score_capture_move(sm->m);
            }
        }

        void MovePicker::score_quiets()
        {
            for (ScoredMove* sm = moves; sm != last; ++sm) {
                *sm = score_quiet_move(sm->m);
            }
        }

        ScoredMove MovePicker::score_capture_move(const Move m)
        {
            ScoredMove sm;
            sm.m = m;
            sm.score = mvv_lva[move_piece_type(m)][pos.piece_on(move_to_sq(m))] + move_promotion_type(m);
            return sm;
        }

        ScoredMove MovePicker::score_quiet_move(const Move m)
        {
            ScoredMove sm;
            sm.m = m;
            if (killers[ply] == m)
                sm.score = 9999;
            else if (killers[MAX_PLY + ply] == m)
                sm.score = 8888;
            else if (move_promotion_type(m))
                sm.score = 5000 + move_promotion_type(m);
            else
                sm.score = history[move_piece_type(m) * SQ_N + move_to_sq(m)];
            return sm;
        }

		void MovePicker::sm_sort() {
			std::sort(curr, curr + (last - curr), sm_score_comp);
		}

        void MovePicker::print()
        {
            std::cout << "\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\tscore\n\n";

            int move_count = 0;
            for (ScoredMove* m = moves; m != last; ++m, ++move_count)
            {
                std::cout << sq2str(move_from_sq(m->m))
                    << sq2str(move_to_sq(m->m))
                    << piece_str[move_promotion_type(m->m)] << '\t'
                    << piece_str[move_piece_type(m->m)] << '\t'
                    << int(move_capture(m->m)) << '\t'
                    << int(move_double(m->m)) << '\t'
                    << int(move_enpassant(m->m)) << '\t'
                    << int(move_castling(m->m)) << '\t'
                    << m->score << '\n';

            }
            std::cout << "\n\nTotal move count:" << move_count;
        }

		inline static bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
			return lhs.score > rhs.score;
		}

	} // namespace MovePick

} // namespace Clovis
