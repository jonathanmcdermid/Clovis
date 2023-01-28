#include "bitboard.h"
#include <iomanip>

using namespace std;

namespace Clovis {

	namespace Bitboards {

		Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
		Bitboard rook_attacks[SQ_N][rook_attack_indices];
	
		// prints a bitboard, useful for debugging
		void print_bitboard(const Bitboard& bb)
		{
			cout << "+---+---+---+---+---+---+---+---+\n";

			for (Rank r = RANK_8; r >= RANK_1; --r)
			{
				for (File f = FILE_A; f <= FILE_H; ++f)
				{
					cout << ((bb & make_square(f, r)) ? "| X " : "|   ");
				}
				cout << "|" + to_string(1 + r) + "\n" + "+---+---+---+---+---+---+---+---+\n";
			}
			cout << "  a   b   c   d   e   f   g   h\n" << "  Bitboard: " << bb << endl;
		}

		// generate bishop moves for a given square with bitboard of blocking pieces
		Bitboard bishop_otf(Square sq, Bitboard block)
		{
			Bitboard attacks = 0ULL;
			Rank r, tr = rank_of(sq);
			File f, tf = file_of(sq);

			for (r = tr + 1, f = tf + 1; r <= RANK_8 && f <= FILE_H; ++r, ++f) {
				attacks |= (1ULL << (r * 8 + f));
				if ((1ULL << (r * 8 + f)) & block) break;
			}
			for (r = tr - 1, f = tf + 1; r >= RANK_1 && f <= FILE_H; --r, ++f) {
				attacks |= (1ULL << (r * 8 + f));
				if ((1ULL << (r * 8 + f)) & block) break;
			}
			for (r = tr + 1, f = tf - 1; r <= RANK_8 && f >= FILE_A; ++r, --f) {
				attacks |= (1ULL << (r * 8 + f));
				if ((1ULL << (r * 8 + f)) & block) break;
			}
			for (r = tr - 1, f = tf - 1; r >= RANK_1 && f >= FILE_A; --r, --f) {
				attacks |= (1ULL << (r * 8 + f));
				if ((1ULL << (r * 8 + f)) & block) break;
			}

			return attacks;
		}

		// generate rook moves for a given square with bitboard of blocking pieces
		Bitboard rook_otf(Square sq, Bitboard block)
		{
			Bitboard attacks = 0ULL;
			Rank r, tr = rank_of(sq);
			File f, tf = file_of(sq);

			for (r = tr + 1; r <= RANK_8; ++r) {
				attacks |= (1ULL << (r * 8 + tf));
				if ((1ULL << (r * 8 + tf)) & block) break;
			}
			for (r = tr - 1; r >= RANK_1; --r) {
				attacks |= (1ULL << (r * 8 + tf));
				if ((1ULL << (r * 8 + tf)) & block) break;
			}
			for (f = tf + 1; f <= FILE_H; ++f) {
				attacks |= (1ULL << (tr * 8 + f));
				if ((1ULL << (tr * 8 + f)) & block) break;
			}
			for (f = tf - 1; f >= FILE_A; --f) {
				attacks |= (1ULL << (tr * 8 + f));
				if ((1ULL << (tr * 8 + f)) & block) break;
			}

			return attacks;
		}

		// set occupancies for bits within an attack mask
		// returns the occupancy bitboard
		Bitboard set_occupancy(Bitboard attack_mask, int index, int bits)
		{
			Bitboard occ = 0ULL;

			for (int i = 0; i < bits; ++i) 
			{
				Square sq = pop_lsb(attack_mask);
				
				if (index & (1 << i))
					occ |= sq;
			}

			return occ;
		}

		// populates slider attack tables
		template<PieceType PT>
		void init_sliders_attacks()
		{
			constexpr Bitboard (*OTF)(Square, Bitboard) = PT == BISHOP ? bishop_otf   : rook_otf;
			constexpr const Bitboard* ATTACK_MASK       = PT == BISHOP ? bishop_masks : rook_masks;
			constexpr const Bitboard* MAGIC             = PT == BISHOP ? bishop_magic : rook_magic;
			constexpr const int* RELEVANT_BITS          = PT == BISHOP ? bishop_rbits : rook_rbits;

			static_assert(PT == BISHOP || PT == ROOK);

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
			{
				int relevant_bits_count = popcnt(ATTACK_MASK[sq]);
				int occupancy_indicies = (1 << relevant_bits_count);

				for (int index = 0; index < occupancy_indicies; ++index)
				{
					Bitboard occ = set_occupancy(ATTACK_MASK[sq], index, relevant_bits_count);

					if (PT == BISHOP)
						bishop_attacks[sq][(occ * MAGIC[sq]) >> RELEVANT_BITS[sq]] = OTF(sq, occ);
					else
						rook_attacks[sq][(occ * MAGIC[sq]) >> RELEVANT_BITS[sq]] = OTF(sq, occ);
				}
			}
		}

		void init_bitboards()
		{
			init_sliders_attacks<BISHOP>();
			init_sliders_attacks<ROOK>();
		}

	} // namespace Bitboards

} // namespace Clovis
