#include "bitboard.h"

using namespace std;

namespace Clovis {

	namespace Bitboards {

		Bitboard pawn_attacks[COLOUR_N][SQ_N];
		Bitboard knight_attacks[SQ_N];
		Bitboard king_attacks[SQ_N];

		Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
		Bitboard rook_attacks[SQ_N][rook_attack_indices];

		Bitboard bishop_masks[SQ_N];
		Bitboard rook_masks[SQ_N];
	
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

		// generates pawn attacks for a given square and side
		template<Colour c> 
		Bitboard mask_pawn_attacks(Square sq)
		{
			Bitboard attacks = 0ULL;
			Bitboard bb = sqbb[sq];
		
			if (c == WHITE) {
				if ((bb << 7) & not_h_file) attacks |= (bb << 7ULL);
				if ((bb << 9) & not_a_file) attacks |= (bb << 9ULL);
			}
			else {
				if ((bb >> 7) & not_a_file) attacks |= (bb >> 7ULL);
				if ((bb >> 9) & not_h_file) attacks |= (bb >> 9ULL);
			}
		
			return attacks;
		}

		// generates knight attacks for a given square
		Bitboard mask_knight_attacks(Square sq)
		{
			Bitboard attacks = 0ULL;
			Bitboard bb = sqbb[sq];

			if ((bb << 17) & not_a_file)	attacks |= (bb << 17ULL);
			if ((bb >> 17) & not_h_file)	attacks |= (bb >> 17ULL);
			if ((bb << 15) & not_h_file)	attacks |= (bb << 15ULL);
			if ((bb >> 15) & not_a_file)	attacks |= (bb >> 15ULL);
			if ((bb << 10) & not_ab_file)	attacks |= (bb << 10ULL);
			if ((bb >> 10) & not_gh_file)	attacks |= (bb >> 10ULL);
			if ((bb << 6) & not_gh_file)	attacks |= (bb << 6ULL);
			if ((bb >> 6) & not_ab_file)	attacks |= (bb >> 6ULL);

			return attacks;
		}

		// generates relevant squares for bishop attacks on a given square
		constexpr Bitboard mask_bishop_attacks(Square sq)
		{
			Bitboard attacks = 0ULL;
			Rank r = RANK_NONE, tr = rank_of(sq);
			File f = FILE_NONE, tf = file_of(sq);

			for (r = tr + 1, f = tf + 1; r < RANK_8 && f < FILE_H; ++r, ++f)
				attacks |= (1ULL << (r * RANK_N + f));
			for (r = tr - 1, f = tf + 1; r > RANK_1 && f < FILE_H; --r, ++f)
				attacks |= (1ULL << (r * RANK_N + f));
			for (r = tr + 1, f = tf - 1; r < RANK_8 && f > FILE_A; ++r, --f)
				attacks |= (1ULL << (r * RANK_N + f));
			for (r = tr - 1, f = tf - 1; r > RANK_1 && f > FILE_A; --r, --f)
				attacks |= (1ULL << (r * RANK_N + f));

			return attacks;
		}

		// generates relevant squares for rook attacks on a given square
		Bitboard mask_rook_attacks(Square sq)
		{
			Bitboard attacks = 0ULL;
			Rank r, tr = rank_of(sq);
			File f, tf = file_of(sq);

			for (r = tr + 1; r <= 6; ++r)
				attacks |= (1ULL << (r * 8 + tf));
			for (r = tr - 1; r >= 1; --r)
				attacks |= (1ULL << (r * 8 + tf));
			for (f = tf + 1; f <= 6; ++f)
				attacks |= (1ULL << (tr * 8 + f));
			for (f = tf - 1; f >= 1; --f)
				attacks |= (1ULL << (tr * 8 + f));

			return attacks;
		}

		// generates king attacks for a given square
		Bitboard mask_king_attacks(Square sq)
		{
			Bitboard attacks = 0ULL;
			Bitboard bb = sqbb[sq];

			if ((bb << 1) & not_a_file) attacks |= (bb << 1ULL);
			if ((bb >> 1) & not_h_file) attacks |= (bb >> 1ULL);
			if ((bb << 8))				attacks |= (bb << 8ULL);
			if ((bb >> 8))				attacks |= (bb >> 8ULL);
			if ((bb << 7) & not_h_file) attacks |= (bb << 7ULL);
			if ((bb >> 7) & not_a_file) attacks |= (bb >> 7ULL);
			if ((bb << 9) & not_a_file) attacks |= (bb << 9ULL);
			if ((bb >> 9) & not_h_file) attacks |= (bb >> 9ULL);

			return attacks;
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

			for (int i = 0; i < bits; ++i) {
				Square sq = pop_lsb(attack_mask);
				if (index & (1 << i))
					occ |= sq;
			}

			return occ;
		}

		// populates leaper attack tables
		void init_leapers_attacks()
		{
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
			{
				pawn_attacks[WHITE][sq] = mask_pawn_attacks<WHITE>(sq);
				pawn_attacks[BLACK][sq] = mask_pawn_attacks<BLACK>(sq);
				knight_attacks[sq] = mask_knight_attacks(sq);
				king_attacks[sq] = mask_king_attacks(sq);
			}
		}

		// populates slider attack tables
		template<PieceType PT>
		void init_sliders_attacks()
		{
			constexpr Bitboard (*OTF)(Square, Bitboard)	= PT == BISHOP ? bishop_otf : rook_otf;
			constexpr Bitboard (*MASK_ATTACKS)(Square)	= PT == BISHOP ? mask_bishop_attacks : mask_rook_attacks;
			constexpr Bitboard* ATTACK_MASK				= PT == BISHOP ? bishop_masks : rook_masks;
			constexpr const Bitboard* MAGIC				= PT == BISHOP ? bishop_magic : rook_magic;
			constexpr const int* RELEVANT_BITS			= PT == BISHOP ? bishop_rbits : rook_rbits;

			assert(PT == BISHOP || PT == ROOK);

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
			{
				ATTACK_MASK[sq] = MASK_ATTACKS(sq);

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
			init_leapers_attacks();
			init_sliders_attacks<BISHOP>();
			init_sliders_attacks<ROOK>();
		}

	} // namespace Bitboards

} // namespace Clovis
