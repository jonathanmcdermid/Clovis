#pragma once

#include <iostream>

#include "types.h"
#include "position.h"
#include "bitboard.h"

namespace Clovis {

	struct Position;

	struct ScoredMove {
		constexpr ScoredMove() : move(MOVE_NONE), score(0) {}
		void operator=(Move m) { this->move = m; }
		operator Move() const { return move; }
		Move move;
		int score;
	};

	namespace MoveGen {

		template<typename T, MoveType M> T* generate(const Position& pos, T* moves);
		template<typename T> void print_moves(T* m, T* end);

		struct MoveList {
		public:
			MoveList(const Position& pos) : last(generate<Move, ALL_MOVES>(pos, moves.data())) {}
			int size() { return last - moves.data(); }
			auto begin() const { return moves.data(); }
			auto end() const { return last; }
			void remove_illegal(Position pos) {
				last = std::remove_if(moves.data(), last, [&pos](const Move& move)
					{ return pos.do_move(move) ? (void)pos.undo_move(move), false : true; });
			}
		private:
			std::array<Move, MAX_MOVES> moves;
			Move* last;
		};

	} // namespace MoveGen

} // namsepace Clovis
