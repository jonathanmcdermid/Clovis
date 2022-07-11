#include "UCI.h"

using namespace Clovis;

int main(int argc, char* argv[]) 
{

	Bitboards::init_bitboards();
	Search::init_search();
	Eval::init_eval();
	Position::init();

	UCI::loop(argc, argv);

	return 0;
}