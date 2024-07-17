#include "clovis/engine/bitboard.hpp"
#include "uci.hpp"

int main(const int argc, char* argv[])
{
    clovis::bitboards::init_bitboards();

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i) { args.emplace_back(argv[i]); }

    if (argc < 2) { clovis::uci::loop(args); }
    else { return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}
