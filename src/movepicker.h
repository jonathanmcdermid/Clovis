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

		struct KEntry {
			Move primary{ MOVE_NONE }, secondary{ MOVE_NONE };
		};

		extern std::array<int, cft_size> history_table;
		extern std::array<Move, cft_size> counter_table;
		extern std::array<KEntry, MAX_PLY> killer_table;

		constexpr auto history_bonus = [] {
			std::array<int, MAX_PLY + 1> arr{};

			for (int i = 0; i <= MAX_PLY; ++i)
				arr[i] = 32 * std::min(i * i, 400);

			return arr;
		}();

		constexpr auto mvv_lva = [] {
			std::array<std::array<int, 15>, 15> arr{};

			for (Colour c : { WHITE, BLACK })
				for (PieceType p1 = PAWN; p1 <= KING; ++p1)
					for (PieceType p2 = PAWN; p2 <= KING; ++p2)
						arr[make_piece(p1, c)][make_piece(p2, ~c)] = KING - p1 + KING * p2;

			return arr;
		}();

		inline int cft_index(Colour c, Move m) {
			return c * SQ_N * SQ_N + move_from_sq(m) * SQ_N + move_to_sq(m);
		}

		inline int& get_history_entry(Colour c, Move m) {
			return history_table[cft_index(c, m)];
		}

		inline Move& get_counter_entry(Colour c, Move m) {
			return counter_table[cft_index(c, m)];
		}

		inline void age_history() {
			for (auto& it : history_table)
				it >>= 4;
		}

		inline void reset_counter() {
			counter_table.fill(MOVE_NONE);
		}

		inline void reset_killers() {
			killer_table.fill({ MOVE_NONE, MOVE_NONE });
		}

		inline void reset_history() {
			history_table.fill(0);
		}

		inline void clear() {
			reset_counter();
			reset_killers();
			reset_history();
		}

		inline void update_killers(Move m, int ply) {
			if (killer_table[ply].primary != m) {
				killer_table[ply].secondary = killer_table[ply].primary;
				killer_table[ply].primary = m;
			}
		}

		inline void update_history_entry(Move m, Colour c, int bonus) {
			int& history_entry = history_table[cft_index(c, m)];
			history_entry += bonus - (history_entry * abs(bonus) >> 14);
		}

		inline void update_counter_entry(Colour c, Move prev, Move curr) {
			counter_table[cft_index(c, prev)] = curr;
		}

		inline bool is_killer(Move m, int ply) {
			return m == killer_table[ply].primary || m == killer_table[ply].secondary;
		}

		class MovePicker {
		public:
			MovePicker(const Position& p, int pl, Move pm, Move ttm) 
				: pos(p), ply(pl), stage(TT_MOVE), curr(moves.data()), last(moves.data()),
				last_bad_cap(moves.data()), prev_move(pm), tt_move(ttm) {}
			Move get_next(bool play_quiets);
			template<HashFlag HF> void update_history(Move best_move, int depth);
		private:
			void score_captures();
			void score_quiets();
			const Position& pos;

			int ply, stage;
			ScoredMove *curr, *last, *last_bad_cap;
			std::array<ScoredMove, MAX_MOVES> moves;
			Move prev_move, tt_move;
		};
	
		template<HashFlag HF>
		void MovePicker::update_history(Move best_move, int depth) {

			assert(!move_capture(best_move) || move_promotion_type(best_move));

			update_history_entry(best_move, pos.side, history_bonus[depth]);

			for (const auto& sm : std::ranges::subrange(last_bad_cap, HF == HASH_EXACT ? last : curr)) {
				assert(!move_capture(sm) || move_promotion_type(sm));
				if (sm != best_move)
					update_history_entry(sm, pos.side, -history_bonus[depth]);
			}
		}

	} // namespace MovePick

} // namespace Clovis
