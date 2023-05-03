#include "movepicker.h"

using namespace std;

namespace Clovis {

	namespace MovePick {

		array<int, cft_size> history_table;
		array<Move, cft_size> counter_table;
		array<KEntry, MAX_PLY> killer_table;

		// return the next ordered move
		Move MovePicker::get_next(bool play_quiets) {
			switch (stage) {
			case TT_MOVE:
				++stage;
				if (tt_move != MOVE_NONE && (play_quiets || move_capture(tt_move) || move_promotion_type(tt_move)))
					return tt_move;
				[[fallthrough]];
			case INIT_CAPTURES:
				curr = last_bad_cap = moves.data();
				last = MoveGen::generate<ScoredMove, CAPTURE_MOVES>(pos, moves.data());
				score_captures();
				sort(moves.data(), last, [](const ScoredMove& lhs, const ScoredMove& rhs) {
					return lhs.score > rhs.score; });
				++stage;
				[[fallthrough]];
			case WINNING_CAPTURES:
				while (curr < last) {
					assert(move_capture(*curr) || piece_type(move_promotion_type(*curr)) == QUEEN);
					if (curr->move == tt_move)
						++curr;
					else if (pos.see_ge(*curr, play_quiets ? -100 : 0))
						return *curr++;
					else if (play_quiets)
						*last_bad_cap++ = *curr++;
					else
						++curr;
				}
				++stage;
				[[fallthrough]];
			case INIT_QUIETS:
				if (play_quiets) {
					curr = last_bad_cap;
					last = MoveGen::generate<ScoredMove, QUIET_MOVES>(pos, curr);
					score_quiets();
					sort(last_bad_cap, last, [](const ScoredMove& lhs, const ScoredMove& rhs) {
						return lhs.score > rhs.score; });
				}
				++stage;
				[[fallthrough]];
			case QUIETS:
				while (play_quiets && curr < last) {
					assert(!move_capture(*curr) || piece_type(move_promotion_type(*curr)) != QUEEN);
					if (*curr != tt_move)
						return *curr++;
					++curr;
				}
				curr = moves.data();
				++stage;
				[[fallthrough]];
			case LOSING_CAPTURES:
				if (play_quiets) {
					while (curr < last_bad_cap) {
						assert(move_capture(*curr));
						if (*curr != tt_move)
							return *curr++;
						++curr;
					}
				}
				++stage;
				[[fallthrough]];
			default:
				break;
			}

			return MOVE_NONE;
		}

		void MovePicker::score_captures() {
			for (auto& sm : ranges::subrange(moves.data(), last))
				// promotions supercede mvv-lva
				sm.score = mvv_lva[move_piece_type(sm)][pos.pc_table[move_to_sq(sm)]] + ((move_promotion_type(sm) << 6));
		}

		void MovePicker::score_quiets() {

			auto counter = get_counter_entry(pos.side, prev_move);

			for (auto& sm : ranges::subrange(last_bad_cap, last)) {
				if      (sm == killer_table[ply].primary)   sm.score = 22000;
				else if (sm == killer_table[ply].secondary) sm.score = 21000;
				else if (sm == counter)                sm.score = 20000;
				else sm.score = get_history_entry(pos.side, sm);
			}
		}

	} // namespace MovePick

} // namespace Clovis
