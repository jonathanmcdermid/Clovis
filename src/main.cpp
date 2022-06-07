#include <iostream>

#include "bitboard.h"
#include "movelist.h"
#include "position.h"
#include "time.h"

#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define hard_position "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "

#define start_pos_nodes {20, 400, 8902, 197281, 4865609, 119060324}
#define hard_pos_nodes {14, 191, 2812, 43238, 674624, 11030083}
#define tricky_pos_nodes {48, 2039, 97862, 4085603, 193690690, 8031647685}

const std::string perftFen[3] = { hard_position, start_position, tricky_position };

const unsigned long long pfTable[3][6] = { hard_pos_nodes, start_pos_nodes, tricky_pos_nodes };

unsigned long long nodes;

using namespace Clovis;

Position p;

static inline void perft(int depth) {
	if (depth == 0) {
		++nodes;
		return;
	}

	MoveList ml = MoveList(p);

	for (const Move* m = ml.start(); m != ml.end(); ++m) {
		if (!p.do_move(*m))
			continue;
		perft(depth - 1);
		p.undo_move(*m);
	}
}

static inline void perft_control() {
	for (int i = 0; i < 3; ++i) {
		p.set(perftFen[i].c_str());
		TimeManager tm;
		tm.init();
		for (int depth = 1; depth < 7; ++depth) {
			nodes = 0;
			perft(depth);
			std::cout << "info depth " << depth << " nodes " << nodes << " time " << tm.get_time_elapsed() << std::endl;
			//assert(nodes == pfTable[i][depth - 1]);
		}
	}
}

int main(int argc, char* argv[]) 
{
	init_bitboards();

	perft_control();

	return 0;
}