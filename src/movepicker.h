#pragma once

#include <algorithm>
#include <cstring>
#include <cassert>

#include "position.h"
#include "movelist.h"
#include "tt.h"

namespace Clovis {

	enum StageType : int {
		TT_MOVE,
		INIT_CAPTURES,
		WINNING_CAPTURES,
		INIT_QUIETS,
		QUIETS,
		LOSING_CAPTURES,
		FINISHED
	};

	namespace MovePick {

		extern int history_table[2 * 64 * 64];
		extern Move killers[2 * MAX_PLY];

		static int get_history_entry(Colour s, Move m) {
			return history_table[s * 4096 + move_from_sq(m) * 64 + move_to_sq(m)];
		}

		static int* get_history_entry_ptr(Colour s, Move m) {
			return &history_table[s * 4096 + move_from_sq(m) * 64 + move_to_sq(m)];
		}

		static void clear() {
			memset(history_table, 0, sizeof(history_table));
			memset(killers, 0, sizeof(killers));
		}

		static void update_killers(Move m, int ply) {
			Move* k = &killers[ply * 2];
			if (*k != m)
			{
				k[1] = *k;
				*k = m;
			}
		}

		static bool is_killer(Move m, int ply) {
			Move* k = &killers[ply * 2];
			return (m == *k || m == k[1]);
		}

		class MovePicker {
		public:
			MovePicker(const Position& p, int pl, Move ttm = MOVE_NONE) : pos(p) {
				ply = pl;
				curr = last = end_bad_caps = moves;
				tt_move = ttm;
				stage = TT_MOVE;
			}
			void update_history(Move best_move, int depth);
			Move get_next(bool skip_quiets);
			int get_stage() const { return stage; }
			void print();
		private:
			void score_captures();
			void score_quiets();
			const Position& pos;
			ScoredMove *curr, *last, *end_bad_caps;
			ScoredMove moves[MAX_MOVES];
			Move tt_move;
			int stage;
			int ply;
		};

	} // namespace MovePick

} // namespace Clovis