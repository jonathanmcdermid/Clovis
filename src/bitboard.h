#pragma once

#include <intrin.h>
#include <cstring>
#include <cassert>

#include "random.h"
#include "types.h"

namespace Clovis {

    // precalculated bitboards for each square
    extern Bitboard sqbb[SQ_N];

    inline Bitboard sq_bb(Square sq) { return sqbb[sq]; }

    inline Bitboard operator&(Bitboard bb, Square sq) { return bb & sq_bb(sq); }
    inline Bitboard operator|(Bitboard bb, Square sq) { return bb | sq_bb(sq); }
    inline Bitboard operator^(Bitboard bb, Square sq) { return bb ^ sq_bb(sq); }
    inline Bitboard& operator|=(Bitboard& bb, Square sq) { return bb |= sq_bb(sq); }
    inline Bitboard& operator^=(Bitboard& bb, Square sq) { return bb ^= sq_bb(sq); }

    inline Bitboard operator|(Square s1, Square s2) { return sq_bb(s1) | sq_bb(s2); }

#if defined(__GNUC__)

    inline int count_bits(Bitboard bb) {
        return __builtin_popcountll(bb);
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        return Square(__builtin_ctzll(bb));
    }

#elif defined(_MSC_VER)

#ifdef _WIN64

    inline int count_bits(Bitboard bb) {
        return __popcnt64(bb);
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        unsigned long pos;
        _BitScanForward64(&pos, bb);
        return Square(pos);
    }

#else

    inline int count_bits(Bitboard bb) {
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

        extern Bitboard pawn_attacks[COLOUR_N][SQ_N];
        extern Bitboard knight_attacks[SQ_N];
        extern Bitboard king_attacks[SQ_N];
        extern Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
        extern Bitboard rook_attacks[SQ_N][rook_attack_indices];

        void print_bitboard(const Bitboard& bb);

        Bitboard mask_pawn_attacks(Square sq, Colour side);
        Bitboard mask_knight_attacks(Square sq);
        Bitboard mask_bishop_attacks(Square sq);
        Bitboard mask_rook_attacks(Square sq);
        Bitboard mask_king_attacks(Square sq);

        Bitboard bishop_otf(Square sq, Bitboard block);
        Bitboard rook_otf(Square sq, Bitboard block);

        Bitboard set_occupancy(Bitboard attack_mask, int index, int bits);

        Bitboard generate_magic();
        Bitboard find_magic(Square sq, int relevant_bits, bool is_bishop);

        void init_magic_numbers();

        void init_leapers_attacks();
        void init_sliders_attacks();

        Bitboard get_bishop_attacks(Bitboard occ, Square sq);
        Bitboard get_rook_attacks(Bitboard occ, Square sq);
        Bitboard get_queen_attacks(Bitboard occ, Square sq);

        Bitboard get_attacks(Bitboard occ, Square sq, PieceType pt);

        void init_bitboards(bool calc_magic = false);

    } // namespace Bitboards

} // namespace Clovis