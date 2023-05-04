#include "cmdline.h"
#include "bitboard.h"

int main(const int argc, char* argv[]) {

	Clovis::Bitboards::init_bitboards();

	Clovis::CMDLine::handle_cmd(argc, argv);

	return EXIT_SUCCESS;
}
