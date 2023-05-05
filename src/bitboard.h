#pragma once

#include <bit>

#include "types.h"

namespace Clovis {

	constexpr Bitboard sqbb(const Square sq) { return 1ULL << sq; }

	constexpr Bitboard operator|(const Square sq1, const Square sq2) { return sqbb(sq1) | sqbb(sq2); }
	constexpr Bitboard operator&(const Bitboard bb, const Square sq) { return bb & sqbb(sq); }
	constexpr Bitboard operator|(const Bitboard bb, const Square sq) { return bb | sqbb(sq); }
	constexpr Bitboard operator^(const Bitboard bb, const Square sq) { return bb ^ sqbb(sq); }
	constexpr Bitboard& operator|=(Bitboard& bb, const Square sq) { return bb |= sqbb(sq); }
	constexpr Bitboard& operator^=(Bitboard& bb, const Square sq) { return bb ^= sqbb(sq); }

	constexpr Square lsb(const Bitboard bb) {
		assert(bb);
		return static_cast<Square>(std::countr_zero(bb));
	}

	constexpr Square pop_lsb(Bitboard& bb) {
		assert(bb);
		const Square sq = lsb(bb);
		bb &= bb - 1;
		return sq;
	}

	constexpr bool multiple_bits(const Bitboard bb) {
		return bb & (bb - 1);
	}

	template<Direction D>
	constexpr Bitboard shift(const Bitboard bb) {
		if constexpr (D >= 0) return bb << D;
		else return bb >> -D;
	}

	namespace Bitboards {

		constexpr int bishop_attack_indices = 512;
		constexpr int rook_attack_indices = 4096;

		// pre-calculated magic numbers for generating rook attacks
		constexpr auto rook_magic = std::array{
			0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,  0x100081001000420ULL, 
			0x200020010080420ULL,  0x3001c0002010008ULL,  0x8480008002000100ULL, 0x2080088004402900ULL, 
			0x800098204000ULL,     0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL, 
			0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL, 0x801000060821100ULL, 
			0x80044006422000ULL,   0x100808020004000ULL,  0x12108a0010204200ULL, 0x140848010000802ULL, 
			0x481828014002800ULL,  0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL, 
			0x100400080208000ULL,  0x2040002120081000ULL, 0x21200680100081ULL,   0x20100080080080ULL, 
			0x2000a00200410ULL,    0x20080800400ULL,      0x80088400100102ULL,   0x80004600042881ULL, 
			0x4040008040800020ULL, 0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL, 
			0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,  0x200046502000484ULL, 
			0x480400080088020ULL,  0x1000422010034000ULL, 0x30200100110040ULL,   0x100021010009ULL, 
			0x2002080100110004ULL, 0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL, 
			0x101002200408200ULL,  0x40802000401080ULL,   0x4008142004410100ULL, 0x2060820c0120200ULL, 
			0x1001004080100ULL,    0x20c020080040080ULL,  0x2935610830022400ULL, 0x44440041009200ULL, 
			0x280001040802101ULL,  0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL, 
			0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,  0x1004081002402ULL, 
		};

		// pre-calculated magic numbers for generating bishop attacks
		constexpr auto bishop_magic = std::array{
			0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,   0x108060845042010ULL, 
			0x581104180800210ULL,  0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL, 
			0x4050404440404ULL,    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL, 
			0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,  0x801010402020200ULL, 
			0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL, 
			0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL, 
			0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL, 
			0x4001001021004000ULL, 0x800040400a011002ULL, 0xe4004081011002ULL,   0x1c004001012080ULL, 
			0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL, 
			0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL, 
			0x209188240001000ULL,  0x400408a884001800ULL, 0x110400a6080400ULL,   0x1840060a44020800ULL, 
			0x90080104000041ULL,   0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL, 
			0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL, 0x300000261044000aULL, 
			0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL, 0x4010801011c04ULL, 
			0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL, 
			0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL, 
		};

		constexpr auto file_masks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				for (Rank r = RANK_1; r < RANK_N; ++r)
					arr[sq] |= make_square(file_of(sq), r);

			return arr;
		}();

		constexpr auto rank_masks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				for (File f = FILE_A; f < FILE_N; ++f)
					arr[sq] |= make_square(f, rank_of(sq));

