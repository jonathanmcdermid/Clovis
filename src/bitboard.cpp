#include "bitboard.h"

using namespace std;

namespace Clovis {

	namespace Bitboards {

		// 64 - number of relevant occupancy bits for every bishop square
		constexpr int b_rbits[SQ_N] = {
			58, 59, 59, 59, 59, 59, 59, 58,
			59, 59, 59, 59, 59, 59, 59, 59,
			59, 59, 57, 57, 57, 57, 59, 59,
			59, 59, 57, 55, 55, 57, 59, 59,
			59, 59, 57, 55, 55, 57, 59, 59,
			59, 59, 57, 57, 57, 57, 59, 59,
			59, 59, 59, 59, 59, 59, 59, 59,
			58, 59, 59, 59, 59, 59, 59, 58,
		};

		// 64 - number of relevant occupancy bits for every rook square
		constexpr int r_rbits[SQ_N] = {
			52, 53, 53, 53, 53, 53, 53, 52,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			52, 53, 53, 53, 53, 53, 53, 52,
		};

		constexpr int attacks_size = 8 * rook_attack_indices;

		Bitboard pawn_attacks[COLOUR_N][SQ_N];
		Bitboard knight_attacks[SQ_N];
		Bitboard king_attacks[SQ_N];

		Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
		Bitboard rook_attacks[SQ_N][rook_attack_indices];

		Bitboard bishop_masks[SQ_N];
		Bitboard rook_masks[SQ_N];

		// precalculated magic numbers for generating rook attacks
		Bitboard r_magic[SQ_N] = {
			0x8a80104000800020ULL,
			0x140002000100040ULL,
			0x2801880a0017001ULL,
			0x100081001000420ULL,
			0x200020010080420ULL,
			0x3001c0002010008ULL,
			0x8480008002000100ULL,
			0x2080088004402900ULL,
			0x800098204000ULL,
			0x2024401000200040ULL,
			0x100802000801000ULL,
			0x120800800801000ULL,
			0x208808088000400ULL,
			0x2802200800400ULL,
			0x2200800100020080ULL,
			0x801000060821100ULL,
			0x80044006422000ULL,
			0x100808020004000ULL,
			0x12108a0010204200ULL,
			0x140848010000802ULL,
			0x481828014002800ULL,
			0x8094004002004100ULL,
			0x4010040010010802ULL,
			0x20008806104ULL,
			0x100400080208000ULL,
			0x2040002120081000ULL,
			0x21200680100081ULL,
			0x20100080080080ULL,
			0x2000a00200410ULL,
			0x20080800400ULL,
			0x80088400100102ULL,
			0x80004600042881ULL,
			0x4040008040800020ULL,
			0x440003000200801ULL,
			0x4200011004500ULL,
			0x188020010100100ULL,
			0x14800401802800ULL,
			0x2080040080800200ULL,
			0x124080204001001ULL,
			0x200046502000484ULL,
			0x480400080088020ULL,
			0x1000422010034000ULL,
			0x30200100110040ULL,
			0x100021010009ULL,
			0x2002080100110004ULL,
			0x202008004008002ULL,
			0x20020004010100ULL,
			0x2048440040820001ULL,
			0x101002200408200ULL,
			0x40802000401080ULL,
			0x4008142004410100ULL,
			0x2060820c0120200ULL,
			0x1001004080100ULL,
			0x20c020080040080ULL,
			0x2935610830022400ULL,
			0x44440041009200ULL,
			0x280001040802101ULL,
			0x2100190040002085ULL,
			0x80c0084100102001ULL,
			0x4024081001000421ULL,
			0x20030a0244872ULL,
			0x12001008414402ULL,
			0x2006104900a0804ULL,
			0x1004081002402ULL
		};

