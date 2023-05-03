#include "cmdline.h"

using namespace std;

namespace Clovis {

	namespace CMDLine {
	
		// handle any start-up command line arguments
		void handle_cmd(int argc, char* argv[]) {
			
			if (argc < 2) {
				UCI::loop(argc, argv);
				exit(EXIT_SUCCESS);
			}

			if (!strcmp(argv[1], "--help")) {
				cout << "bench [depth=" << DEFAULT_BENCH_DEPTH 
					<< "] [threads="    << DEFAULT_BENCH_THREADS 
					<< "] [hash="       << DEFAULT_BENCH_MB  << "]"  << endl
					<< "search positions as speficied by OpenBench"  << endl;
				exit(EXIT_SUCCESS);
			}

			if (!strcmp(argv[1], "bench")) {
				Bench::benchmark(argc, argv);
				exit(EXIT_SUCCESS);
			}

			if (!strcmp(argv[1], "perft")) {
				Perft::perft();
				exit(EXIT_SUCCESS);
			}

			if (!strcmp(argv[1], "iq")) {
				IQ::iq_test();
				exit(EXIT_SUCCESS);
			}

			if (!strcmp(argv[1], "tune")) {
				Tuner::tune_eval();
				exit(EXIT_SUCCESS);
			}
			
			if (!strcmp(argv[1], "generate")) {
				Parse::generate_dataset();
				exit(EXIT_SUCCESS);
			}

			exit(EXIT_FAILURE);
		}
		
	} // namespace Bench

} // namespace Clovis
