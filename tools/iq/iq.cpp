#include <fstream>
#include <sstream>
#include <vector>

#include "clovis/engine/search.hpp"

struct IQPosition
{
    std::string fen;
    std::vector<clovis::Move> moves;
};

int main(const int argc, char* argv[])
{
    clovis::bitboards::init_bitboards();

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i) { args.emplace_back(argv[i]); }

    std::vector<IQPosition> iq;
    std::ifstream ifs(args[1]);
    std::string line;
    std::string token;

    while (std::getline(ifs, line))
    {
        if (line.empty()) { continue; }

        size_t idx = line.find(',');
        size_t idx_end = line.find(',', idx + 1);
        std::string fen = line.substr(0, idx);
        std::istringstream is(line.substr(idx + 1, idx_end - idx - 1).c_str());
        std::vector<clovis::Move> moves;
        clovis::Position pos(fen.c_str());

        while (is >> token) { moves.push_back(pos.parse(token)); }

        iq.emplace_back(fen, moves);
    }

    ifs.close();

    clovis::search::SearchLimits limits;
    limits.time = {1000, 1000};
    limits.moves_left = 0;
    int passes = 0;
    int fails = 0;

    for (const auto& it : iq)
    {
        clovis::Position pos(it.fen.c_str());
        pos.print_position();
        clovis::search::SearchInfo info;
        clovis::search::start_search(pos, limits, info);

        if (std::find(it.moves.begin(), it.moves.end(), info.pv_line.moves[0]) != it.moves.end())
        {
            std::cout << "PASS!" << '\n';
            ++passes;
        }
        else
        {
            std::cout << "FAIL! best move: ";
            for (const auto& move : it.moves) { std::cout << move << " "; }
            std::cout << '\n';
            ++fails;
        }

        std::cout << passes << " tests passed!" << '\n' << fails << " tests failed!" << '\n';
        clovis::search::clear();
    }

    return EXIT_SUCCESS;
}
