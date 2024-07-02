#include "bitboard.h"

namespace clovis::bitboards {

std::array<std::array<Bitboard, BISHOP_ATTACK_INDICES>, SQ_N> bishop_attacks;
std::array<std::array<Bitboard, ROOK_ATTACK_INDICES>, SQ_N> rook_attacks;

// prints a bitboard, useful for debugging
void print_bitboard(const Bitboard& bb)
{
    for (Rank r = RANK_8; r >= RANK_1; --r)
    {
        std::cout << r + 1 << ' ';
        for (File f = FILE_A; f < FILE_N; ++f) { std::cout << ((bb & make_square(f, r)) ? "x " : ". "); }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h" << '\n';
}

// generate bishop moves for a given square with bitboard of blocking pieces
constexpr Bitboard bishop_otf(const Square sq, const Bitboard occ)
{
    Bitboard attacks = 0ULL;

    for (const auto dir1 : {NORTH, SOUTH})
    {
        for (const auto dir2 : {EAST, WEST})
        {
            Square s = sq;
            while (valid_dir(s, dir1) && valid_dir(s, dir2))
            {
                s += dir1 + dir2;
                attacks |= s;
                if (occ & s) { break; }
            }
        }
    }

    return attacks;
}

// generate rook moves for a given square with bitboard of blocking pieces
constexpr Bitboard rook_otf(const Square sq, const Bitboard occ)
{
    Bitboard attacks = 0ULL;

    for (const auto dir : {NORTH, SOUTH, EAST, WEST})
    {
        Square s = sq;
        while (valid_dir(s, dir))
        {
            s += dir;
            attacks |= s;
            if (occ & s) { break; }
        }
    }

    return attacks;
}

// set occupancies for bits within an attack mask
// returns the occupancy bitboard
constexpr Bitboard set_occupancy(Bitboard attack_mask, const int index, const int bits)
{
    Bitboard occ = 0ULL;

    for (int i = 0; i < bits; ++i)
    {
        const Square sq = pop_lsb(attack_mask);
        if (index & (1 << i)) { occ |= sq; }
    }

    return occ;
}

void init_bitboards()
{
    for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
    {
        for (int index = 0; index < (1 << std::popcount(BISHOP_MASKS[sq])); ++index)
        {
            const Bitboard occ = set_occupancy(BISHOP_MASKS[sq], index, std::popcount(BISHOP_MASKS[sq]));
            bishop_attacks[sq][(occ * BISHOP_MAGIC[sq]) >> BISHOP_BIT_COUNT[sq]] = bishop_otf(sq, occ);
        }

        for (int index = 0; index < (1 << std::popcount(ROOK_MASKS[sq])); ++index)
        {
            const Bitboard occ = set_occupancy(ROOK_MASKS[sq], index, std::popcount(ROOK_MASKS[sq]));
            rook_attacks[sq][(occ * ROOK_MAGIC[sq]) >> ROOK_BIT_COUNT[sq]] = rook_otf(sq, occ);
        }
    }
}

} // namespace clovis::bitboards
