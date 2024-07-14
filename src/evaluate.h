#pragma once

#include "position.h"
#include "tt.h"

namespace clovis::eval {

struct EvalInfo : PTEntry
{
    constexpr EvalInfo() = default;
    explicit EvalInfo(const PTEntry& pte) : PTEntry(pte) {}

    std::array<int, 2> n_att{0, 0};
};

// clang-format off
constexpr std::array<Score, 32> PAWN_SOURCE = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {184, 244}, {182, 248}, {158, 235}, {195, 203},
    {71, 97}, {89, 96}, {108, 88}, {111, 44},
    {65, 85}, {79, 80}, {90, 75}, {102, 73},
    {59, 77}, {67, 80}, {82, 74}, {99, 70},
    {63, 71}, {72, 72}, {77, 76}, {82, 80},
    {57, 73}, {78, 75}, {73, 83}, {69, 84},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 16> KNIGHT_SOURCE = {{
    {248, 199}, {294, 209}, {285, 234}, {301, 232},
    {306, 212}, {298, 235}, {314, 238}, {320, 240},
    {300, 222}, {316, 236}, {318, 243}, {327, 252},
    {305, 235}, {309, 244}, {324, 251}, {321, 258},
}};

constexpr std::array<Score, 16> BISHOP_SOURCE = {{
    {303, 230}, {335, 223}, {315, 235}, {322, 235},
    {303, 236}, {332, 220}, {332, 234}, {328, 238},
    {313, 238}, {331, 238}, {334, 239}, {331, 243},
    {307, 241}, {311, 241}, {317, 242}, {328, 238},
}};

constexpr std::array<Score, 16> ROOK_SOURCE = {{
    {441, 434}, {439, 436}, {442, 437}, {443, 435},
    {418, 439}, {440, 433}, {451, 429}, {450, 429},
    {434, 437}, {442, 437}, {446, 434}, {451, 434},
    {428, 445}, {446, 438}, {447, 438}, {454, 434},
}};

constexpr std::array<Score, 32> QUEEN_SOURCE = {{
    {870, 814}, {837, 845}, {833, 856}, {866, 840},
    {876, 806}, {848, 839}, {869, 843}, {837, 865},
    {895, 805}, {892, 820}, {885, 830}, {874, 848},
    {880, 830}, {874, 838}, {872, 838}, {872, 841},
    {892, 812}, {888, 831}, {887, 819}, {886, 839},
    {887, 816}, {900, 811}, {892, 826}, {892, 833},
    {890, 791}, {905, 784}, {908, 798}, {907, 805},
    {893, 786}, {888, 787}, {889, 790}, {902, 785},
}};

constexpr std::array<Score, 16> KING_SOURCE = {{
    {76, 14}, {105, 36}, {72, 58}, {69, 59},
    {88, 44}, {107, 56}, {82, 72}, {53, 85},
    {45, 63}, {89, 68}, {68, 85}, {62, 94},
    {37, 64}, {70, 79}, {71, 92}, {41, 101},
}};

constexpr std::array<Score, 32> PASSED_PAWN = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {40, 111}, {29, 110}, {41, 85}, {29, 108},
    {30, 61}, {22, 63}, {11, 49}, {5, 35},
    {12, 39}, {8, 40}, {0, 23}, {0, 20},
    {0, 11}, {0, 20}, {0, 7}, {0, 0},
    {2, 15}, {0, 13}, {0, 2}, {0, 3},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, RANK_N> CANDIDATE_PASSER = {{
    {0, 0}, {0, 5}, {0, 9}, {7, 24}, {20, 49}, {25, 69}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 7> QUIET_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {6, 1}, {4, 4}, {3, 3}, {1, 3}, {0, 0},
}};

constexpr std::array<Score, 7> CAPTURE_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {8, 21}, {13, 20}, {10, 23}, {1, 16}, {0, 0},
}};

