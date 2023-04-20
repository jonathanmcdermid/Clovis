#include "bitboard.h"

using namespace std;

namespace Clovis {

	Bitboard between_bb[SQ_N][SQ_N];

	namespace Bitboards {

		Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
		Bitboard rook_attacks[SQ_N][rook_attack_indices];

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

		// generate bishop moves for a given square with bitboard of blocking pieces
		Bitboard bishop_otf(Square sq, Bitboard occ) {

			Bitboard attacks = 0ULL;

			for (const auto& dir1 : { NORTH, SOUTH }) {
				for (const auto& dir2 : { EAST, WEST }) {
					Square s = sq;
					while (valid_dir(s, dir1) && valid_dir(s, dir2)) {
						s += dir1 + dir2;
						attacks |= s;
						if (occ & s) break;
					}
				}
			}

			return attacks;
		}

		// generate rook moves for a given square with bitboard of blocking pieces
		Bitboard rook_otf(Square sq, Bitboard occ) {

			Bitboard attacks = 0ULL;

			for (const auto& dir : { NORTH, SOUTH, EAST, WEST }) {
				Square s = sq;
				while (valid_dir(s, dir)) {
					s += dir;
					attacks |= s;
					if (occ & s) break;
				}
			}

			return attacks;
		}

		// set occupancies for bits within an attack mask
		// returns the occupancy bitboard
		Bitboard set_occupancy(Bitboard attack_mask, int index, int bits) {

			Bitboard occ = 0ULL;

			for (int i = 0; i < bits; ++i) {
				Square sq = pop_lsb(attack_mask);
				if (index & (1 << i))
					occ |= sq;
			}

			return occ;
		}

		// populates slider attack tables
		void init_sliders_attacks() {

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) {
				for (int index = 0; index < (1 << popcount(bishop_masks[sq])); ++index) {
					Bitboard occ = set_occupancy(bishop_masks[sq], index, popcount(bishop_masks[sq]));
					bishop_attacks[sq][(occ * bishop_magic[sq]) >> bishop_rbits[sq]] = bishop_otf(sq, occ);
				}
				
				for (int index = 0; index < (1 << popcount(rook_masks[sq])); ++index) {
					Bitboard occ = set_occupancy(rook_masks[sq], index, popcount(rook_masks[sq]));
					rook_attacks[sq][(occ * rook_magic[sq]) >> rook_rbits[sq]] = rook_otf(sq, occ);
				}
			}
		}

		void init_bitboards() {

			init_sliders_attacks();

			for (Square sq1 = SQ_ZERO; sq1 < SQ_N; ++sq1) {
				for (Square sq2 = SQ_ZERO; sq2 < SQ_N; ++sq2) {
					if (get_attacks<ROOK>(sq1) & sq2)
						between_bb[sq1][sq2] = get_attacks<ROOK>  (sqbb(sq2), sq1) & get_attacks<ROOK>  (sqbb(sq1), sq2);
					else if (get_attacks<BISHOP>(sq1) & sq2)
						between_bb[sq1][sq2] = get_attacks<BISHOP>(sqbb(sq2), sq1) & get_attacks<BISHOP>(sqbb(sq1), sq2);
				}
			}
		}

	} // namespace Bitboards

} // namespace Clovis
