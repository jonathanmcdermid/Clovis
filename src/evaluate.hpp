#pragma once

#include "position.hpp"
#include "tt.hpp"

namespace clovis::eval {

enum EvalType : int
{
    NORMAL,
    SAFETY,
    EVAL_TYPE_N
};

enum TraceIndex : int
{
    TI_NORMAL,

    PAWN_PSQT = TI_NORMAL,
    KNIGHT_PSQT = PAWN_PSQT + 32,
    BISHOP_PSQT = KNIGHT_PSQT + 16,
    ROOK_PSQT = BISHOP_PSQT + 16,
    QUEEN_PSQT = ROOK_PSQT + 16,
    KING_PSQT = QUEEN_PSQT + 32,
    PASSED_PAWN_PSQT = KING_PSQT + 16,
    CANDIDATE_PASSER_PSQT = PASSED_PAWN_PSQT + 32,
    QUIET_MOBILITY = CANDIDATE_PASSER_PSQT + 8,
    CAPTURE_MOBILITY = QUIET_MOBILITY + 7,
    DOUBLE_PAWN = CAPTURE_MOBILITY + 7,
    ISOLATED_PAWN,
    BISHOP_PAIR,
    ROOK_FULL,
    ROOK_SEMI,
    ROOK_CLOSED,
    TEMPO,
    KING_OPEN,
    KING_ADJ_OPEN,
    KNIGHT_OUTPOST,
    BISHOP_OUTPOST,
    WEAK_QUEEN,
    ROOK_OUR_PASSER,
    ROOK_THEIR_PASSER,
    TALL_PAWN,
    FIANCHETTO,
    ROOK_ON_SEVENTH,

    TI_SAFETY,

    SAFETY_PAWN_SHIELD = TI_SAFETY,
    SAFETY_INNER_RING = SAFETY_PAWN_SHIELD + 32,
    SAFETY_OUTER_RING = SAFETY_INNER_RING + 7,
    SAFETY_VIRTUAL_MOBILITY = SAFETY_OUTER_RING + 7,

    TI_N,

    SAFETY_N_ATT = TI_N,

    TI_MISC
};

struct EvalInfo : PTEntry
{
    constexpr EvalInfo() = default;
    explicit EvalInfo(const PTEntry& pte) : PTEntry(pte) {}

    std::array<int, 2> n_att{0, 0};
};

// clang-format off

constexpr std::array<Score, 32> PAWN_SOURCE = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {141, 284}, {161, 273}, {169, 245}, {186, 230},
    {80, 92}, {99, 87}, {119, 83}, {115, 67},
    {66, 81}, {84, 76}, {86, 76}, {103, 67},
    {55, 74}, {64, 74}, {78, 72}, {97, 67},
    {61, 68}, {72, 69}, {76, 73}, {80, 76},
    {56, 69}, {78, 69}, {72, 81}, {73, 80},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};
constexpr std::array<Score, 16> KNIGHT_SOURCE = {{
    {238, 197}, {318, 201}, {300, 228}, {321, 226},
    {309, 211}, {304, 233}, {334, 231}, {338, 239},
    {319, 219}, {338, 231}, {340, 239}, {344, 247},
    {321, 229}, {323, 243}, {337, 250}, {335, 257},
}};
constexpr std::array<Score, 16> BISHOP_SOURCE = {{
    {316, 232}, {329, 238}, {333, 236}, {332, 240},
    {317, 234}, {350, 224}, {347, 236}, {347, 239},
    {324, 242}, {345, 238}, {354, 239}, {344, 245},
    {320, 243}, {321, 244}, {332, 244}, {342, 242},
}};
constexpr std::array<Score, 16> ROOK_SOURCE = {{
    {445, 441}, {437, 448}, {447, 446}, {450, 443},
    {419, 448}, {436, 444}, {446, 441}, {456, 436},
    {423, 445}, {441, 445}, {445, 440}, {444, 441},
    {420, 451}, {434, 445}, {442, 448}, {446, 442},
}};
constexpr std::array<Score, 32> QUEEN_SOURCE = {{
    {882, 814}, {861, 842}, {858, 856}, {888, 832},
    {884, 809}, {843, 839}, {855, 848}, {828, 875},
    {895, 811}, {880, 829}, {881, 833}, {854, 866},
    {876, 841}, {866, 854}, {869, 842}, {861, 854},
    {891, 817}, {880, 846}, {886, 834}, {883, 847},
    {889, 828}, {908, 806}, {896, 830}, {896, 826},
    {887, 809}, {903, 796}, {915, 798}, {907, 817},
    {903, 790}, {893, 795}, {894, 799}, {908, 790},
}};
constexpr std::array<Score, 16> KING_SOURCE = {{
    {74, 17}, {107, 35}, {65, 60}, {72, 56},
    {84, 44}, {113, 53}, {79, 75}, {51, 87},
    {56, 59}, {97, 69}, {79, 84}, {68, 92},
    {24, 63}, {73, 77}, {58, 94}, {38, 103},
}};
constexpr std::array<Score, 32> PASSED_PAWN = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {13, 125}, {2, 121}, {17, 93}, {13, 88},
    {13, 64}, {7, 63}, {11, 42}, {5, 35},
    {10, 37}, {0, 35}, {0, 20}, {0, 18},
    {4, 14}, {0, 16}, {0, 4}, {0, 3},
    {0, 14}, {0, 12}, {5, 0}, {0, 4},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};
constexpr std::array<Score, 8> CANDIDATE_PASSER = {{
    {0, 0}, {0, 2}, {0, 5}, {5, 18}, {12, 39}, {26, 49}, {0, 0}, {0, 0},
}};
constexpr std::array<Score, 7> QUIET_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {5, 1}, {4, 3}, {4, 2}, {2, 4}, {0, 0},
}};
constexpr std::array<Score, 7> CAPTURE_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {11, 20}, {11, 21}, {12, 22}, {0, 14}, {0, 0},
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
    31, 33, 32, 0,
}};
constexpr std::array<short, 7> INNER_RING_ATTACK = {{
    0, 23, 14, 29, 28, 23, 0,
}};
constexpr std::array<short, 7> OUTER_RING_ATTACK = {{
    0, 2, 25, 7, 7, 19, 0,
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

extern std::array<std::array<int, 2>, TI_MISC> T;

bool is_open_file(Bitboard pawns, File f);
bool is_doubled_pawn(Bitboard bb, Square sq);
bool is_isolated_pawn(Bitboard bb, Square sq);

template <Colour US> bool is_passed_pawn(Bitboard bb, Square sq);
template <Colour US> bool is_candidate_passer(const Position& pos, Square sq);
template <Colour US> bool is_outpost(Square sq, const EvalInfo& ei);
template <Colour US> bool is_fianchetto(const Position& pos, Square sq);
template <Colour US> bool is_tall_pawn(const Position& pos, Square sq);

template <bool TRACE> int evaluate(const Position& pos);

} // namespace clovis::eval
