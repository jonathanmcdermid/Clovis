#pragma once

#include <fstream>

#include "search.h"

namespace Clovis {
	
	namespace Bench {

		struct BenchMark {
			BenchMark(string f) : fen(f), time(0LL) {;}
			string fen;
			TimePoint time;
			Search::SearchInfo info;
		};
		
		TimePoint benchmark(int argc, char* argv[]);

	} // namespace Bench

} // namespace Clovis
