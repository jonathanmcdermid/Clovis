#include "perft.h"

namespace Clovis {

	namespace Perft {

		const std::string perftFen[3] = { 
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ",
			"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ",
			"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
		};

		const unsigned long long pfTable[3][6] = {
			{20, 400, 8902, 197281, 4865609, 119060324},
			{14, 191, 2812, 43238, 674624, 11030083},
			{48, 2039, 97862, 4085603, 193690690, 8031647685}
		};

		Position pos;

		unsigned long long nodes;

		void perft(int depth) 
		{
			if (depth == 0) 
			{
				++nodes;
				return;
			}

			for (const auto& sm : MoveGen::MoveList(pos)) 
			{
				if (!pos.do_move(sm.m))
					continue;
				perft(depth - 1);
				pos.undo_move(sm.m);
			}
		}

		void perft_control() 
		{
			for (int i = 0; i < 3; ++i) 
			{
				pos.set(perftFen[i].c_str());
				TimeManager tm;
				tm.set();
				for (int depth = 1; depth < 7; ++depth) 
				{
					nodes = 0;
					perft(depth);
					std::cout << "info depth " << depth << " nodes " << nodes << " time " << tm.get_time_elapsed() << std::endl;
					assert(nodes == pfTable[i][depth - 1]);
				}
			}
		}

	} // namespace Perft

} // namespace Clovis
