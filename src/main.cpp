#include "benchmark.h"
#include "bitboard.h"
#include "iq.h"
#include "perft.h"
#include "tuner.h"
#include "uci.h"

int main(const int argc, char* argv[])
{
    clovis::bitboards::init_bitboards();

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i) { args.emplace_back(argv[i]); }

    const std::string_view arg = argc > 1 ? argv[1] : "";

    if (argc < 2) { clovis::uci::loop(args); }
    else if (arg == "bench") { clovis::bench::benchmark(args); }
    else if (arg == "perft") { clovis::perft::perft(); }
    else if (arg == "iq") { clovis::iq::iq_test(); }
    else if (arg == "tune") { clovis::tuner::tune_eval(); }
    else if (arg == "gen") { clovis::parse::generate_data(); }
    else if (arg == "--help")
    {
        std::cout << "bench [depth=" << clovis::DEFAULT_BENCH_DEPTH << "] [threads=" << clovis::DEFAULT_BENCH_THREADS
                  << "] [hash=" << clovis::DEFAULT_BENCH_MB << "]" << '\n'
                  << "search positions as specified by OpenBench" << '\n';
    }
    else { return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}
