#include "cmdline.h"

#define DEFAULT_BENCH_MB 16
#define DEFAULT_BENCH_DEPTH 13
#define DEFAULT_BENCH_THREADS 1

using namespace std;

namespace Clovis {

	namespace CMDLine {

		void benchmark(int argc, char* argv[])
		{
			vector<BenchMark> bm;

			string file_name = "src/bench.csv";
			ifstream ifs;
			ifs.open(file_name.c_str(), ifstream::in);
			string line;

			while (true)
			{
				if (ifs.eof())
					break;
				getline(ifs, line);
				if (line.length())
				{
					size_t idx = line.find("\"");
					size_t idx_end = line.find("\"", idx + 1);
					string fen = line.substr(idx + 1, idx_end - idx - 1);
					bm.push_back(BenchMark(fen, 0, 0, 0ULL, MOVE_NONE, MOVE_NONE));				
				}
			}

			ifs.close();

		    U64 total_nodes = 0ULL;
			TimePoint total_time = 0ULL;
		
		    int depth = argc > 2 ? atoi(argv[2]) : DEFAULT_BENCH_DEPTH;
		    int threads = argc > 3 ? atoi(argv[3]) : DEFAULT_BENCH_THREADS;
		    int mb = argc > 4 ? atoi(argv[4]) : DEFAULT_BENCH_MB;

		    tt.resize(mb);
		    tm.set();
			
			Search::SearchLimits limits;

			limits.depth = depth;

			for (auto& it : bm) 
			{
				tm.set();
				Position pos(it.fen.c_str());
				Search::start_search(pos, limits, it.best_move, it.ponder_move, it.score, it.nodes);
				tt.clear();
				it.time = tm.get_time_elapsed();
				total_nodes += it.nodes;
				total_time += it.time;
		    }
		
		    for (auto& it : bm) 
			{
				cout << "score cp: "	<< setw(4) << it.score
					<< " best: "	<< setw(4) << move2str(it.best_move) 
					<< " ponder: "	<< setw(4) << move2str(it.ponder_move)
					<< " nodes: "	<< setw(7) << it.nodes
					<< " nps: "		<< setw(6) << 1000ULL * it.nodes / (it.time + 1) << endl;
		    }

			cout << total_nodes << " nodes "
				<< 1000ULL * total_nodes / (total_time + 1) << " nps" << endl;
		}

		// handle any start-up command line arguments
		void handle_cmd(int argc, char* argv[])
		{
			if (argc < 2)
			{
				UCI::loop(argc, argv);
				exit(EXIT_SUCCESS);
			}

			if (!strcmp(argv[1], "--help")) 
			{
    		    cout << "bench [depth=" << DEFAULT_BENCH_DEPTH 
					<< "] [threads=" << DEFAULT_BENCH_THREADS 
					<< "] [hash=" << DEFAULT_BENCH_MB  << "]" << endl
					<< "search positions as speficied by OpenBench" << endl;
				exit(EXIT_SUCCESS);
    		}

    		if (!strcmp(argv[1], "bench")) 
			{
    		    benchmark(argc, argv);
    		    exit(EXIT_SUCCESS);
    		}

			exit(EXIT_FAILURE);
		}

		
	} // Bench

} // Clovis
