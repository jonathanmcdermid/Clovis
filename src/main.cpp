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

#ifndef NDEBUG
	MovePick::test_movepicker();
	Eval::test_eval();
#endif

	UCI::loop(argc, argv);

	return 0;
}