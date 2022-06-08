#include "UCI.h"
#include "perft.h"
#include "evaluate.h"
#include "movelist.h"
#include <iostream>

using namespace Clovis;

int main(int argc, char* argv[]) 
{
	Bitboards::init_bitboards();
	Search::init_search();
	Eval::init_eval();

	UCI::loop(argc, argv);

	return 0;
}