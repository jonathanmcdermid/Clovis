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

		extern int bishop_rbits[SQ_N];
		extern int rook_rbits[SQ_N];

		extern Bitboard bishop_masks[SQ_N];
		extern Bitboard rook_masks[SQ_N];
		extern Bitboard bishop_magic[SQ_N];
		extern Bitboard rook_magic[SQ_N];
        extern Bitboard pawn_attacks[COLOUR_N][SQ_N];
        extern Bitboard knight_attacks[SQ_N];
        extern Bitboard king_attacks[SQ_N];
        extern Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
        extern Bitboard rook_attacks[SQ_N][rook_attack_indices];

        void print_bitboard(const Bitboard& bb);

        void init_bitboards(bool calc_magic);

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
