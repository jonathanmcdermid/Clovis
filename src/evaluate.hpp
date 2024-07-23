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
    {188, 248}, {185, 252}, {158, 241}, {212, 203},
    {78, 101}, {92, 108}, {113, 97}, {124, 37},
    {68, 90}, {79, 88}, {95, 78}, {105, 78},
    {61, 81}, {68, 87}, {85, 78}, {102, 75},
    {66, 75}, {74, 76}, {79, 80}, {83, 84},
    {59, 79}, {79, 82}, {75, 88}, {66, 91},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 16> KNIGHT_SOURCE = {{
    {238, 203}, {275, 214}, {264, 241}, {287, 235},
    {290, 217}, {286, 239}, {298, 242}, {305, 246},
    {284, 224}, {303, 237}, {304, 247}, {313, 256},
    {289, 239}, {297, 247}, {314, 254}, {308, 260},
}};

constexpr std::array<Score, 16> BISHOP_SOURCE = {{
    {289, 227}, {327, 218}, {301, 236}, {307, 235},
    {290, 237}, {319, 221}, {323, 233}, {315, 240},
    {303, 238}, {324, 239}, {320, 241}, {321, 243},
    {292, 244}, {299, 241}, {306, 241}, {317, 236},
}};

constexpr std::array<Score, 16> ROOK_SOURCE = {{
    {430, 433}, {428, 436}, {430, 436}, {431, 436},
    {407, 439}, {425, 434}, {444, 426}, {438, 428},
    {424, 438}, {429, 438}, {434, 436}, {440, 433},
    {419, 448}, {437, 439}, {437, 440}, {446, 434},
}};

constexpr std::array<Score, 32> QUEEN_SOURCE = {{
    {859, 819}, {848, 836}, {818, 854}, {859, 835},
    {876, 805}, {842, 852}, {855, 851}, {840, 863},
    {893, 802}, {888, 822}, {890, 830}, {864, 853},
    {883, 826}, {872, 844}, {870, 838}, {865, 848},
    {896, 798}, {890, 822}, {890, 819}, {888, 839},
    {888, 799}, {898, 808}, {893, 824}, {891, 833},
    {888, 788}, {913, 760}, {909, 788}, {905, 805},
    {891, 773}, {885, 775}, {884, 784}, {901, 779},
}};

constexpr std::array<Score, 16> KING_SOURCE = {{
    {79, 11}, {107, 35}, {74, 55}, {66, 59},
    {89, 44}, {109, 55}, {84, 70}, {56, 82},
    {35, 66}, {88, 67}, {66, 85}, {64, 92},
    {34, 66}, {76, 82}, {78, 92}, {40, 102},
}};

constexpr std::array<Score, 32> PASSED_PAWN = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {44, 111}, {34, 105}, {41, 85}, {25, 120},
    {35, 61}, {26, 64}, {11, 53}, {6, 38},
    {12, 42}, {6, 45}, {0, 27}, {0, 20},
    {0, 9}, {0, 22}, {0, 12}, {0, 0},
    {2, 16}, {0, 12}, {0, 2}, {0, 2},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 8> CANDIDATE_PASSER = {{
    {0, 0}, {0, 5}, {0, 13}, {9, 26}, {9, 56}, {14, 59}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 7> QUIET_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {5, 1}, {4, 4}, {3, 3}, {2, 2}, {0, 0},
}};

constexpr std::array<Score, 7> CAPTURE_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {5, 25}, {14, 21}, {11, 27}, {2, 15}, {0, 0},
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
    29, 37, 28, 0,
}};

constexpr std::array<short, 7> INNER_RING_ATTACK = {{
    0, 16, 23, 25, 23, 21, 0,
}};

constexpr std::array<short, 7> OUTER_RING_ATTACK = {{
    0, 0, 28, 16, 10, 19, 0,
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
