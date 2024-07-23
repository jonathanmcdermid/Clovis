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
    {164, 255}, {163, 259}, {176, 228}, {204, 216},
    {71, 101}, {91, 100}, {115, 91}, {128, 37},
    {62, 84}, {81, 82}, {86, 79}, {103, 70},
    {55, 74}, {67, 83}, {79, 76}, {101, 72},
    {62, 70}, {72, 73}, {75, 78}, {82, 81},
    {54, 74}, {75, 76}, {71, 86}, {66, 86},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 16> KNIGHT_SOURCE = {{
    {231, 190}, {289, 205}, {277, 236}, {295, 234},
    {293, 219}, {288, 238}, {308, 242}, {315, 247},
    {295, 225}, {317, 236}, {315, 247}, {322, 252},
    {299, 240}, {308, 248}, {318, 254}, {315, 261},
}};

constexpr std::array<Score, 16> BISHOP_SOURCE = {{
    {292, 230}, {325, 233}, {310, 237}, {312, 240},
    {296, 237}, {329, 220}, {332, 237}, {324, 244},
    {308, 241}, {336, 240}, {329, 241}, {328, 246},
    {302, 246}, {308, 244}, {316, 245}, {325, 240},
}};

constexpr std::array<Score, 16> ROOK_SOURCE = {{
    {440, 437}, {438, 443}, {439, 442}, {439, 442},
    {415, 443}, {434, 441}, {453, 431}, {450, 432},
    {430, 442}, {435, 442}, {446, 436}, {447, 437},
    {421, 451}, {444, 440}, {441, 442}, {453, 436},
}};

constexpr std::array<Score, 32> QUEEN_SOURCE = {{
    {857, 817}, {849, 840}, {820, 866}, {875, 833},
    {878, 807}, {845, 840}, {856, 850}, {834, 869},
    {887, 803}, {890, 817}, {891, 832}, {867, 850},
    {879, 827}, {869, 848}, {870, 840}, {863, 853},
    {897, 807}, {895, 815}, {890, 823}, {888, 844},
    {884, 813}, {901, 811}, {899, 817}, {892, 839},
    {879, 803}, {914, 765}, {912, 785}, {907, 805},
    {890, 779}, {884, 787}, {877, 801}, {904, 775},
}};

constexpr std::array<Score, 16> KING_SOURCE = {{
    {81, 8}, {106, 33}, {70, 55}, {67, 58},
    {95, 43}, {112, 54}, {74, 74}, {45, 87},
    {37, 63}, {102, 66}, {75, 84}, {54, 95},
    {31, 64}, {76, 82}, {78, 92}, {40, 104},
}};

constexpr std::array<Score, 32> PASSED_PAWN = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {36, 112}, {39, 102}, {24, 89}, {11, 117},
    {31, 64}, {18, 64}, {18, 44}, {2, 40},
    {7, 43}, {0, 37}, {0, 26}, {0, 16},
    {3, 14}, {0, 14}, {0, 9}, {0, 4},
    {0, 17}, {0, 13}, {0, 1}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 8> CANDIDATE_PASSER = {{
    {0, 0}, {0, 6}, {0, 6}, {14, 21}, {12, 47}, {13, 58}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 7> QUIET_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {4, 1}, {4, 4}, {3, 3}, {1, 3}, {0, 0},
}};

constexpr std::array<Score, 7> CAPTURE_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {7, 23}, {12, 21}, {11, 25}, {1, 14}, {0, 0},
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
    30, 34, 31, 0,
}};

constexpr std::array<short, 7> INNER_RING_ATTACK = {{
    0, 21, 18, 27, 23, 22, 0,
}};

constexpr std::array<short, 7> OUTER_RING_ATTACK = {{
    0, 0, 29, 14, 7, 18, 0,
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
