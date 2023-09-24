#include "iq.h"

#include <fstream>
#include <sstream>

namespace clovis::iq {

    void iq_test() {
        std::vector<IQPosition> iq;
        std::ifstream           ifs("src/iq_test.epd");
        std::string             line, token;

        while (std::getline(ifs, line)) {
            if (line.empty()) continue;

            size_t      idx = line.find(','), idx_end = line.find(',', idx + 1);
            std::string fen = line.substr(0, idx);
            std::istringstream is(
                line.substr(idx + 1, idx_end - idx - 1).c_str());
            std::vector<Move> moves;
            Position          pos(fen.c_str());

            while (is >> token) moves.push_back(parse::parse(pos, token));

            iq.emplace_back(fen, moves);
        }

        ifs.close();

        search::SearchLimits limits;
        limits.time = {100000, 100000};
        int passes = 0, fails = 0;

        for (const auto &it : iq) {
            Position pos(it.fen.c_str());
            pos.print_position();
            search::SearchInfo info;
            search::start_search(pos, limits, info);

            if (std::ranges::find(it.moves.begin(), it.moves.end(),
                                  info.pv_line.moves[0]) != it.moves.end()) {
                std::cout << "PASS!" << std::endl;
                ++passes;
            } else {
                std::cout << "FAIL! best move: ";
                for (const auto &move : it.moves) std::cout << move << " ";
                std::cout << std::endl;
                ++fails;
            }

            std::cout << passes << " tests passed!" << std::endl
                      << fails << " tests failed!" << std::endl;

            search::clear();
        }
    }

} // namespace clovis::iq
