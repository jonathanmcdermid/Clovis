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
    KING_PSQT = QUEEN_PSQT + 16,
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
    {245, 306}, {270, 305}, {259, 285}, {301, 257},
    {116, 120}, {135, 112}, {165, 107}, {169, 84},
    {101, 101}, {118, 96}, {126, 93}, {144, 88},
    {83, 95}, {93, 95}, {116, 89}, {136, 88},
    {87, 86}, {102, 87}, {109, 94}, {112, 98},
    {82, 92}, {107, 90}, {103, 104}, {99, 107},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 16> KNIGHT_SOURCE = {{
    {332, 232}, {434, 238}, {410, 274}, {439, 266},
    {428, 246}, {429, 274}, {455, 275}, {464, 283},
    {435, 260}, {457, 274}, {466, 284}, {474, 294},
    {438, 271}, {447, 286}, {469, 293}, {465, 305},
}};

constexpr std::array<Score, 16> BISHOP_SOURCE = {{
    {420, 257}, {474, 258}, {454, 274}, {462, 275},
    {439, 273}, {475, 256}, {477, 271}, {474, 278},
    {441, 281}, {472, 275}, {482, 278}, {472, 284},
    {434, 282}, {441, 285}, {454, 283}, {474, 275},
}};

constexpr std::array<Score, 16> ROOK_SOURCE = {{
    {614, 517}, {609, 526}, {620, 524}, {620, 523},
    {580, 529}, {612, 521}, {630, 514}, {633, 513},
    {596, 526}, {619, 524}, {624, 519}, {631, 518},
    {592, 534}, {623, 521}, {617, 529}, {631, 521},
}};

constexpr std::array<Score, 16> QUEEN_SOURCE = {{
    {1146, 950}, {1145, 960}, {1145, 964}, {1172, 943},
    {1137, 962}, {1162, 945}, {1177, 950}, {1175, 963},
    {1156, 966}, {1171, 957}, {1161, 977}, {1157, 987},
    {1148, 978}, {1140, 998}, {1146, 977}, {1142, 994},
}};

constexpr std::array<Score, 16> KING_SOURCE = {{
    {61, 6}, {110, 28}, {60, 57}, {62, 56},
    {89, 40}, {124, 54}, {83, 77}, {46, 91},
    {43, 59}, {103, 70}, {79, 89}, {72, 98},
    {11, 64}, {89, 79}, {70, 100}, {33, 111},
}};

constexpr std::array<Score, 32> PASSED_PAWN = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {53, 136}, {27, 138}, {47, 109}, {25, 110},
    {25, 80}, {17, 79}, {9, 59}, {9, 43},
    {14, 48}, {3, 49}, {0, 28}, {0, 20},
    {6, 18}, {0, 27}, {0, 8}, {0, 5},
    {4, 18}, {0, 20}, {0, 5}, {0, 12},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 8> CANDIDATE_PASSER = {{
    {0, 0}, {0, 6}, {0, 6}, {6, 30}, {14, 59}, {21, 67}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 7> QUIET_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {8, 2}, {6, 6}, {6, 4}, {2, 8}, {0, 0},
}};

constexpr std::array<Score, 7> CAPTURE_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {15, 28}, {17, 29}, {18, 31}, {0, 30}, {0, 0},
}};

constexpr Score DOUBLE_PAWN_PENALTY = {1, 9};
constexpr Score ISOLATED_PAWN_PENALTY = {18, 7};
constexpr Score BISHOP_PAIR_BONUS = {37, 52};
constexpr Score ROOK_OPEN_FILE_BONUS = {33, 0};
constexpr Score ROOK_SEMI_OPEN_FILE_BONUS = {0, 0};
constexpr Score ROOK_CLOSED_FILE_PENALTY = {18, 6};
constexpr Score TEMPO_BONUS = {32, 18};
constexpr Score KING_OPEN_PENALTY = {40, 12};
constexpr Score KING_ADJACENT_OPEN_PENALTY = {0, 13};
constexpr Score KNIGHT_OUTPOST_BONUS = {53, 19};
constexpr Score BISHOP_OUTPOST_BONUS = {52, 0};
constexpr Score WEAK_QUEEN_PENALTY = {49, 0};
constexpr Score ROOK_ON_OUR_PASSER_FILE = {7, 15};
constexpr Score ROOK_ON_THEIR_PASSER_FILE = {4, 33};
constexpr Score TALL_PAWN_PENALTY = {16, 21};
constexpr Score FIANCHETTO_BONUS = {24, 12};
constexpr Score ROOK_ON_SEVENTH_RANK = {4, 26};

constexpr std::array<short, 32> PAWN_SHIELD = {{
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    31, 34, 33, 0,
}};

constexpr std::array<short, 7> INNER_RING_ATTACK = {{
    0, 31, 20, 34, 30, 25, 0,
}};

constexpr std::array<short, 7> OUTER_RING_ATTACK = {{
    0, 7, 32, 12, 10, 19, 0,
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
            for (const auto pt : {PAWN}) { arr[make_piece(pt, col)][sq] = PIECE_TYPE_SOURCE[pt][SOURCE_32[relative_square(col, sq)]]; }
            for (const auto pt : {KNIGHT, BISHOP, ROOK, QUEEN, KING}) { arr[make_piece(pt, col)][sq] = PIECE_TYPE_SOURCE[pt][SOURCE_16[sq]]; }
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
