#pragma once

#include "position.h"
#include "movelist.h"
#include <algorithm>

namespace Clovis {

	namespace MovePick {

		class MovePicker {
		public:
			MovePicker(const Position& pos) {
				last = gen_moves(pos, moves);
				curr = moves;
			}
			void sm_sort();
			ScoredMove get_next();
			const ScoredMove* begin() const { return moves; }
			const ScoredMove* end() const { return last; }
		private:
			ScoredMove* curr, * last;
			ScoredMove moves[MAX_MOVES];
		};

	} // namespace MovePick

} // namespace Clovis