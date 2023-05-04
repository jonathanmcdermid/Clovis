#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>

#include "perft.h"

using namespace std;

namespace Clovis {

	namespace Perft {

		void perft_helper(Position& pos, const int depth, U64& nodes) {

			if (depth == 0) {
				++nodes;
				return;
			}

			for (const auto& m : MoveGen::MoveList(pos)) {
				if (!pos.do_move(m)) continue;
				perft_helper(pos, depth - 1, nodes);
				pos.undo_move(m);
			}
		}

		void perft() {

			vector<PerftPosition> pp;
			ifstream ifs("src/perft.epd");
			string line, token;

			while (getline(ifs, line)) {

				if (line.empty()) continue;

				size_t idx = line.find(',');
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

				pp.push_back({ fen, nv });
			}

			ifs.close();

			bool failed = false;

			for (auto& [fen, nodes] : pp) {

				auto start_time = chrono::steady_clock::now();
				Position pos(fen.c_str());
				cout << "testing position " << fen << endl;

				for (size_t depth = 1; depth - 1 < nodes.size(); ++depth) {

					U64 result_nodes = 0;
					perft_helper(pos, depth, result_nodes);

					failed = (result_nodes != nodes[depth - 1]);
					cout << (failed ? " FAIL! " : " PASS! ")
						<< "depth: "    << depth
						<< "expected: " << setw(10) << nodes[depth - 1]
						<< " result: "  << setw(10) << result_nodes
						<< " time:"     << setw(7)
						<< chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start_time).count() << endl;
				}
			}

			cout << "Done!" << endl << (failed ? "Some tests failed." : "All tests passed!") << endl;
		}

	} // namespace Perft

} // namespace Clovis
