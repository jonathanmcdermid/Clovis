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
		ScoredMove MovePicker::get_next()
		{
			if (curr == last)
				return ScoredMove();
			return *curr++;
		}

        void MovePicker::score(const Position& p)
        {
            for (ScoredMove* sm = moves; sm != last; ++sm) {
                *sm = score_move(p, sm->m);
            }
        }

        ScoredMove MovePicker::score_move(const Position& pos, const Move m)
        {
            ScoredMove sm;
            sm.m = m;

            if (m == tt_move) 
            {
                sm.score = 15000;
            }
            else if (move_capture(m))
            {
                sm.score = 10000 + mvv_lva[move_piece_type(m)][pos.piece_on(move_to_sq(m))];
            }
            else
            {
                if (killers[ply] == m)
                    sm.score = 9999;
                else if (killers[MAX_PLY + ply] == m)
                    sm.score = 8888;
                else if (move_promotion_type(m))
                    sm.score = 5000 + move_promotion_type(m);
                else
                    sm.score = history[move_piece_type(m) * SQ_N + move_to_sq(m)];
            }
            return sm;
        }

		void MovePicker::sm_sort() {
			std::sort(curr, curr + (last - curr), sm_score_comp);
		}

		inline static bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
			return lhs.score > rhs.score;
		}

	} // namespace MovePick

} // namespace Clovis
