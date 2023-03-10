#include "iq.h"

namespace Clovis {

	namespace IQ {

		void iq_test() 
		{
			vector<IQPosition> iq;
			string file_name = "src/iqtest.epd";
			ifstream ifs;
			ifs.open(file_name.c_str(), ifstream::in);
			string line, token;

			while (true)
			{
				if (ifs.eof())
					break;
				
				getline(ifs, line);
				
				if (line.length())
				{
					size_t idx = line.find(",");
					size_t idx_end = line.find(",", idx + 1);
					string fen = line.substr(0, idx);
					istringstream is(line.substr(idx + 1, idx_end - idx - 1).c_str());
					vector<Move> moves;

					Position pos = Position(fen.c_str());

					while (is >> token)
					{
						moves.push_back(Parse::parse(pos, token));
					}

					iq.push_back(IQPosition(fen, moves));
				}
			}
			
			ifs.close();

			Search::SearchLimits limits;
			limits.time[WHITE] = 100000;
			limits.time[BLACK] = 100000;
			int passes = 0, fails = 0;
			
			for (auto& it : iq)
			{
				Position pos(it.fen.c_str());
				Move best_move, ponder_move;
				int score;
				U64 nodes;
				Search::start_search(pos, limits, best_move, ponder_move, score, nodes);
				bool res = false;
				
				for (auto& move : it.moves)
				{
					if (best_move == move)
					{
						res = true;
						break;
					}
				}
				
				if (res)
				{
					cout << "PASS!" << endl;
					++passes;
				}
				else	
				{
					cout << "FAIL!" << endl;
					++fails;
				}
				
				cout << passes << " tests passed!" << endl
				     << fails  << " tests failed!" << endl;

				Search::clear();
			}
		}

	}

} // namespace Clovis
