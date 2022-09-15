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

        inline static bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
            return lhs.score > rhs.score;
        }

        void MovePicker::update_history(Move best_move, int depth) 
        {
            ScoredMove* last_searched_quiet;
            switch (stage)
            {
            case QUIETS:
                last_searched_quiet = curr;
                break;
            case LOSING_CAPTURES:
                last_searched_quiet = last;
                break;
            default:
                return;
            }
            int bonus = std::min(depth * depth, 400);
            int* history_entry;
            for(ScoredMove* m = end_bad_caps; m < last_searched_quiet; ++m)
            {
                assert(move_capture(m->m) == false);
                history_entry = get_history_entry_ptr(pos.side_to_move(), m->m);
                if (m->m == best_move)
                    *history_entry += 32 * bonus - *history_entry * bonus / 512;
                else
                    *history_entry += -32 * bonus - *history_entry * bonus / 512;
            }
        }

		// return the next ordered move
		Move MovePicker::get_next(bool skip_quiets)
		{
            switch (stage)
            {
            case TT_MOVE:
                ++stage;
                if (tt_move != MOVE_NONE && (skip_quiets == false || move_capture(tt_move) != NO_PIECE))
                    return tt_move;
            case INIT_CAPTURES:
                curr = end_bad_caps = moves;
                last = gen_cap_moves<ScoredMove>(pos, moves);
                score_captures();
                std::sort(curr, last, sm_score_comp);
                ++stage;
            case WINNING_CAPTURES:
                while (curr < last)
                {
                    if (curr->m == tt_move)
                        ++curr;
                    else if (pos.see(curr->m))
                        return *curr++;
                    else 
                        *end_bad_caps++ = *curr++;
                }
                ++stage;
            case INIT_QUIETS:
                if (skip_quiets == false)
                {
                    curr = end_bad_caps;
                    last = gen_quiet_moves<ScoredMove>(pos, moves);
                    score_quiets();
                    std::sort(curr, last, sm_score_comp);
                }
                ++stage;
            case QUIETS:
                while (skip_quiets == false && curr < last)
                {
                    if (curr->m != tt_move)
                        return *curr++;
                    ++curr;
                }
                curr = moves;
                ++stage;
            case LOSING_CAPTURES:
                while (curr < end_bad_caps)
                {
                    if (curr->m != tt_move)
                        return *curr++;
                    ++curr;
                }
                break;
            default:
                break;
            }

	        return MOVE_NONE;
		}

        void MovePicker::score_captures()
        {
            for (ScoredMove* sm = moves; sm != last; ++sm) 
                sm->score = mvv_lva[move_piece_type(sm->m)][pos.piece_on(move_to_sq(sm->m))] + move_promotion_type(sm->m);
        }

        void MovePicker::score_quiets()
        {
            for (ScoredMove* sm = end_bad_caps; sm != last; ++sm)
            {
                if (killers[ply * 2] == sm->m)
                    sm->score = 22000;
                else if (killers[ply * 2 + 1] == sm->m)
                    sm->score = 21000;
                else if (move_promotion_type(sm->m) || move_castling(sm->m))
                    sm->score = 20000 + move_promotion_type(sm->m);
                else
                    sm->score = get_history_entry(pos.side_to_move(), sm->m);
            }
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

	} // namespace MovePick

} // namespace Clovis
