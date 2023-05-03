#pragma once

#include <fstream>

#include "search.h"

namespace Clovis {
	
	namespace Bench {

		struct BenchMark {
			BenchMark(std::string f) : fen(f), time(0LL) {;}
			std::string fen;
			long long time;
			Search::SearchInfo info;
		};
		
		long long benchmark(int argc, char* argv[]);

	} // namespace Bench

} // namespace Clovis
