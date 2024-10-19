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
    {225, 242}, {128, 246}, {252, 208}, {247, 196},
    {75, 113}, {82, 100}, {134, 113}, {121, 102},
    {61, 90}, {84, 90}, {94, 74}, {106, 78},
    {69, 84}, {73, 88}, {83, 80}, {100, 73},
    {69, 85}, {88, 77}, {73, 83}, {79, 76},
    {65, 80}, {81, 84}, {70, 93}, {60, 86},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 16> KNIGHT_SOURCE = {{
    {156, 187}, {230, 210}, {254, 196}, {266, 201},
    {238, 178}, {232, 225}, {238, 219}, {254, 224},
    {234, 217}, {252, 214}, {249, 223}, {249, 229},
    {232, 214}, {248, 225}, {260, 230}, {248, 240},
}};

constexpr std::array<Score, 16> BISHOP_SOURCE = {{
    {246, 202}, {262, 206}, {265, 216}, {276, 209},
    {278, 214}, {282, 206}, {294, 209}, {282, 224},
    {270, 226}, {300, 226}, {291, 213}, {291, 220},
    {267, 222}, {270, 219}, {277, 222}, {291, 212},
}};

constexpr std::array<Score, 16> ROOK_SOURCE = {{
    {390, 393}, {384, 390}, {395, 389}, {401, 387},
    {376, 393}, {387, 388}, {403, 388}, {406, 383},
    {373, 408}, {378, 403}, {385, 405}, {403, 393},
    {386, 406}, {382, 404}, {403, 398}, {405, 395},
}};

constexpr std::array<Score, 32> QUEEN_SOURCE = {{
    {869, 768}, {902, 764}, {868, 795}, {867, 798},
    {844, 790}, {803, 833}, {841, 824}, {846, 821},
    {863, 769}, {867, 781}, {859, 808}, {834, 841},
    {889, 744}, {854, 804}, {855, 820}, {848, 823},
    {868, 763}, {886, 753}, {873, 792}, {858, 806},
    {842, 769}, {890, 757}, {870, 783}, {875, 754},
    {860, 773}, {913, 698}, {894, 738}, {886, 740},
    {857, 714}, {863, 729}, {878, 727}, {871, 745},
}};

constexpr std::array<Score, 16> KING_SOURCE = {{
    {74, 4}, {103, 38}, {72, 59}, {69, 59},
    {81, 43}, {96, 60}, {75, 78}, {74, 81},
    {30, 55}, {112, 63}, {89, 81}, {83, 90},
    {54, 54}, {89, 81}, {59, 95}, {0, 113},
}};

constexpr std::array<Score, 32> PASSED_PAWN = {{
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
    {54, 94}, {60, 103}, {65, 57}, {71, 39},
    {55, 60}, {27, 68}, {0, 46}, {18, 39},
    {20, 38}, {6, 41}, {0, 16}, {0, 17},
    {0, 14}, {13, 26}, {0, 17}, {0, 15},
    {0, 8}, {0, 19}, {0, 0}, {0, 18},
    {0, 0}, {0, 0}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 8> CANDIDATE_PASSER = {{
    {0, 0}, {0, 0}, {0, 9}, {5, 33}, {20, 58}, {0, 32}, {0, 0}, {0, 0},
}};

constexpr std::array<Score, 7> QUIET_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {6, 3}, {3, 6}, {3, 3}, {3, 0}, {0, 0},
}};

constexpr std::array<Score, 7> CAPTURE_MOBILITY_BONUS = {{
    {0, 0}, {0, 0}, {6, 26}, {9, 31}, {16, 27}, {4, 8}, {0, 0},
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
    8, 48, 46, 0,
}};

constexpr std::array<short, 7> INNER_RING_ATTACK = {{
    0, 0, 18, 34, 29, 20, 0,
}};

constexpr std::array<short, 7> OUTER_RING_ATTACK = {{
    0, 0, 36, 14, 12, 13, 0,
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
