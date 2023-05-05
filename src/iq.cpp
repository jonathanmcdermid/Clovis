#include <fstream>
#include <sstream>

#include "iq.h"

using namespace std;

namespace clovis {

	namespace iq {

		void iq_test() {

			vector<IQPosition> iq;
			ifstream ifs("src/iqtest.epd");
			string line, token;

			while (getline(ifs, line)) {

				if (line.empty()) continue;

				size_t idx = line.find(','), idx_end = line.find(',', idx + 1);
				string fen = line.substr(0, idx);
				istringstream is(line.substr(idx + 1, idx_end - idx - 1).c_str());
				vector<Move> moves;
				Position pos(fen.c_str());

				while (is >> token)
					moves.push_back(parse::parse(pos, token));

				iq.emplace_back(fen, moves);
			}
			
			ifs.close();

			search::SearchLimits limits;
			limits.time = { 100000, 100000 };
			int passes = 0, fails = 0;
			
			for (const auto& it : iq) {

				Position pos(it.fen.c_str());
				pos.print_position();
				search::SearchInfo info;
				search::start_search(pos, limits, info);

				if (ranges::find(it.moves.begin(), it.moves.end(), info.pline.moves[0]) != it.moves.end()) {
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

				search::clear();
			}
		}

	} // namespace iq

} // namespace clovis
