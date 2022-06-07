#include "UCI.h"
#include "perft.h"

using namespace Clovis;

int main(int argc, char* argv[]) 
{
	Bitboards::init_bitboards();
	Search::init_search();

	UCI::loop(argc, argv);

	return 0;
}