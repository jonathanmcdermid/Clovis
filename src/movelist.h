#pragma once

#include "types.h"
#include "position.h"
#include "bitboard.h"

namespace Clovis {

	struct MoveList {
		MoveList(const Position& pos);
		void print();
		const Move* start() const { return moveList; }
		const Move* end() const { return last; }
	private:
		Move moveList[MAX_MOVES] = { MOVE_NONE }, * last = moveList;
	};

}