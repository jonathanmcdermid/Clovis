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
			SearchLimits() : time{ 5000 }, inc{ 0 }, move_time(0), start_time(0), 
				moves_left(20), depth(0), mate(0), perft(0), infinite(false), nodes(0ULL) {}
			int time[COLOUR_N], inc[COLOUR_N], move_time, start_time;
			int moves_left, depth, mate, perft;
			bool infinite;
			U64 nodes;
		};

		struct Line {
			Line() : moves{ MOVE_NONE }, last(moves) {}
			int move_count()    const { return last - moves; }
			const Move* begin() const { return moves; }
			const Move* end()   const { return last; }
			Move moves[MAX_PLY], *last;
		};

		struct SearchInfo {
			SearchInfo() : score(0), nodes(0ULL) {}
			int score;
			U64 nodes;
			Line pline;
		};

		void start_search(Position& pos, SearchLimits& limits, SearchInfo& params);
		void init_search();
		void init_values();
		void clear();

	} // namespace Search

} // namespace Clovis
