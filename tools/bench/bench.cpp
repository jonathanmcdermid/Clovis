#include <chrono>
#include <fstream>
#include <vector>

#include "clovis/engine/search.hpp"

constexpr int DEFAULT_BENCH_DEPTH = 13;
// constexpr int DEFAULT_BENCH_THREADS = 1;
constexpr int DEFAULT_BENCH_MB = 16;

int main(const int argc, char* argv[])
{
    clovis::bitboards::init_bitboards();

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i) { args.emplace_back(argv[i]); }

    std::vector<std::string> bm;
    std::ifstream ifs(args[1]);
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

    clovis::tt.resize(mb);

    clovis::search::SearchLimits limits;
    limits.depth = depth;

    uint64_t total_nodes = 0ULL;
    clovis::Duration total_time = 0LL;

    for (const auto& it : bm)
    {
        auto start_time = std::chrono::steady_clock::now();
        clovis::Position pos(it.c_str());
        clovis::search::SearchInfo info;
        clovis::search::start_search(pos, limits, info);
        total_time += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();
        total_nodes += info.nodes;
        clovis::search::clear();
    }

    std::cout << "bench: " << total_nodes << " nps: " << 1000ULL * total_nodes / (total_time + 1) << " time: " << total_time << " ms" << '\n';

    return EXIT_SUCCESS;
}
