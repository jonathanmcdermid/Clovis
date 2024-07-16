#include "perft.h"

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace clovis::perft {

void perft_helper(Position& pos, const size_t depth, uint64_t& nodes)
{
    if (depth == 0)
    {
        ++nodes;
        return;
    }

    for (const auto& m : move_gen::MoveList(pos))
    {
        if (!pos.do_move(m)) { continue; }
        perft_helper(pos, depth - 1, nodes);
        pos.undo_move(m);
    }
}

void perft()
{
    std::vector<PerftPosition> pp;
    std::ifstream ifs("src/perft.epd");
    std::string line;
    std::string token;

    while (getline(ifs, line))
    {
        if (line.empty()) { continue; }

        size_t idx = line.find(',');
        std::string fen = line.substr(0, idx);
        std::istringstream is(line.substr(idx + 1).c_str());
        std::vector<uint64_t> nv;

        while (is >> token)
        {
            is >> token; // consume depth token
            nv.push_back(stoull(token));
        }

        pp.push_back({fen, nv});
    }

    ifs.close();
    bool failed = false;

    for (auto& [fen, nodes] : pp)
    {
        auto start_time = std::chrono::steady_clock::now();
        Position pos(fen.c_str());
        std::cout << "testing position " << fen << '\n';

        for (size_t depth = 1; depth - 1 < nodes.size(); ++depth)
        {
            uint64_t result_nodes = 0;
            perft_helper(pos, depth, result_nodes);
            failed = (result_nodes != nodes[depth - 1]);
            std::cout << (failed ? " FAIL! " : " PASS! ") << "depth: " << depth << "expected: " << std::setw(10) << nodes[depth - 1]
                      << " result: " << std::setw(10) << result_nodes << " time:" << std::setw(7)
                      << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() << '\n';
        }
    }

    std::cout << "Done!" << '\n' << (failed ? "Some tests failed." : "All tests passed!") << '\n';
}

} // namespace clovis::perft
