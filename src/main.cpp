#define NDEBUG

#include "UCI.h"

using namespace Clovis;

int main(int argc, char* argv[]) 
{
	Bitboards::init_bitboards();
	Search::init_search();
	MovePick::init_movepicker();
	Eval::init_eval();
	Position::init_position();

	UCI::loop(argc, argv);

	return 0;
}