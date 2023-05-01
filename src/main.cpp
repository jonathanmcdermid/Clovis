#include "cmdline.h"

using namespace Clovis;

int main(int argc, char* argv[]) {

	Bitboards::init_bitboards();
	Search::init_search();

	CMDLine::handle_cmd(argc, argv);

	return EXIT_SUCCESS;
}
