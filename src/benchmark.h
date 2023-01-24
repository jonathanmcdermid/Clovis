#pragma once

#include <fstream>

#include "search.h"

namespace Clovis {
	
	namespace Bench {

		struct BenchMark {
			BenchMark(string f, int s, TimePoint t, U64 n, Move bm, Move pm)
				: fen(f), score(s), time(t), nodes(n), best_move(bm), ponder_move(pm) {;}
			string fen;
			int score;
			TimePoint time;
			U64 nodes;
			Move best_move;
			Move ponder_move;
		};
		
		TimePoint benchmark(int argc, char* argv[]);

	} // namespace Bench

} // namespace Clovis
