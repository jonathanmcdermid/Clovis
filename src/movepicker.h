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

		static int get_history_entry(Colour side, Move move) {
			return history_table[side * 4096 + move_from_sq(move) * 64 + move_to_sq(move)];
		}

		static int* get_history_entry_ptr(Colour side, Move move) {
			return &history_table[side * 4096 + move_from_sq(move) * 64 + move_to_sq(move)];
		}

		static void age_history()
		{
			for (int i = 0; i < 2 * 64 * 64; ++i)
				history_table[i] /= 16;
		}

		static void reset_killers()
		{
			memset(killers, 0, sizeof(killers));
		}

		static void reset_history() {
			memset(history_table, 0, sizeof(history_table));
		}

		static void update_killers(Move move, int ply) {
			Move* k = &killers[ply * 2];
			if (k[0] != move)
			{
				k[1] = k[0];
				k[0] = move;
			}
		}

		static void update_history_entry(Move move, Colour side, int bonus) {
			int* history_entry = get_history_entry_ptr(side, move);
			*history_entry += 32 * bonus - *history_entry * abs(bonus) / 512;
		}

		static bool is_killer(Move move, int ply) {
			Move* k = &killers[ply * 2];
			return (move == k[0] || move == k[1]);
		}

		void init_movepicker();

		void test_movepicker();

		class MovePicker {
		public:
			MovePicker(const Position& pos, int ply, Move tt_move = MOVE_NONE) : pos(pos), ply(ply), tt_move(tt_move) {
				curr = last = end_bad_caps = moves;
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