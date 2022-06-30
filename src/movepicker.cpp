#include "movepicker.h"

namespace Clovis {

	namespace MovePick {

        int history_table[2 * 64 * 64];
        Move killers[2 * MAX_PLY];

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

        void MovePicker::update_history(Move best_move, int depth) {
            int bonus = std::min(depth * depth, 400);
            int* history_entry;
            Move m;
            set_quiet_boundaries();
            while (remember_quiets(m))
            {
                assert(move_capture(m) == false);
                history_entry = get_history_entry_ptr(pos.side_to_move(), m);
                if (m == best_move)
                    *history_entry += 32 * bonus - *history_entry * bonus / 512;
                else
                    *history_entry += 32 * (-bonus) - *history_entry * bonus / 512;
            }
        }

		// return the next ordered move
		ScoredMove MovePicker::get_next(bool skip_quiets)
		{
        start:
            switch (stage)
            {
            case TT_MOVE:
                ++stage;
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
                if (curr < last)
                {
                    if (curr->m == tt_move.m)
                    {
                        ++curr;
                        goto start;
                    }
                    if (pos.see(curr->m, 0))
                        return *curr++;
                    else 
                    {
                        *end_bad_caps++ = *curr++;
                        goto start;
                    }
                }
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
                curr = moves;
                ++stage;
                goto start;
            case LOSING_CAPTURES:
                if (curr < end_bad_caps)
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

        void MovePicker::set_quiet_boundaries()
        {
            switch (stage)
            {
            case QUIETS:
                memory_index = end_bad_caps;
                last_searched_quiet = curr;
                break;
            case LOSING_CAPTURES:
                memory_index = end_bad_caps;
                last_searched_quiet = last;
                break;
            default:
                memory_index = last_searched_quiet = moves;
                break;
            }
        }

        bool MovePicker::remember_quiets(Move& m)
        {
            if (memory_index == last_searched_quiet) 
                return false;
            else 
            {
                m = memory_index->m;
                memory_index++;
                return true;
            }
        }

        void MovePicker::score_captures()
        {
            for (ScoredMove* sm = moves; sm != last; ++sm) 
            {
                *sm = score_capture_move(sm->m);
            }
        }

        void MovePicker::score_quiets()
        {
            for (ScoredMove* sm = moves; sm != last; ++sm) 
            {
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
            if (killers[ply * 2] == m)
                sm.score = 22000;
            else if (killers[ply * 2 + 1] == m)
                sm.score = 21000;
            else if (move_promotion_type(m) || move_castling(m))
                sm.score = 20000 + move_promotion_type(m);
            else
                sm.score = get_history_entry(pos.side_to_move(), sm.m);
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
