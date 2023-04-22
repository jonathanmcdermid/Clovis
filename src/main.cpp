#include "cmdline.h"

using namespace Clovis;

int main(int argc, char* argv[]) {

	Random::seed_random(0ULL);
	Search::init_search();
	Eval::init_eval();
	Zobrist::init_zobrist();

	CMDLine::handle_cmd(argc, argv);

	return EXIT_SUCCESS;
}
