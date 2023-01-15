#pragma once

#include <immintrin.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <string>

#include "random.h"
#include "types.h"

namespace Clovis {

    // precalculated bitboards for each square
    constexpr Bitboard sqbb[SQ_N] = {
        0x1ULL,
        0x2ULL,
        0x4ULL,
        0x8ULL,
        0x10ULL,
        0x20ULL,
        0x40ULL,
        0x80ULL,
        0x100ULL,
        0x200ULL,
        0x400ULL,
        0x800ULL,
        0x1000ULL,
        0x2000ULL,
        0x4000ULL,
        0x8000ULL,
        0x10000ULL,
        0x20000ULL,
        0x40000ULL,
        0x80000ULL,
        0x100000ULL,
        0x200000ULL,
        0x400000ULL,
        0x800000ULL,
        0x1000000ULL,
        0x2000000ULL,
        0x4000000ULL,
        0x8000000ULL,
        0x10000000ULL,
        0x20000000ULL,
        0x40000000ULL,
        0x80000000ULL,
        0x100000000ULL,
        0x200000000ULL,
        0x400000000ULL,
        0x800000000ULL,
        0x1000000000ULL,
        0x2000000000ULL,
        0x4000000000ULL,
        0x8000000000ULL,
        0x10000000000ULL,
        0x20000000000ULL,
        0x40000000000ULL,
        0x80000000000ULL,
        0x100000000000ULL,
        0x200000000000ULL,
        0x400000000000ULL,
        0x800000000000ULL,
        0x1000000000000ULL,
        0x2000000000000ULL,
        0x4000000000000ULL,
        0x8000000000000ULL,
        0x10000000000000ULL,
        0x20000000000000ULL,
        0x40000000000000ULL,
        0x80000000000000ULL,
        0x100000000000000ULL,
        0x200000000000000ULL,
        0x400000000000000ULL,
        0x800000000000000ULL,
        0x1000000000000000ULL,
        0x2000000000000000ULL,
        0x4000000000000000ULL,
        0x8000000000000000ULL,
    };

    constexpr Bitboard operator|(Square sq1, Square sq2) { return sqbb[sq1] | sqbb[sq2]; }
    constexpr Bitboard operator&(Bitboard bb, Square sq) { return bb & sqbb[sq]; }
    constexpr Bitboard operator|(Bitboard bb, Square sq) { return bb | sqbb[sq]; }
    constexpr Bitboard operator^(Bitboard bb, Square sq) { return bb ^ sqbb[sq]; }
    constexpr Bitboard& operator|=(Bitboard& bb, Square sq) { return bb |= sqbb[sq]; }
    constexpr Bitboard& operator^=(Bitboard& bb, Square sq) { return bb ^= sqbb[sq]; }

#if defined(__GNUC__)

    inline int popcnt(Bitboard bb) {
        return __builtin_popcountll(bb);
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        return Square(__builtin_ctzll(bb));
    }

#elif defined(_MSC_VER)

#ifdef _WIN64

    inline int popcnt(Bitboard bb) {
        return __popcnt64(bb);
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        unsigned long pos;
        _BitScanForward64(&pos, bb);
        return Square(pos);
    }

#else

    inline int popcnt(Bitboard bb) {
        return __popcnt(int32_t(bb)) + __popcnt(int32_t(bb >> 32));
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        unsigned long pos;

        if (bb & 0xffffffff)
        {
            _BitScanForward(&pos, int32_t(bb));
            return Square(pos);
        }
        else
        {
            _BitScanForward(&pos, int32_t(bb >> 32));
            return Square(pos + 32);
        }
    }

#endif

#else  // Cant use GCC or MSVC 

#error "Invalid Compiler"

#endif

    inline Square pop_lsb(Bitboard& bb) {
        assert(bb);
        Square sq = lsb(bb);
        bb &= bb - 1;
        return sq;
    }

    constexpr Bitboard not_a_file = 18374403900871474942ULL;
    constexpr Bitboard not_h_file = 9187201950435737471ULL;
    constexpr Bitboard not_gh_file = 4557430888798830399ULL;
    constexpr Bitboard not_ab_file = 18229723555195321596ULL;

    namespace Bitboards {

        constexpr int bishop_attack_indices = 512;
        constexpr int rook_attack_indices = 4096;

		// 64 - number of relevant occupancy bits for every bishop square
		constexpr int bishop_rbits[SQ_N] = {
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
		constexpr int rook_rbits[SQ_N] = {
			52, 53, 53, 53, 53, 53, 53, 52,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			52, 53, 53, 53, 53, 53, 53, 52,
		};

		// precalculated magic numbers for generating rook attacks
		constexpr Bitboard rook_magic[SQ_N] = {
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
		constexpr Bitboard bishop_magic[SQ_N] = {
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

		extern Bitboard bishop_masks[SQ_N];
		extern Bitboard rook_masks[SQ_N];
        extern Bitboard pawn_attacks[COLOUR_N][SQ_N];
        extern Bitboard knight_attacks[SQ_N];
        extern Bitboard king_attacks[SQ_N];
        extern Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
        extern Bitboard rook_attacks[SQ_N][rook_attack_indices];

        void print_bitboard(const Bitboard& bb);

        void init_bitboards();

        template<PieceType PT>
        inline Bitboard get_attacks(Bitboard occ, Square sq)
		{
			assert(PT != PAWN);
            return PT == KNIGHT ? knight_attacks[sq] 
                : PT == BISHOP	? bishop_attacks[sq][(occ & bishop_masks[sq]) * bishop_magic[sq] >> bishop_rbits[sq]]
                : PT == ROOK	? rook_attacks[sq][(occ & rook_masks[sq]) * rook_magic[sq] >> rook_rbits[sq]]
                : PT == QUEEN	? get_attacks<BISHOP>(occ, sq) | get_attacks<ROOK>(occ, sq)
                : king_attacks[sq];
        }

    } // namespace Bitboards

} // namespace Clovis
