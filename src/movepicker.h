#pragma once

#include "position.h"
#include "movelist.h"
#include <algorithm>

namespace Clovis {

	enum StageType : int {
		TT_MOVE,
		INIT_CAPTURES,
		WIN_CAPTURES,
		INIT_QUIETS,
		QUIETS,
		LOSING_CAPTURES
	};

	namespace MovePick {

		class MovePicker {
		public:
			MovePicker(const Position& p, const Move* k, const int* h, int pl, Move ttm = MOVE_NONE) : pos(p) {
				ply = pl;
				curr = last = end_good_caps = end_bad_caps = moves;
				killers = k;
				history = h;
				tt_move = ttm;
				stage = TT_MOVE;
			}
			ScoredMove get_next(bool skip_quiets);
			void print();
		private:
			void score_captures();
			void score_quiets();
			ScoredMove score_capture_move(const Move m);
			ScoredMove score_quiet_move(const Move m);
			void sm_sort();
			ScoredMove* curr, *last, *end_good_caps, *end_bad_caps;
			ScoredMove moves[MAX_MOVES];
			const Position& pos;
			const Move* killers;
			const int* history;
			ScoredMove tt_move;
			int stage;
			int ply;
		};

	} // namespace MovePick

} // namespace Clovis