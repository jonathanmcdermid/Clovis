#pragma once

#include <fstream>

#include "search.h"

namespace Clovis {
	
	namespace Bench {

		struct BenchMark {
			BenchMark(string fen, int score, TimePoint time, U64 nodes, Move best_move, Move ponder_move)
				: fen(fen), score(score), time(time), nodes(nodes), best_move(best_move), ponder_move(ponder_move) {;}
			string fen;
			int score;
			TimePoint time;
			U64 nodes;
			Move best_move;
			Move ponder_move;
		};
		
		void benchmark(int argc, char* argv[]);

	} // Bench

} // Clovis
