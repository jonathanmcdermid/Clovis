#include "cmdline.h"
#include "bitboard.h"

int main(const int argc, char* argv[]) {

	clovis::bitboards::init_bitboards();

	clovis::cmdline::handle_cmd(argc, argv);

	return EXIT_SUCCESS;
}
