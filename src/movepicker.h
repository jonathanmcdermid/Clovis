#pragma once

#include "position.h"
#include "movelist.h"
#include <algorithm>

namespace Clovis {

	namespace MovePick {

		class MovePicker {
		public:
			MovePicker(const Position& pos, const Move* k, const int* h, int p, Move ttm = MOVE_NONE) {
				last = gen_moves(pos, moves);
				ply = p;
				curr = moves;
				killers = k;
				history = h;
				tt_move = ttm;
				score(pos);
			}
			void score(const Position& pos);
			ScoredMove score_move(const Position& pos, const Move m);
			void sm_sort();
			ScoredMove get_next();
			const ScoredMove* begin() const { return moves; }
			const ScoredMove* end() const { return last; }
		private:
			ScoredMove* curr, * last;
			ScoredMove moves[MAX_MOVES];
			const Move* killers;
			const int* history;
			Move tt_move;
			int ply;
		};

	} // namespace MovePick

} // namespace Clovis