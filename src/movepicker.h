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
			*history_entry += (bonus << 5) - (*history_entry * abs(bonus) >> 9);
		}

		inline void update_counter_entry(Colour c, Move prev, Move curr) {
			counter_table[cft_index(c, prev)] = curr;
		}

		constexpr bool is_killer(Move m, int ply) {
			Move* k = &killers[ply << 1];
			return (m == k[0] || m == k[1]);
		}

		void init_movepicker();

		class MovePicker {
		public:
			MovePicker(const Position& pos, int ply, Move prev_move, Move tt_move) : pos(pos), ply(ply), tt_move(tt_move), prev_move(prev_move) {
				curr = last = end_bad_caps = moves;
				stage = TT_MOVE;
			}
			template<HashFlag HF> void update_history(Move best_move, int depth);
			template<bool PLAY_QUIETS> Move get_next();
			int get_stage() const { return stage; }
			void print();
		private:
			void score_captures();
			void score_quiets();
			const Position& pos;
			int ply;
			ScoredMove* curr, *last, *end_bad_caps;
			ScoredMove moves[MAX_MOVES];
			Move tt_move, prev_move;
			int stage;
		};

		// return the next ordered move
		template<bool PLAY_QUIETS>
		Move MovePicker::get_next()
		{
			switch (stage)
			{
			case TT_MOVE:
				++stage;
				if (tt_move != MOVE_NONE && (PLAY_QUIETS || move_capture(tt_move)))
					return tt_move;
			case INIT_CAPTURES:
				curr = end_bad_caps = moves;
				last = MoveGen::generate<ScoredMove, CAPTURE_MOVES>(pos, moves);
				score_captures();
				sort(moves, last, sm_score_comp);
				++stage;
			case WINNING_CAPTURES:
				while (curr < last)
				{
					assert(move_capture(*curr));
					if (curr->move == tt_move)
						++curr;
					else if (pos.see(*curr) >= 0)
						return *curr++;
					else
						*end_bad_caps++ = *curr++;
				}
				++stage;
			case INIT_QUIETS:
				if (PLAY_QUIETS)
				{
					curr = end_bad_caps;
					last = MoveGen::generate<ScoredMove, QUIET_MOVES>(pos, curr);
					score_quiets();
					sort(end_bad_caps, last, sm_score_comp);
				}
				++stage;
			case QUIETS:
				while (PLAY_QUIETS && curr < last)
				{
					assert(!move_capture(*curr));
					if (*curr != tt_move)
						return *curr++;
					++curr;
				}
				curr = moves;
				++stage;
			case LOSING_CAPTURES:
				while (curr < end_bad_caps)
				{
					assert(move_capture(*curr));
					if (*curr != tt_move)
						return *curr++;
					++curr;
				}
				++stage;
				break;
			default:
				break;
			}

			return MOVE_NONE;
		}

		template<HashFlag HF>
		void MovePicker::update_history(Move best_move, int depth)
		{
			ScoredMove* last_searched_quiet = HF == HASH_EXACT ? last : curr;

			assert(!move_capture(best_move));

			update_history_entry(best_move, pos.side, history_bonus[depth]);

			for (ScoredMove* sm = end_bad_caps; sm < last_searched_quiet; ++sm)
			{
				assert(!move_capture(*sm));
				if (*sm != best_move)
					update_history_entry(*sm, pos.side, -history_bonus[depth]);
			}
		}

	} // namespace MovePick

} // namespace Clovis
