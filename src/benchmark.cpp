#include "benchmark.h"

#include <chrono>
#include <fstream>
#include <vector>

#include "search.h"

namespace clovis::bench {

void benchmark(std::vector<std::string>& args)
{
    std::vector<std::string> bm;
    std::ifstream ifs("../src/bench.csv");
    std::string line;

    while (std::getline(ifs, line))
    {
        if (line.empty()) { continue; }
        const size_t idx = line.find('\"');
        const size_t idx_end = line.find('\"', idx + 1);
        bm.push_back(line.substr(idx + 1, idx_end - idx - 1));
    }

    ifs.close();

    const int depth = args.size() > 2 ? stoi(args[2]) : DEFAULT_BENCH_DEPTH;
    const int mb = args.size() > 4 ? stoi(args[4]) : DEFAULT_BENCH_MB;

    tt.resize(mb);

    search::SearchLimits limits;
    limits.depth = depth;

    uint64_t total_nodes = 0ULL;
    Duration total_time = 0LL;

    for (const auto& it : bm)
    {
        auto start_time = std::chrono::steady_clock::now();
        Position pos(it.c_str());
        search::SearchInfo info;
        search::start_search(pos, limits, info);
        total_time += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
        total_nodes += info.nodes;
        search::clear();
    }

    std::cout << "bench: " << total_nodes << " nps: " << 1000ULL * total_nodes / (total_time + 1) << " time: " << total_time << " ms" << '\n';
}

} // namespace clovis::bench
