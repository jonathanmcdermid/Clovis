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
			MoveList(const Position& pos) : moves{ MOVE_NONE }, last(generate<Move, ALL_MOVES>(pos, moves)) {}
			int size() const { return (last - moves); }
			const Move* begin() const { return moves; }
			const Move* end()   const { return last;  }
			void remove_illegal(Position pos);
		private:
			Move moves[MAX_MOVES], *last;
		};

	} // namespace MoveGen

} // namsepace Clovis
