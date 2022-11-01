#pragma once

#include <intrin.h>
#include <cstring>
#include <cassert>

#include "random.h"
#include "types.h"

#define count_bits(bitboard) __popcnt64(bitboard)

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

    inline Square lsb(Bitboard bb) {
        assert(bb);
        unsigned long i;
        _BitScanForward64(&i, bb);
        return (Square) i;
    }

    inline Square msb(Bitboard bb) {
        assert(bb);
        unsigned long i;
        _BitScanReverse64(&i, bb);
        return (Square) i;
    }

    inline Bitboard lsb_bb(Bitboard bb) {
        assert(bb);
        return bb & -bb;
    }

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