			return arr;
		}();

		constexpr auto pawn_attacks = [] {
			std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr{};

			for (const auto c : { WHITE, BLACK })
				for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
					for (const auto dir : { EAST, WEST })
						if (is_valid(sq + pawn_push(c)) && rank_of(sq) == rank_of(sq + dir))
							arr[c][sq] |= sq + pawn_push(c) + dir;

			return arr;
		}();

		constexpr auto knight_attacks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) {
				for (const auto d1 : { NORTH, SOUTH }) {
					for (const auto d2 : { EAST, WEST }) {
						if (is_valid(sq + d1 + d1) && rank_of(sq) == rank_of(sq + d2))
							arr[sq] |= sq + d1 + d1 + d2;
						if (is_valid(sq + d1) && rank_of(sq) == rank_of(sq + d2 + d2))
							arr[sq] |= sq + d1 + d2 + d2;
					}
				}
			}

			return arr;
		}();

		constexpr auto empty_bishop_attacks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
				for (const auto d1 : { NORTH, SOUTH })
					for (const auto d2 : { EAST, WEST })
						for (Square s2 = s1 + d1 + d2; is_valid(s2) && rank_of(s2) == rank_of(s2 - d2); s2 += d1 + d2)
							arr[s1] |= s2;

			return arr;
		}();

		constexpr auto empty_rook_attacks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1) {
				for (const auto dir : { NORTH, SOUTH })
					for (Square s2 = s1 + dir; is_valid(s2); s2 += dir)
						arr[s1] |= s2;
				for (const auto dir : { EAST, WEST })
					for (Square s2 = s1 + dir; rank_of(s2) == rank_of(s2 - dir); s2 += dir)
						arr[s1] |= s2;
			}

			return arr;
		}();

		constexpr auto empty_queen_attacks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[sq] = empty_bishop_attacks[sq] | empty_rook_attacks[sq];

			return arr;
		}();

		constexpr auto king_attacks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				for (const auto d1 : { NO_DIR, NORTH, SOUTH })
					for (const auto d2 : { NO_DIR, EAST, WEST })
						if (d1 != d2 && is_valid(sq + d1) && rank_of(sq) == rank_of(sq + d2))
							arr[sq] |= sq + d1 + d2;

			return arr;
		}();

		constexpr auto bishop_masks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
				for (const auto d1 : { NORTH, SOUTH })
					for (const auto d2 : { EAST, WEST })
						for (Square s2 = s1; is_valid(s2 + d1) && rank_of(s2) == rank_of(s2 + d2); s2 += d1 + d2)
							if (s2 != s1)
								arr[s1] |= s2;

			return arr;
		}();

		constexpr auto rook_masks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1) {
				for (const auto dir : { NORTH, SOUTH })
					for (Square s2 = s1; is_valid(s2 + dir); s2 += dir)
						if (s2 != s1)
							arr[s1] |= s2;
				for (const auto dir : { EAST, WEST })
					for (Square s2 = s1; rank_of(s2) == rank_of(s2 + dir); s2 += dir)
						if (s2 != s1)
							arr[s1] |= s2;
			}

			return arr;
		}();

		constexpr auto bishop_rbits = [] {
			std::array<int, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[sq] = SQ_N - std::popcount(bishop_masks[sq]);

			return arr;
		}();

		constexpr auto rook_rbits = [] {
			std::array<int, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[sq] = SQ_N - std::popcount(rook_masks[sq]);

			return arr;
		}();

		constexpr auto between_bb = [] {
			std::array<std::array<Bitboard, SQ_N>, SQ_N> arr{};

			for (Square sq1 = SQ_ZERO; sq1 < SQ_N; ++sq1) {
				for (Square sq2 = SQ_ZERO; sq2 < SQ_N; ++sq2) {

					Bitboard bb = (empty_bishop_attacks[sq1] & sq2)
						? empty_bishop_attacks[sq1] & empty_bishop_attacks[sq2]
						: (empty_rook_attacks[sq1] & sq2)
						? empty_rook_attacks[sq1] & empty_rook_attacks[sq2]
						: 0ULL;

					while (bb)
						if (const Square sq3 = pop_lsb(bb); 
							(sq1 < sq3 && sq3 < sq2) || (sq1 > sq3 && sq3 > sq2))
							arr[sq1][sq2] |= sq3;
				}
			}

			return arr;
		}();

		extern std::array<std::array<Bitboard, bishop_attack_indices>, SQ_N> bishop_attacks;
		extern std::array<std::array<Bitboard, rook_attack_indices>, SQ_N> rook_attacks;

		void print_bitboard(const Bitboard& bb);
		void init_bitboards();

		constexpr Bitboard between_squares(const Square sq1, const Square sq2) { return between_bb[sq1][sq2]; }

		template<PieceType PT>
		constexpr Bitboard get_attacks(const Bitboard occ, const Square sq) {

			static_assert(PT != PAWN);
            
			return PT == KNIGHT    ? knight_attacks[sq]
			        : PT == BISHOP ? bishop_attacks[sq][(occ & bishop_masks[sq]) * bishop_magic[sq] >> bishop_rbits[sq]]
				: PT == ROOK   ? rook_attacks  [sq][(occ & rook_masks  [sq]) * rook_magic[sq]   >> rook_rbits  [sq]]
				: PT == QUEEN  ? get_attacks<BISHOP>(occ, sq) | get_attacks<ROOK>(occ, sq)
				: king_attacks[sq];
		}

		template<PieceType PT>
		constexpr Bitboard get_attacks(const Square sq) {

			static_assert(PT != PAWN);
            
			return PT == KNIGHT    ? knight_attacks[sq]
			        : PT == BISHOP ? empty_bishop_attacks[sq]
				: PT == ROOK   ? empty_rook_attacks[sq]
				: PT == QUEEN  ? empty_queen_attacks[sq]
				: king_attacks[sq];
		}

		constexpr Bitboard get_attacks(const PieceType pt, const Bitboard occ, const Square sq) {

			assert(pt != PAWN);

			return pt == KNIGHT    ? knight_attacks[sq]
			        : pt == BISHOP ? get_attacks<BISHOP>(occ, sq)
				: pt == ROOK   ? get_attacks<ROOK>(occ, sq)
				: pt == QUEEN  ? get_attacks<QUEEN>(occ, sq)
				: king_attacks[sq];
		}

	} // namespace Bitboards

} // namespace Clovis
