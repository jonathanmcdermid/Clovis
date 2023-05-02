#include "iq.h"

using namespace std;

namespace Clovis {

	namespace IQ {

		void iq_test() {

			vector<IQPosition> iq;
			ifstream ifs("src/iqtest.epd");
			string line, token;

			while (getline(ifs, line)) {
				
				if (line.length()) {
					size_t idx = line.find(",");
					size_t idx_end = line.find(",", idx + 1);
					string fen = line.substr(0, idx);
					istringstream is(line.substr(idx + 1, idx_end - idx - 1).c_str());
					vector<Move> moves;

					Position pos(fen.c_str());

					while (is >> token)
						moves.push_back(Parse::parse(pos, token));

					iq.push_back(IQPosition(fen, moves));
				}
			}
			
			ifs.close();

			Search::SearchLimits limits;
			limits.time[WHITE] = 100000;
			limits.time[BLACK] = 100000;
			int passes = 0, fails = 0;
			
			for (const auto& it : iq) {

				Position pos(it.fen.c_str());
				pos.print_position();
				Search::SearchInfo info;
				Search::start_search(pos, limits, info);
				bool res = false;
				
				for (const auto& move : it.moves) {
					if (info.pline.moves[0] == move) {
						res = true;
						break;
					}
				}
				
				if (res) {
					cout << "PASS!" << endl;
					++passes;
				} else {
					cout << "FAIL! best move: ";
					for (const auto& move : it.moves)
						cout << move << " ";
					cout << endl;
					++fails;
				}
				
				cout << passes << " tests passed!" << endl
				     << fails  << " tests failed!" << endl;

				Search::clear();
			}
		}

	}

} // namespace Clovis
