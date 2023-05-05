#include <fstream>
#include <chrono>

#include "benchmark.h"
#include "search.h"

using namespace std;

namespace Clovis {
	
	namespace Bench {
		
		void benchmark(const int argc, char* argv[]) {

			vector<string> bm;
			ifstream ifs("src/bench.csv");
			string line;

			while (getline(ifs, line)) {
				if (line.empty()) continue;
				const size_t idx = line.find('\"');
				const size_t idx_end = line.find('\"', idx + 1);
				bm.push_back(line.substr(idx + 1, idx_end - idx - 1));
			}

			ifs.close();
		
			const int depth = argc > 2 ? atoi(argv[2]) : DEFAULT_BENCH_DEPTH;
			const int mb    = argc > 4 ? atoi(argv[4]) : DEFAULT_BENCH_MB;

			tt.resize(mb);
			
			Search::SearchLimits limits;
			limits.depth = depth;

			uint64_t total_nodes = 0ULL;
			Duration total_time = 0LL;

			for (auto& it : bm) {
				auto start_time = chrono::steady_clock::now();
				Position pos(it.c_str());
				Search::SearchInfo info;
				Search::start_search(pos, limits, info);
				total_time += chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count();
				total_nodes += info.nodes;
				Search::clear();
			}

			cout << "bench: " << total_nodes 
			     << " nps: "  << 1000ULL * total_nodes / (total_time + 1) 
			     << " time: " << total_time 
			     << " ms"     << endl;
		}

	} // namespace Bench

} // namespace Clovis
