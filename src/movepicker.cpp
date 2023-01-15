#include "movepicker.h"

using namespace std;

namespace Clovis {

	namespace MovePick {

        int history_table[];
        Move counter_table[];
        Move killers[];

        // initialize lookup tables and precalculated values
        void init_movepicker()
        {
            /*for (int i = 0; i <= MAX_PLY; ++i)
                history_bonus[i] = min(i * i, 400);

			for (PieceType att = PAWN; att <= KING; ++att)
            {
                for (PieceType vic = PAWN; vic <= KING; ++vic)
                {
                    mvv_lva[make_piece(att, WHITE)][make_piece(vic, BLACK)] = ((vic - 1) << 3) - att + 6;
                    mvv_lva[make_piece(att, BLACK)][make_piece(vic, WHITE)] = ((vic - 1) << 3) - att + 6;
                }
            }*/
		}

		// return the next ordered move
		Move MovePicker::get_next(bool play_quiets)
		{
			switch (stage)
			{
			case TT_MOVE:
				++stage;
				if (tt_move != MOVE_NONE && (play_quiets || move_capture(tt_move)))
					return tt_move;
			case INIT_CAPTURES:
				curr = end_bad_caps = moves;
				last = MoveGen::generate<ScoredMove, CAPTURE_MOVES>(pos, moves);
				score_captures();
				sort(moves, last, sm_score_comp);
				++stage;
			case WINNING_CAPTURES:
				while (curr < last)
				{
					assert(move_capture(*curr));
					if (curr->move == tt_move)
						++curr;
					else if (pos.see(*curr) >= 0)
						return *curr++;
					else
						*end_bad_caps++ = *curr++;
				}
				++stage;
			case INIT_QUIETS:
				if (play_quiets)
				{
					curr = end_bad_caps;
					last = MoveGen::generate<ScoredMove, QUIET_MOVES>(pos, curr);
					score_quiets();
					sort(end_bad_caps, last, sm_score_comp);
				}
				++stage;
			case QUIETS:
				while (play_quiets && curr < last)
				{
					assert(!move_capture(*curr));
					if (*curr != tt_move)
						return *curr++;
					++curr;
				}
				curr = moves;
				++stage;
			case LOSING_CAPTURES:
				while (curr < end_bad_caps)
				{
					assert(move_capture(*curr));
					if (*curr != tt_move)
						return *curr++;
					++curr;
				}
				++stage;
				break;
			default:
				break;
			}

			return MOVE_NONE;
		}

        void MovePicker::score_captures()
        {
            for (ScoredMove* sm = moves; sm < last; ++sm)
                // promotions supercede mvv-lva
                sm->score = mvv_lva[move_piece_type(*sm)][pos.piece_board[move_to_sq(*sm)]] + ((move_promotion_type(*sm) << 6));
        }

        void MovePicker::score_quiets()
        {
            size_t primary_index = ply << 1;
            size_t secondary_index = primary_index + 1;

            Move counter_move = get_counter_entry(pos.side, prev_move);

            for (ScoredMove* sm = end_bad_caps; sm < last; ++sm)
            {
                if (*sm == killers[primary_index])
                    sm->score = 22000;
                else if (*sm == killers[secondary_index])
                    sm->score = 21000;
                else if (move_promotion_type(*sm))
                    sm->score = 20000 + move_promotion_type(*sm);
                else if (*sm == counter_move)
                    sm->score = 20000;
                else
                    sm->score = get_history_entry(pos.side, *sm);
            }
        }

        void MovePicker::print()
        {
            cout << "\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\tscore\n\n";

            int count = 0;
            for (ScoredMove* sm = moves; sm != last; ++sm, ++count)
            {
                cout << move_from_sq(*sm)
                    << move_to_sq(*sm)
                    << piece_str[move_promotion_type(*sm)] << '\t'
                    << piece_str[move_piece_type(*sm)] << '\t'
                    << int(move_capture(*sm)) << '\t'
                    << int(move_double(*sm)) << '\t'
                    << int(move_enpassant(*sm)) << '\t'
                    << int(move_castling(*sm)) << '\t'
                    << sm->score << '\n';

            }
            cout << "\n\nTotal move count:" << count;
        }

	} // namespace MovePick

} // namespace Clovis