		// precalculated magic numbers for generating bishop attacks
		Bitboard b_magic[SQ_N] = {
			0x40040844404084ULL,
			0x2004208a004208ULL,
			0x10190041080202ULL,
			0x108060845042010ULL,
			0x581104180800210ULL,
			0x2112080446200010ULL,
			0x1080820820060210ULL,
			0x3c0808410220200ULL,
			0x4050404440404ULL,
			0x21001420088ULL,
			0x24d0080801082102ULL,
			0x1020a0a020400ULL,
			0x40308200402ULL,
			0x4011002100800ULL,
			0x401484104104005ULL,
			0x801010402020200ULL,
			0x400210c3880100ULL,
			0x404022024108200ULL,
			0x810018200204102ULL,
			0x4002801a02003ULL,
			0x85040820080400ULL,
			0x810102c808880400ULL,
			0xe900410884800ULL,
			0x8002020480840102ULL,
			0x220200865090201ULL,
			0x2010100a02021202ULL,
			0x152048408022401ULL,
			0x20080002081110ULL,
			0x4001001021004000ULL,
			0x800040400a011002ULL,
			0xe4004081011002ULL,
			0x1c004001012080ULL,
			0x8004200962a00220ULL,
			0x8422100208500202ULL,
			0x2000402200300c08ULL,
			0x8646020080080080ULL,
			0x80020a0200100808ULL,
			0x2010004880111000ULL,
			0x623000a080011400ULL,
			0x42008c0340209202ULL,
			0x209188240001000ULL,
			0x400408a884001800ULL,
			0x110400a6080400ULL,
			0x1840060a44020800ULL,
			0x90080104000041ULL,
			0x201011000808101ULL,
			0x1a2208080504f080ULL,
			0x8012020600211212ULL,
			0x500861011240000ULL,
			0x180806108200800ULL,
			0x4000020e01040044ULL,
			0x300000261044000aULL,
			0x802241102020002ULL,
			0x20906061210001ULL,
			0x5a84841004010310ULL,
			0x4010801011c04ULL,
			0xa010109502200ULL,
			0x4a02012000ULL,
			0x500201010098b028ULL,
			0x8040002811040900ULL,
			0x28000010020204ULL,
			0x6000020202d0240ULL,
			0x8918844842082200ULL,
			0x4010011029020020ULL
		};

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
		Bitboard mask_bishop_attacks(Square sq)
		{
			Bitboard attacks = 0ULL;
			Rank r, tr = rank_of(sq);
			File f, tf = file_of(sq);

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

		// generates a pseudo-random magic bitboard
		Bitboard generate_magic()
		{
			return Random::random_U64() & Random::random_U64() & Random::random_U64();
		}


		// brute force method of finding magic numbers for slider piece move generation
		Bitboard find_magic(Square sq, int relevant_bits, bool is_bishop)
		{
			Bitboard occ[rook_attack_indices];
			Bitboard attacks[rook_attack_indices];
			Bitboard used_attacks[rook_attack_indices];
			Bitboard mask = is_bishop ? mask_bishop_attacks(sq) : mask_rook_attacks(sq);
			int occ_indices = 1 << relevant_bits;

			for (int i = 0; i < occ_indices; ++i)
			{
				occ[i] = set_occupancy(mask, i, relevant_bits);
				attacks[i] = is_bishop ? bishop_otf(sq, occ[i]) : rook_otf(sq, occ[i]);
			}

			for (int i = 0; i < INT_MAX; ++i)
			{
				Bitboard magic = generate_magic();

				if (popcnt((mask * magic) & 0xFF00000000000000) < 6) 
					continue;

				memset(used_attacks, 0ULL, attacks_size);

				int index;

				for (index = 0; index < occ_indices; ++index)
				{
					int magic_index = (occ[index] * magic) >> (SQ_N - relevant_bits);

					if (used_attacks[magic_index] == 0ULL)
						used_attacks[magic_index] = attacks[index];
					else if (used_attacks[magic_index] != attacks[index])
						break;
				}

				if (index == occ_indices)
					return magic;
			}

			cout << "find_magic failed\n";
			return 0ULL;
		}

		// populates magic number tables
		void init_magic_numbers()
		{
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) {
				r_magic[sq] = find_magic(sq, r_rbits[sq], false);
				b_magic[sq] = find_magic(sq, b_rbits[sq], true);
			}
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
		void init_sliders_attacks()
		{
			bool is_bishop = true;
		start:
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
			{
				bishop_masks[sq] = mask_bishop_attacks(sq);
				rook_masks[sq] = mask_rook_attacks(sq);

				Bitboard attack_mask = is_bishop ? bishop_masks[sq] : rook_masks[sq];

				int relevant_bits_count = popcnt(attack_mask);
				int occupancy_indicies = (1 << relevant_bits_count);

				for (int index = 0; index < occupancy_indicies; ++index)
				{
					Bitboard occ = set_occupancy(attack_mask, index, relevant_bits_count);
					if (is_bishop)
						bishop_attacks[sq][(occ * b_magic[sq]) >> b_rbits[sq]] = bishop_otf(sq, occ);
					else
						rook_attacks[sq][(occ * r_magic[sq]) >> r_rbits[sq]] = rook_otf(sq, occ);
				}
			}
			if (is_bishop) 
			{
				is_bishop = false;
				goto start;
			}
		}

		void init_bitboards(bool calc_magic)
		{
			init_leapers_attacks();
			init_sliders_attacks();

			if (calc_magic)
				init_magic_numbers();
		}

		Bitboard get_bishop_attacks(Bitboard occ, Square sq)
		{
			return bishop_attacks[sq][(occ & bishop_masks[sq])* b_magic[sq] >> b_rbits[sq]];
		}

		Bitboard get_rook_attacks(Bitboard occ, Square sq)
		{
			return rook_attacks[sq][(occ & rook_masks[sq]) * r_magic[sq] >> r_rbits[sq]];
		}

		Bitboard get_queen_attacks(Bitboard occ, Square sq)
		{
			return get_bishop_attacks(occ, sq) | get_rook_attacks(occ, sq);
		}

	} // namespace Bitboards

} // namespace Clovis
