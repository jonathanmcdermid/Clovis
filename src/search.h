#pragma once

#include <math.h>

#include "uci.h"
#include "position.h"
#include "evaluate.h"
#include "movelist.h"
#include "movepicker.h"
#include "time.h"
#include "tt.h"

namespace Clovis {

	namespace Search {

		struct SearchLimits {
			SearchLimits() {
				time[WHITE] = time[BLACK] = 5000;
				inc[WHITE] = inc[BLACK] = move_time = start_time = 0;
				moves_left = 20;
				depth = mate = perft = 0;
				infinite = false;
				nodes = 0ULL;
			}
			int time[COLOUR_N], inc[COLOUR_N], move_time, start_time;
			int moves_left, depth, mate, perft;
			bool infinite;
			U64 nodes;
		};

		struct Line {
			Line() : last(moves) {}
			int move_count() const { return last - moves; }
			const Move* begin() const { return moves; }
			const Move* end() const { return last; }
			Move moves[MAX_PLY] = { MOVE_NONE }, *last;
		};

		void start_search(Position& pos, SearchLimits& lim, int& score, U64& nodes, Line& pline);
		void init_search();
		void init_values();
		void clear();

		extern SearchLimits limits;

	} // namespace Search

} // namespace Clovis
