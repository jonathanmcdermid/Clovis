#include "UCI.h"
#include "perft.h"
#include "evaluate.h"
#include <iostream>

using namespace Clovis;

int main(int argc, char* argv[]) 
{
	Bitboards::init_bitboards();
	Search::init_search();
	Eval::init_eval();

	Position pos;
	pos.set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	std::cout<<Eval::evaluate(pos);

	UCI::loop(argc, argv);

	return 0;
}