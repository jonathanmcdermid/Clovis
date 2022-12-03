#pragma once

#include "uci.h"

using namespace std;

namespace Clovis {

	namespace CMDLine {

		struct BenchMark {
			BenchMark(string fen, int score, TimePoint time, U64 nodes, Move best_move, Move ponder_move)
				: fen(fen), score(score), time(time), nodes(nodes), best_move(best_move), ponder_move(ponder_move) {;}
			string fen;
			int score;
			TimePoint time;
			U64 nodes;
			Move best_move;
			Move ponder_move;
		};

		void handle_cmd(int argc, char* argv[]);
		void benchmark(int argc, char* argv[]);

	} // Bench

} // Clovis
