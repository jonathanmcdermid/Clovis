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
		extern Move killers[MAX_PLY << 1];

		constexpr int history_bonus[MAX_PLY + 1] = {
			    0, 
               32,   128,   288,   512,   800,  1152,  1568,  2048, 
             2592,  3200,  3872,  4608,  5408,  6272,  7200,  8192, 
             9248, 10368, 11552, 12800, 12800, 12800, 12800, 12800, 
            12800, 12800, 12800, 12800, 12800, 12800, 12800, 12800, 
            12800, 12800, 12800, 12800, 12800, 12800, 12800, 12800, 
            12800, 12800, 12800, 12800, 12800, 12800, 12800, 12800, 
            12800, 12800, 12800, 12800, 12800, 12800, 12800, 12800, 
            12800, 12800, 12800, 12800, 12800, 12800, 12800, 12800,
		};

        // MVV-LVA lookup table [attacker][victim]
        constexpr int mvv_lva[15][15] = {
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  5, 13, 21, 29, 37, 45, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12, 20, 28, 36, 44, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  3, 11, 19, 27, 35, 43, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  2, 10, 18, 26, 34, 42, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  9, 17, 25, 33, 41, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 16, 24, 32, 40, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  5, 13, 21, 29, 37, 45,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  4, 12, 20, 28, 36, 44,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  3, 11, 19, 27, 35, 43,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  2, 10, 18, 26, 34, 42,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  1,  9, 17, 25, 33, 41,  0,  0,  0,  0,  0,  0,  0,  0, 
			0,  0,  8, 16, 24, 32, 40,  0,  0,  0,  0,  0,  0,  0,  0, 
		};

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
				it >>= 4;
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
			Move* k = &killers[ply << 1];
			if (k[0] != m)
			{
				k[1] = k[0];
				k[0] = m;
			}
		}

		inline void update_history_entry(Move move, Colour side, int bonus) {
			int* history_entry = get_history_entry_ptr(side, move);
			*history_entry += bonus - (*history_entry * abs(bonus) >> 14);
		}

		inline void update_counter_entry(Colour c, Move prev, Move curr) {
			counter_table[cft_index(c, prev)] = curr;
		}

		constexpr bool is_killer(Move m, int ply) {
			Move* k = &killers[ply << 1];
			return (m == k[0] || m == k[1]);
		}

		class MovePicker {
		public:
			MovePicker(const Position& p, int pl, Move pm, Move ttm) 
				: pos(p), ply(pl), prev_move(pm), tt_move(ttm) {
				curr = last = end_bad_caps = moves;
				stage = TT_MOVE;
			}
			template<HashFlag HF> void update_history(Move best_move, int depth);
			Move get_next(bool play_quiets);
		private:
			void score_captures();
			void score_quiets();
			const Position& pos;
			int ply;
			ScoredMove* curr, *last, *end_bad_caps;
			ScoredMove moves[MAX_MOVES];
			Move prev_move, tt_move;
			int stage;
		};
	
		template<HashFlag HF>
		void MovePicker::update_history(Move best_move, int depth)
		{
			ScoredMove* last_searched_quiet = HF == HASH_EXACT ? last : curr;

			assert(!move_capture(best_move) || move_promotion_type(*sm));

			update_history_entry(best_move, pos.side, history_bonus[depth]);

			for (ScoredMove* sm = end_bad_caps; sm < last_searched_quiet; ++sm)
			{
				assert(!move_capture(*sm) || move_promotion_type(*sm));
				if (*sm != best_move)
					update_history_entry(*sm, pos.side, -history_bonus[depth]);
			}
		}

	} // namespace MovePick

} // namespace Clovis
