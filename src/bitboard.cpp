#include "bitboard.h"

using namespace std;

namespace Clovis {

	namespace Bitboards {

		// prints a bitboard, useful for debugging
		void print_bitboard(const Bitboard& bb) {

			for (Rank r = RANK_8; r >= RANK_1; --r) {
				cout << r + 1 << ' ';
				for (File f = FILE_A; f < FILE_N; ++f)
					cout << ((bb & make_square(f, r)) ? "x " : ". ");
				cout << endl;
			}
			cout << "  a b c d e f g h" << endl;
		}

	} // namespace Bitboards

} // namespace Clovis
