#pragma once

#include <algorithm>
#include <cstring>
#include <cassert>

#include "position.h"
#include "movelist.h"
#include "tt.h"

namespace Clovis {

	namespace MovePick {

		// colour * from square * to square table size
		constexpr size_t cft_size = COLOUR_N * SQ_N * SQ_N;

		extern int history_table[cft_size];
		extern Move counter_table[cft_size];
		extern Move killers[2 * MAX_PLY];
		extern int history_bonus[MAX_PLY + 1];

		constexpr int cft_index(Colour c, Move m) {
			return c * SQ_N * SQ_N + move_from_sq(m) * SQ_N + move_to_sq(m);
		}

		inline int get_history_entry(Colour c, Move m) {
			return history_table[cft_index(c, m)];
		}

		inline int* get_history_entry_ptr(Colour c, Move m) {
			return &history_table[cft_index(c, m)];
		}

		inline Move get_counter_entry(Colour c, Move m) {
			return counter_table[cft_index(c, m)];
		}

		inline void age_history() {
			for (auto& it : history_table)
				it /= 16;
		}

		inline void reset_counter() {
			memset(counter_table, 0, sizeof(counter_table));
		}

		inline void reset_killers() {
			memset(killers, 0, sizeof(killers));
		}

		inline void reset_history() {
			memset(history_table, 0, sizeof(history_table));
		}

		inline void clear() {
			reset_counter();
			reset_killers();
			reset_history();
		}

		inline void update_killers(Move m, int ply) {
			Move* k = &killers[ply * 2];
			if (k[0] != m)
			{
				k[1] = k[0];
				k[0] = m;
			}
		}

		inline void update_history_entry(Move move, Colour side, int bonus) {
			int* history_entry = get_history_entry_ptr(side, move);
			*history_entry += 32 * bonus - *history_entry * abs(bonus) / 512;
		}

		inline void update_counter_entry(Colour c, Move prev, Move curr) {
			counter_table[cft_index(c, prev)] = curr;
		}

		inline bool is_killer(Move m, int ply) {
			Move* k = &killers[ply * 2];
			return (m == k[0] || m == k[1]);
		}

		void init_movepicker();

		void test_movepicker();

		class MovePicker {
		public:
			MovePicker(const Position& pos, int ply, Move prev_move, Move tt_move) : pos(pos), ply(ply), tt_move(tt_move), prev_move(prev_move) {
				curr = last = end_bad_caps = moves;
				stage = TT_MOVE;
			}
			template<HashFlag hf> void update_history(Move best_move, int depth);
			Move get_next(bool skip_quiets);
			int get_stage() const { return stage; }
			void print();
		private:
			void score_captures();
			void score_quiets();
			const Position& pos;
			int ply;
			ScoredMove *curr, *last, *end_bad_caps;
			ScoredMove moves[MAX_MOVES];
			Move tt_move, prev_move;
			int stage;
		};

		template<HashFlag hf>
		void MovePicker::update_history(Move best_move, int depth)
		{
			assert(!move_capture(best_move));

			update_history_entry(best_move, pos.stm(), history_bonus[depth]);

			ScoredMove* last_searched_quiet = (hf == HASH_EXACT) ? last : curr;

			for (ScoredMove* sm = end_bad_caps; sm < last_searched_quiet; ++sm)
			{
				assert(!move_capture(*sm));
				if (*sm != best_move)
					update_history_entry(*sm, pos.stm(), -history_bonus[depth]);
			}
		}

	} // namespace MovePick

} // namespace Clovis
