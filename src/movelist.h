#pragma once

#include <iostream>

#include "types.h"
#include "position.h"
#include "bitboard.h"

namespace Clovis {

	class Position;

	struct ScoredMove {
		Move m = MOVE_NONE;
		int score = 0;
		void operator=(Move m) { this->m = m; }
		bool operator==(Move m) { return this->m == m; }
		bool operator!=(Move m) { return this->m != m; }
	};

	inline static bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
		return lhs.score > rhs.score;
	}

	ScoredMove* gen_moves(const Position& pos, ScoredMove* ml);

	namespace MoveGen {

		struct MoveList {
			MoveList(const Position& pos) : last(gen_moves(pos, moves)) {}
			int size() const { return (last - moves); }
			void print();
			const ScoredMove* begin() const { return moves; }
			const ScoredMove* end() const { return last; }
		private:
			ScoredMove moves[MAX_MOVES], *last;
		};

	} // namespace MoveGen

} // namsepace Clovis