constexpr Score DOUBLE_PAWN_PENALTY = {1, 9};
constexpr Score ISOLATED_PAWN_PENALTY = {12, 6};
constexpr Score BISHOP_PAIR_BONUS = {23, 43};
constexpr Score ROOK_OPEN_FILE_BONUS = {18, 2};
constexpr Score ROOK_SEMI_OPEN_FILE_BONUS = {0, 0};
constexpr Score ROOK_CLOSED_FILE_PENALTY = {14, 4};
constexpr Score TEMPO_BONUS = {23, 15};
constexpr Score KING_OPEN_PENALTY = {33, 10};
constexpr Score KING_ADJACENT_OPEN_PENALTY = {4, 10};
constexpr Score KNIGHT_OUTPOST_BONUS = {37, 11};
constexpr Score BISHOP_OUTPOST_BONUS = {35, 0};
constexpr Score WEAK_QUEEN_PENALTY = {31, 5};
constexpr Score ROOK_ON_OUR_PASSER_FILE = {4, 8};
constexpr Score ROOK_ON_THEIR_PASSER_FILE = {5, 29};
constexpr Score TALL_PAWN_PENALTY = {10, 24};
constexpr Score FIANCHETTO_BONUS = {17, 11};
constexpr Score ROOK_ON_SEVENTH_RANK = {0, 23};

constexpr std::array<short, 32> PAWN_SHIELD = {{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    32, 34, 30, 0,
}};

constexpr std::array<short, 7> INNER_RING_ATTACK = {{
    0, 19, 18, 26, 23, 21, 0,
}};

constexpr std::array<short, 7> OUTER_RING_ATTACK = {{
    0, 0, 28, 14, 10, 19, 0,
}};

constexpr short ATTACK_FACTOR = 59;
constexpr short VIRTUAL_MOBILITY = 14;

// clang-format on

constexpr auto SOURCE_32 = [] {
    std::array<Square, SQ_N> arr{};

    for (Square sq = SQ_ZERO; sq < 32; ++sq)
    {
        const int r = sq / 4;
        const int f = sq & 0x3;
        arr[make_square(f, r ^ 7)] = arr[make_square(f ^ 7, r ^ 7)] = sq;
    }

    return arr;
}();

constexpr auto SOURCE_16 = [] {
    std::array<Square, SQ_N> arr = SOURCE_32;

    for (Square sq = SQ_ZERO; sq < 16; ++sq)
    {
        const int r = sq / 4;
        const int f = sq & 0x3;
        arr[make_square(f, r ^ 7) ^ 56] = arr[make_square(f ^ 7, r ^ 7) ^ 56] = sq;
    }

    return arr;
}();

constexpr auto SOURCE_10 = [] {
    std::array<Square, SQ_N> arr{};

    Square index = SQ_ZERO;

    for (Square sq = SQ_ZERO; sq < 16; ++sq)
    {
        if (const int r = sq / 4, f = sq & 0x3; r >= f)
        {
            arr[make_square(f, r)] = arr[make_square(f, r ^ 7)] = arr[make_square(f ^ 7, r)] = arr[make_square(f ^ 7, r ^ 7)] = index;
            arr[make_square(r, f)] = arr[make_square(r, f ^ 7)] = arr[make_square(r ^ 7, f)] = arr[make_square(r ^ 7, f ^ 7)] = index;
            ++index;
        }
    }

    return arr;
}();

constexpr std::array<const Score*, 7> PIECE_TYPE_SOURCE = {
    nullptr, PAWN_SOURCE.data(), KNIGHT_SOURCE.data(), BISHOP_SOURCE.data(), ROOK_SOURCE.data(), QUEEN_SOURCE.data(), KING_SOURCE.data()};

constexpr auto PIECE_TABLE = [] {
    std::array<std::array<Score, SQ_N>, 15> arr{};

    for (const auto col : {WHITE, BLACK})
    {
        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
        {
            for (const auto pt : {PAWN, QUEEN}) { arr[make_piece(pt, col)][sq] = PIECE_TYPE_SOURCE[pt][SOURCE_32[relative_square(col, sq)]]; }
            for (const auto pt : {KNIGHT, BISHOP, ROOK, KING}) { arr[make_piece(pt, col)][sq] = PIECE_TYPE_SOURCE[pt][SOURCE_16[sq]]; }
            // for (const auto pt : {})
            //     arr[make_piece(pt, col)][sq] =
            //     piece_type_source[pt][source10[sq]];
        }
    }

    return arr;
}();

