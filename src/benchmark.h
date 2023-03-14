#pragma once

#include <fstream>

#include "search.h"

namespace Clovis {
	
	namespace Bench {

		struct BenchMark {
			BenchMark(string f) : fen(f) {;}
			string fen;
			int score;
			TimePoint time;
			U64 nodes;
			Search::Line pline;
		};
		
		TimePoint benchmark(int argc, char* argv[]);

	} // namespace Bench

} // namespace Clovis
