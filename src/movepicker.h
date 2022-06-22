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
			if (killers[ply] != m)
			{
				killers[MAX_PLY + ply] = killers[ply];
				killers[ply] = m;
			}
		}

		static bool is_killer(Move m, int ply) {
			return (m == killers[MAX_PLY + ply] || m == killers[ply]);
		}

		class MovePicker {
		public:
			MovePicker(const Position& p, int pl, Move ttm = MOVE_NONE) : pos(p) {
				ply = pl;
				curr = last = end_bad_caps = memory_index = last_searched_quiet = moves;
				tt_move = ttm;
				stage = TT_MOVE;
			}
			void update_history(Move best_move, int depth);
			ScoredMove get_next(bool skip_quiets);
			void set_quiet_boundaries();
			bool remember_quiets(Move& m);
			int get_stage() const { return stage; }
			void print();
		private:
			void score_captures();
			void score_quiets();
			ScoredMove score_capture_move(const Move m);
			ScoredMove score_quiet_move(const Move m);
			void sm_sort();
			ScoredMove* curr, *last, *end_bad_caps, *memory_index, *last_searched_quiet;
			ScoredMove moves[MAX_MOVES];
			const Position& pos;
			ScoredMove tt_move;
			int stage;
			int ply;
		};

	} // namespace MovePick

} // namespace Clovis