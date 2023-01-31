#pragma once

#include <iostream>

#include "types.h"
#include "position.h"
#include "bitboard.h"

namespace Clovis {

	struct Position;

	struct ScoredMove {
		Move move = MOVE_NONE;
		int score = 0;
		void operator=(Move m) { this->move = m; }
		operator Move() const { return move; }
	};

	constexpr bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
		return lhs.score > rhs.score;
	}

	namespace MoveGen {

		template<typename T, MoveType M> T* generate(const Position& pos, T* moves);

		struct MoveList {
			MoveList(const Position& pos) : last(generate<Move, ALL_MOVES>(pos, moves)) {}
			int size() const { return (last - moves); }
			const Move* begin() const { return moves; }
			const Move* end()   const { return last;  }
		private:
			Move moves[MAX_MOVES], *last;
		};

	} // namespace MoveGen

} // namsepace Clovis
