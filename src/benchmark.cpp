#include "benchmark.h"

using namespace std;

namespace Clovis {
	
	namespace Bench {
		
		long long benchmark(int argc, char* argv[]) {

			vector<BenchMark> bm;

			ifstream ifs("src/bench.csv");

			string line;

			while (getline(ifs, line)) {
				if (line.empty()) continue;
				const size_t idx = line.find("\"");
				const size_t idx_end = line.find("\"", idx + 1);
				bm.push_back(BenchMark(line.substr(idx + 1, idx_end - idx - 1)));
			}

			ifs.close();
		
			const int depth = argc > 2 ? atoi(argv[2]) : DEFAULT_BENCH_DEPTH;
			const int mb    = argc > 4 ? atoi(argv[4]) : DEFAULT_BENCH_MB;

			tt.resize(mb);
			
			Search::SearchLimits limits;

			limits.depth = depth;

			U64 total_nodes = 0ULL;
			long long total_time = 0LL;

			for (auto& it : bm) {
				auto start_time = chrono::steady_clock::now();
				Position pos(it.fen.c_str());
				Search::start_search(pos, limits, it.info);
				it.time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count();
				total_nodes += it.info.nodes;
				total_time += it.time;
				Search::clear();
			}
			
			for (const auto& it : bm) {
				cout << "score cp: " << setw(4) << it.info.score
				     << " best: "    << setw(4) << it.info.pline.moves[0] 
				     << " ponder: "  << setw(4) << it.info.pline.moves[1]
				     << " nodes: "   << setw(7) << it.info.nodes
				     << " nps: "     << setw(6) << 1000ULL * it.info.nodes / (it.time + 1) << endl;
			}

			cout << "bench: " << total_nodes 
			     << " nps: "  << 1000ULL * total_nodes / (total_time + 1) 
			     << " time: " << total_time 
			     << " ms"     << endl;

			return total_time;
		}

	} // namespace Bench

} // namespace Clovis