constexpr auto PASSED_TABLE = [] {
    std::array<std::array<Score, SQ_N>, 2> arr{};

    for (const auto col : {WHITE, BLACK})
    {
        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) { arr[col][sq] = PASSED_PAWN[SOURCE_32[relative_square(col, sq)]]; }
    }

    return arr;
}();

constexpr auto SHIELD_TABLE = [] {
    std::array<std::array<short, SQ_N>, 2> arr{};

    for (const auto col : {WHITE, BLACK})
    {
        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) { arr[col][sq] = PAWN_SHIELD[SOURCE_32[relative_square(col, sq)]]; }
    }

    return arr;
}();

constexpr auto ISOLATED_MASKS = [] {
    std::array<Bitboard, SQ_N> arr{};

    for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
    {
        arr[sq] =
            (file_of(sq) != FILE_A ? bitboards::FILE_MASKS[sq + WEST] : 0ULL) | (file_of(sq) != FILE_H ? bitboards::FILE_MASKS[sq + EAST] : 0ULL);
    }

    return arr;
}();

constexpr auto PASSED_MASKS = [] {
    std::array<std::array<Bitboard, SQ_N>, 2> arr{};

    for (const auto c : {WHITE, BLACK})
    {
        for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
        {
            for (Square s2 = s1; is_valid(s2); s2 += pawn_push(c)) { arr[c][s1] |= bitboards::PAWN_ATTACKS[c][s2] | s2; }
        }
    }

    return arr;
}();

constexpr auto OUTPOST_PAWN_MASKS = [] {
    std::array<std::array<Bitboard, SQ_N>, 2> arr = PASSED_MASKS;

    for (const auto c : {WHITE, BLACK})
    {
        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) { arr[c][sq] &= ~bitboards::FILE_MASKS[sq]; }
    }

    return arr;
}();

constexpr auto ROOK_ON_PASSER_MASKS = [] {
    std::array<std::array<Bitboard, SQ_N>, 2> arr = PASSED_MASKS;

    for (const auto c : {WHITE, BLACK})
    {
        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) { arr[c][sq] ^= OUTPOST_PAWN_MASKS[c][sq]; }
    }

    return arr;
}();

constexpr auto INNER_RING = [] {
    std::array<Bitboard, SQ_N> arr{};

    for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) { arr[sq] = bitboards::get_attacks<KING>(sq) | sq; }

    return arr;
}();

constexpr auto OUTER_RING = [] {
    std::array<Bitboard, SQ_N> arr{};

    for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
    {
        Bitboard bb = bitboards::get_attacks<KING>(s1);
        while (bb) { arr[s1] |= bitboards::get_attacks<KING>(bitboards::pop_lsb(bb)); }
        arr[s1] &= ~(bitboards::get_attacks<KING>(s1) | s1);
    }

    return arr;
}();

constexpr std::array<Bitboard, 2> OUTPOST_MASKS = {bitboards::RANK_MASKS[A4] | bitboards::RANK_MASKS[A5] | bitboards::RANK_MASKS[A6],
                                                   bitboards::RANK_MASKS[A3] | bitboards::RANK_MASKS[A4] | bitboards::RANK_MASKS[A5]};

constexpr Bitboard LIGHT_MASK = 0x55aa55aa55aa55aaULL;

constexpr Bitboard DARK_MASK = 0xaa55aa55aa55aa55ULL;

constexpr std::array<Bitboard, 2> FIANCHETTO_BISHOP_MASK = {B2 | G2, B7 | G7};

constexpr std::array<Bitboard, 2> CENTER_MASK = {D5 | E5, D4 | E4};

extern std::array<std::array<int, PHASE_N>, TI_MISC> T;

template <bool TRACE> int evaluate(const Position& pos);

bool is_doubled_pawn(Bitboard bb, Square sq);

} // namespace clovis::eval
