#pragma once

#include "position.h"

namespace clovis {

	struct Position;

	struct ScoredMove {
		constexpr ScoredMove() = default;
		ScoredMove& operator=(const Move m) { move = m; return *this; }
		operator Move() const { return move; }

		Move move{ MOVE_NONE };
		int score{ 0 };
	};

	namespace move_gen {

		template<typename T, MoveType M> T* generate(const Position& pos, T* moves);
		template<typename T> void print_moves(const T* m, const T* end);

		class MoveList {
		public:
			explicit MoveList(const Position& pos) : moves{}, last(generate<Move, ALL_MOVES>(pos, moves.data())) {}
			[[nodiscard]] int size() const { return static_cast<int>(last - moves.data()); }
			[[nodiscard]] auto begin() const { return moves.data(); }
			[[nodiscard]] auto end() const { return last; }
			void remove_illegal(Position& pos) {
				last = std::remove_if(moves.data(), last, [&pos](const Move& move)
					{ return pos.do_move(move) ? (void)pos.undo_move(move), false : true; });
			}
		private:
			std::array<Move, MAX_MOVES> moves;
			Move* last;
		};

	} // namespace move_gen

} // namespace clovis
