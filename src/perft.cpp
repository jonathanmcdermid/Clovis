#include "perft.h"

using namespace std;

namespace Clovis {

	namespace Perft {

		void perft(Position& pos, int depth, U64& nodes) {

			if (depth == 0) {
				++nodes;
				return;
			}

			for (const auto& m : MoveGen::MoveList(pos)) {
				if (pos.do_move(m)) continue;
				perft(pos, depth - 1, nodes);
				pos.undo_move(m);
			}
		}

		void perft_control() {

			vector<PerftPosition> pp;
			ifstream ifs("src/perft.epd");
			string line, token;

			while (getline(ifs, line)) {

				if (line.empty()) continue;

				size_t idx = line.find(",");
				string fen = line.substr(0, idx);
				istringstream is(line.substr(idx + 1).c_str());
				int depth = 1;
				vector<U64> nv;

				while (is >> token) {

					assert(token.length() == 2);
					assert(token[1] - '0' == depth);
					is >> token;
					nv.push_back(U64(stoull(token)));
					++depth;
				}

				pp.push_back(PerftPosition(fen, nv));
			}

			ifs.close();

			U64 nodes;
			bool failed = false;

			for (auto& it : pp) {

				tm.set();
				Position pos(it.fen.c_str());
				cout << "testing position " << it.fen << endl;

				for (size_t depth = 1; depth - 1 < it.nodes.size(); ++depth) {

					nodes = 0;
					perft(pos, depth, nodes);

					if (nodes != it.nodes[depth - 1])
						failed = true;
					
					cout << (failed ? " FAIL! " : " PASS! ") 
						 << "depth: "    << depth
						 << "expected: " << setw(10) << it.nodes[depth - 1] 
						 << " result: "  << setw(10) << nodes
						 << " time:"     << setw(7)  << tm.get_time_elapsed() << endl;
				}
			}

			cout << "Done!" << endl << (failed ? "Some tests failed." : "All tests passed!") << endl;
		}

	} // namespace Perft

} // namespace Clovis
