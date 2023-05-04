#include "cmdline.h"

using namespace Clovis;

int main(const int argc, char* argv[]) {

	Bitboards::init_bitboards();

	CMDLine::handle_cmd(argc, argv);

	return EXIT_SUCCESS;
}
