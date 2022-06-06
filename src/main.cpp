#include <iostream>

#include "bitboard.h"
#include "movelist.h"
#include "position.h"

#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "

using namespace Clovis;

int main(int argc, char* argv[]) 
{
	init_bitboards();

	Position pos;
	pos.set(start_position);
	pos.print_position();
	pos.print_attacked_squares(WHITE);
	pos.print_attacked_squares(BLACK);
	pos.print_bitboards();

	MoveList ml = MoveList(pos);
	ml.print();

	return 0;
}