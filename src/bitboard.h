#pragma once

#include <intrin.h>

#include "random.h"
#include "types.h"

#define count_bits(bitboard) __popcnt64(bitboard)

namespace Clovis {

    constexpr U64 get_bit(const Bitboard& bb, Square sq) { return bb & (1ULL << sq); }
    constexpr void set_bit(Bitboard& bb, Square sq) { bb |= (1ULL << sq); }
    constexpr void pop_bit(Bitboard& bb, Square sq) { bb = (get_bit(bb, sq) ? bb ^= (1ULL << sq) : bb); }
    constexpr Square get_lsb_index(Bitboard bb) { return (bb) ? Square(count_bits((bb & -bb) - 1)) : SQ_NONE; }

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

        void init_bitboards(bool calc_magic = false);

    } // namespace Bitboards

} // namespace Clovis