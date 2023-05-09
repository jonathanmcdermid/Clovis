#include "bitboard.h"
#include "uci.h"
#include "benchmark.h"
#include "perft.h"
#include "iq.h"
#include "tuner.h"

int main(const int argc, char* argv[]) {

	clovis::bitboards::init_bitboards();

	if (argc < 2)                       clovis::uci::loop(argc, argv);
	else if (!strcmp(argv[1], "bench")) clovis::bench::benchmark(argc, argv);
	else if (!strcmp(argv[1], "perft")) clovis::perft::perft();
	else if (!strcmp(argv[1], "iq"))    clovis::iq::iq_test();
	else if (!strcmp(argv[1], "tune"))  clovis::tuner::tune_eval();
	else if (!strcmp(argv[1], "gen"))   clovis::parse::generate_data();
	else if (!strcmp(argv[1], "--help"))
		std::cout << "bench [depth=" << clovis::DEFAULT_BENCH_DEPTH
		<< "] [threads="             << clovis::DEFAULT_BENCH_THREADS
		<< "] [hash="                << clovis::DEFAULT_BENCH_MB << "]" << std::endl
		<< "search positions as specified by OpenBench" << std::endl;
	else return EXIT_FAILURE;

	return EXIT_SUCCESS;
}
