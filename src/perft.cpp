#include "perft.h"

using namespace std;

namespace Clovis {

	namespace Perft {

		void perft(Position& pos, int depth, U64& nodes) 
		{
			if (depth == 0) 
			{
				++nodes;
				return;
			}

			//for (const auto& m : MoveGen::MoveList(pos)) 
			//{
			//	if (!pos.do_move(m))
			//		continue;
			//	perft(pos, depth - 1, nodes);
			//	pos.undo_move(m);
			//}

			MovePick::MovePicker mp(pos, 0, MOVE_NONE, MOVE_NONE);

			Move m;

			while ((m = mp.get_next(false)) != MOVE_NONE)
			{
				if (!pos.do_move(m))
					continue;
				perft(pos, depth - 1, nodes);
				pos.undo_move(m);
			}
		}

		void test_perft() 
		{
			cout << "Running perft tests..." << endl;

			vector<perft_position> pp;

			pp.push_back(perft_position(
				"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", 
				{ 20, 400, 8902, 197281, 4865609, 119060324 }));
			pp.push_back(perft_position(
				"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 
				{ 14, 191, 2812, 43238, 674624, 11030083 }));
			pp.push_back(perft_position(
				"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ", 
				{ 48, 2039, 97862, 4085603, 193690690, 8031647685 }));

			U64 nodes;

			for (auto& it : pp) 
			{
				Position pos(it.s.c_str());
				cout << "testing position " << it.s << endl;
				for (int depth = 1; depth < it.nodes.size() + 1; ++depth) 
				{
					nodes = 0;
					perft(pos, depth, nodes);
					cout << "depth " << depth
						<< ": perft test " << ((nodes == it.nodes[depth - 1]) ? "PASS! " : "FAIL! ") 
						<< "expected: " << it.nodes[depth - 1] << " result : " << nodes << endl;
				}
			}

			cout << "Perft tests complete!" << endl;
		}

	} // namespace Perft

} // namespace Clovis
