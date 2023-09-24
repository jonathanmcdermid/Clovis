#pragma once

#ifdef NDEBUG
#undef NDEBUG
#endif

#define NDEBUG

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

namespace clovis {

    typedef uint64_t Key;
    typedef uint64_t Bitboard;
    typedef int64_t  Duration;

    constexpr int MAX_SCALING           = 32;
    constexpr int MAX_GAME_PHASE        = 24;
    constexpr int MAX_PLY               = 64;
    constexpr int MAX_MOVES             = 256;
    constexpr int CHECKMATE_SCORE       = 25000;
    constexpr int MIN_CHECKMATE_SCORE   = CHECKMATE_SCORE - MAX_PLY;
    constexpr int DRAW_SCORE            = 0;
    constexpr int DEFAULT_BENCH_DEPTH   = 13;
    constexpr int DEFAULT_BENCH_THREADS = 1;
    constexpr int DEFAULT_BENCH_MB      = 16;

    constexpr auto game_phase_inc =
        std::array{0, 0, 1, 1, 2, 4, 0, 0, 0, 0, 1, 1, 2, 4, 0};
    constexpr auto piece_value =
        std::array{0, 100, 300, 300, 500, 900, 20000, 0,
                   0, 100, 300, 300, 500, 900, 20000};

    /*
    MOVE BIT FORMATTING

    0000 0000 0000 0000 0011 1111   from square
    0000 0000 0000 1111 1100 0000   to square
    0000 0000 1111 0000 0000 0000   piece
    0000 1111 0000 0000 0000 0000   promoted piece
    0001 0000 0000 0000 0000 0000   capture flag
    0010 0000 0000 0000 0000 0000   double push flag
    0100 0000 0000 0000 0000 0000   en_passant flag
    1000 0000 0000 0000 0000 0000   castling flag
    */

    enum Move : int { MOVE_NONE, MOVE_NULL = 65 };

    enum Colour : int { WHITE, BLACK, COLOUR_N, BOTH = COLOUR_N };

    enum PieceType : int {
        PIECE_TYPE_NONE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        PIECE_TYPE_N = KING
    };

    enum Piece {
        NO_PIECE,
        W_PAWN = PAWN,
        W_KNIGHT,
        W_BISHOP,
        W_ROOK,
        W_QUEEN,
        W_KING,
        B_PAWN = PAWN + 8,
        B_KNIGHT,
        B_BISHOP,
        B_ROOK,
        B_QUEEN,
        B_KING,
    };

    enum MoveType : int { QUIET_MOVES, CAPTURE_MOVES, ALL_MOVES };

    enum StageType : int {
        TT_MOVE,
        INIT_CAPTURES,
        WINNING_CAPTURES,
        INIT_QUIETS,
        QUIETS,
        LOSING_CAPTURES,
        FINISHED
    };

    enum GamePhase : int { MG, EG, PHASE_N = 2 };

    enum HashFlag : uint8_t {
        HASH_NONE,
        HASH_ALPHA,
        HASH_BETA,
        HASH_EXACT,
    };

    enum NodeType : int { NODE_ROOT, NODE_PV, NODE_NON_PV, NODE_NULL };

    enum Direction : int {
        NO_DIR,
        NORTH      = 8,
        EAST       = 1,
        SOUTH      = -NORTH,
        WEST       = -EAST,
        NORTH_EAST = NORTH + EAST,
        SOUTH_EAST = SOUTH + EAST,
        SOUTH_WEST = SOUTH + WEST,
        NORTH_WEST = NORTH + WEST
    };

    enum Square : int {
        A1,
        B1,
        C1,
        D1,
        E1,
        F1,
        G1,
        H1,
        A2,
        B2,
        C2,
        D2,
        E2,
        F2,
        G2,
        H2,
        A3,
        B3,
        C3,
        D3,
        E3,
        F3,
        G3,
        H3,
        A4,
        B4,
        C4,
        D4,
        E4,
        F4,
        G4,
        H4,
        A5,
        B5,
        C5,
        D5,
        E5,
        F5,
        G5,
        H5,
        A6,
        B6,
        C6,
        D6,
        E6,
        F6,
        G6,
        H6,
        A7,
        B7,
        C7,
        D7,
        E7,
        F7,
        G7,
        H7,
        A8,
        B8,
        C8,
        D8,
        E8,
        F8,
        G8,
        H8,
        SQ_N,
        SQ_ZERO = A1,
        SQ_NONE = SQ_N
    };

    enum File : int {
        FILE_NONE = -1,
        FILE_A,
        FILE_B,
        FILE_C,
        FILE_D,
        FILE_E,
        FILE_F,
        FILE_G,
        FILE_H,
        FILE_N
    };

    enum Rank : int {
        RANK_NONE = -1,
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_N
    };

    enum TraceIndex : int {

        TI_NORMAL,

        PAWN_PSQT        = TI_NORMAL,
        KNIGHT_PSQT      = PAWN_PSQT + 32,
        BISHOP_PSQT      = KNIGHT_PSQT + 16,
        ROOK_PSQT        = BISHOP_PSQT + 16,
        QUEEN_PSQT       = ROOK_PSQT + 16,
        KING_PSQT        = QUEEN_PSQT + 32,
        PASSED_PAWN      = KING_PSQT + 16,
        CANDIDATE_PASSER = PASSED_PAWN + 32,
        QUIET_MOBILITY   = CANDIDATE_PASSER + 8,
        CAPTURE_MOBILITY = QUIET_MOBILITY + 7,
        DOUBLE_PAWN      = CAPTURE_MOBILITY + 7,
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

        SAFETY_PAWN_SHIELD      = TI_SAFETY,
        SAFETY_INNER_RING       = SAFETY_PAWN_SHIELD + 32,
        SAFETY_OUTER_RING       = SAFETY_INNER_RING + 7,
        SAFETY_VIRTUAL_MOBILITY = SAFETY_OUTER_RING + 7,

        TI_N,

        SAFETY_N_ATT = TI_N,

        TI_MISC
    };

    enum EvalType : int { NORMAL, SAFETY, EVAL_TYPE_N };

    enum CastleRights {
        NO_CASTLING,
        WHITE_KS     = 1 << 0,
        WHITE_QS     = 1 << 1,
        BLACK_KS     = 1 << 2,
        BLACK_QS     = 1 << 3,
        ALL_CASTLING = WHITE_KS | WHITE_QS | BLACK_KS | BLACK_QS,
    };

    struct Score {
        constexpr Score() = default;
        constexpr Score(const short m, const short e) : mg(m), eg(e) {}
        explicit Score(const std::array<double, PHASE_N> param)
            : mg(static_cast<short>(round(param[MG]))),
              eg(static_cast<short>(round(param[EG]))) {}
        Score &operator+=(const Score &rhs) {
            mg = static_cast<short>(mg + rhs.mg);
            eg = static_cast<short>(eg + rhs.eg);
            return *this;
        }
        Score &operator-=(const Score &rhs) {
            mg = static_cast<short>(mg - rhs.mg);
            eg = static_cast<short>(eg - rhs.eg);
            return *this;
        }
        bool operator==(const Score &rhs) const {
            return mg == rhs.mg && eg == rhs.eg;
        }
        short mg{0}, eg{0};
    };

    struct ScoredMove {
        constexpr   ScoredMove() = default;
        ScoredMove &operator=(const Move m) {
            move = m;
            return *this;
        }
        operator Move() const { return move; }

        Move move{MOVE_NONE};
        int  score{0};
    };

    constexpr Score operator-(const Score s) {
        return {static_cast<short>(-s.mg), static_cast<short>(-s.eg)};
    }
    constexpr Score operator+(const Score s1, const Score s2) {
        return {static_cast<short>(s1.mg + s2.mg),
                static_cast<short>(s1.eg + s2.eg)};
    }
    constexpr Score operator+(const Score s, const short i) {
        return {static_cast<short>(s.mg + i), static_cast<short>(s.eg + i)};
    }
    constexpr Score operator-(const Score s1, const Score s2) {
        return {static_cast<short>(s1.mg - s2.mg),
                static_cast<short>(s1.eg - s2.eg)};
    }
    constexpr Score operator-(const Score s, const short i) {
        return {static_cast<short>(s.mg - i), static_cast<short>(s.eg - i)};
    }
    constexpr Score operator*(const Score s, const short i) {
        return {static_cast<short>(s.mg * i), static_cast<short>(s.eg * i)};
    }
    constexpr Score operator*(const Score s1, const Score s2) {
        return {static_cast<short>(s1.mg * s2.mg),
                static_cast<short>(s1.eg * s2.eg)};
    }
    constexpr Score operator/(const Score s, const short i) {
        return {static_cast<short>(s.mg / i), static_cast<short>(s.eg / i)};
    }
    constexpr Score operator/(const Score s1, const Score s2) {
        return {static_cast<short>(s1.mg / s2.mg),
                static_cast<short>(s1.eg / s2.eg)};
    }
    constexpr Score operator<<(const Score s, const short i) {
        return {static_cast<short>(s.mg << i), static_cast<short>(s.eg << i)};
    }
    constexpr Square operator+(const Square sq, const Direction dir) {
        return static_cast<Square>(static_cast<int>(sq) +
                                   static_cast<int>(dir));
    }
    constexpr Square operator-(const Square sq, const Direction dir) {
        return static_cast<Square>(static_cast<int>(sq) -
                                   static_cast<int>(dir));
    }
    constexpr Square &operator+=(Square &sq, const Direction dir) {
        return sq = sq + dir;
    }
    constexpr Square &operator-=(Square &sq, const Direction dir) {
        return sq = sq - dir;
    }

    constexpr Bitboard sq_bb(const Square sq) {
        return 1ULL << sq;
    }

    constexpr Bitboard operator|(const Square sq1, const Square sq2) {
        return sq_bb(sq1) | sq_bb(sq2);
    }
    constexpr Bitboard operator&(const Bitboard bb, const Square sq) {
        return bb & sq_bb(sq);
    }
    constexpr Bitboard operator|(const Bitboard bb, const Square sq) {
        return bb | sq_bb(sq);
    }
    constexpr Bitboard operator^(const Bitboard bb, const Square sq) {
        return bb ^ sq_bb(sq);
    }
    constexpr Bitboard &operator|=(Bitboard &bb, const Square sq) {
        return bb |= sq_bb(sq);
    }
    constexpr Bitboard &operator^=(Bitboard &bb, const Square sq) {
        return bb ^= sq_bb(sq);
    }

    constexpr CastleRights ks_castle_rights(const Colour c) {
        return static_cast<CastleRights>(1 << (c << 1));
    }

    constexpr CastleRights qs_castle_rights(const Colour c) {
        return static_cast<CastleRights>(1 << ((c << 1) | 1));
    }

    constexpr CastleRights castle_rights(const Colour c) {
        return static_cast<CastleRights>(3 << (c << 1));
    }

    constexpr Move encode_move(const Square from, const Square to,
                               const Piece piece, const Piece promo,
                               const bool cap, const bool dub, const bool ep,
                               const bool cast) {
        return static_cast<Move>(from | (to << 6) | (piece << 12) |
                                 (promo << 16) | (cap << 20) | (dub << 21) |
                                 (ep << 22) | (cast << 23));
    }

    constexpr Square move_from_sq(const Move m) {
        return static_cast<Square>(m & 0x3f);
    }

    constexpr Square move_to_sq(const Move m) {
        return static_cast<Square>((m & 0xfc0) >> 6);
    }

    constexpr Piece move_piece_type(const Move m) {
        return static_cast<Piece>((m & 0xf000) >> 12);
    }

    constexpr Piece move_promotion_type(const Move m) {
        return static_cast<Piece>((m & 0xf0000) >> 16);
    }

    constexpr bool move_capture(const Move m) {
        return m & 0x100000;
    }

    constexpr bool move_double(const Move m) {
        return m & 0x200000;
    }

    constexpr bool move_en_passant(const Move m) {
        return m & 0x400000;
    }

    constexpr bool move_castling(const Move m) {
        return m & 0x800000;
    }

    constexpr Direction pawn_push(const Colour c) {
        return static_cast<Direction>((8 ^ (c * 0xffffffff)) + c);
    }

    constexpr File file_of(const Square sq) {
        return static_cast<File>(sq & 7);
    }

    constexpr Rank rank_of(const Square sq) {
        return static_cast<Rank>(sq >> 3);
    }

    constexpr Rank relative_rank(const Colour c, const Rank r) {
        return static_cast<Rank>(r ^ (c * 7));
    }

    constexpr Square relative_square(const Colour c, const Square sq) {
        return static_cast<Square>(sq ^ (c * 56));
    }

    constexpr Square make_square(const int f, const int r) {
        return static_cast<Square>((r << 3) | f);
    }

    constexpr Square make_square(const File f, const Rank r) {
        return static_cast<Square>((r << 3) | f);
    }

    constexpr Square flip_square(const Square sq) {
        return static_cast<Square>(sq ^ 56);
    }

    constexpr bool is_valid(const Square sq) {
        return !(sq & 0xffffffc0);
    }

    constexpr Colour get_side(const Piece pc) {
        return static_cast<Colour>(pc >> 3);
    }

    constexpr Piece make_piece(const PieceType pt, const Colour c) {
        return static_cast<Piece>((c << 3) | pt);
    }

    constexpr PieceType piece_type(const Piece pc) {
        return static_cast<PieceType>(pc & 7);
    }

    constexpr Colour operator~(const Colour c) {
        return static_cast<Colour>(c ^ 1);
    }

    constexpr int distance_between(const Square s1, const Square s2) {
        return std::max(abs(file_of(s2) - file_of(s1)),
                        abs(rank_of(s2) - rank_of(s1)));
    }

    constexpr bool king_side_castle(const Square to) {
        return file_of(to) == FILE_G;
    }

    constexpr void get_castle_rook_squares(const Square king_tar, Square &rf,
                                           Square &rt) {
        if (king_side_castle(king_tar)) {
            rf = king_tar + EAST;
            rt = king_tar + WEST;
        } else {
            rf = king_tar + WEST + WEST;
            rt = king_tar + EAST;
        }
    }

    // returns whether or not a direction can be moved to from a given square
    constexpr bool valid_dir(const Square sq, const Direction dir) {
        return dir == NORTH   ? rank_of(sq) != RANK_8
               : dir == SOUTH ? rank_of(sq) != RANK_1
               : dir == EAST  ? file_of(sq) != FILE_H
               : dir == WEST  ? file_of(sq) != FILE_A
                              : false;
    }

    inline Square str2sq(const std::string &s) {
        assert(s.length() == 2);
        return make_square(static_cast<File>(s[0] - 'a'),
                           static_cast<Rank>(s[1] - '1'));
    }

    constexpr char sq_names[65][3] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2",
        "d2", "e2", "f2", "g2", "h2", "a3", "b3", "c3", "d3", "e3", "f3",
        "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a5",
        "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6",
        "e6", "f6", "g6", "h6", "a7", "b7", "c7", "d7", "e7", "f7", "g7",
        "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "-"};

    inline std::string sq2str(const Square sq) {
        return sq_names[sq];
    }

    // convert move to std::string
    inline std::string move2str(const Move m) {
        return (move_promotion_type(m))
                   ? sq2str(move_from_sq(m)) + sq2str(move_to_sq(m)) +
                         " pnbrqk  pnbrqk"[move_promotion_type(m)]
                   : sq2str(move_from_sq(m)) + sq2str(move_to_sq(m));
    }

    inline std::ostream &operator<<(std::ostream &os, const Square &sq) {
        os << sq2str(sq);
        return os;
    }

    inline std::ostream &operator<<(std::ostream &os, const Score &s) {
        os << "{" << s.mg << ", " << s.eg << "}";
        return os;
    }

    inline std::ostream &operator<<(std::ostream &os, const Move &m) {
        os << move_from_sq(m) << move_to_sq(m);
        if (move_promotion_type(m))
            os << " pnbrqk  pnbrqk"[move_promotion_type(m)];
        return os;
    }

#define INCR_OPERATORS(T)                                   \
    constexpr T &operator++(T &d) {                         \
        return d = static_cast<T>(static_cast<int>(d) + 1); \
    }                                                       \
    constexpr T &operator--(T &d) {                         \
        return d = static_cast<T>(static_cast<int>(d) - 1); \
    }

#define BASE_OPERATORS(T)                                 \
    constexpr T operator-(T d) {                          \
        return static_cast<T>(-static_cast<int>(d));      \
    }                                                     \
    constexpr T operator+(T d1, int d2) {                 \
        return static_cast<T>(static_cast<int>(d1) + d2); \
    }                                                     \
    constexpr T operator-(T d1, int d2) {                 \
        return static_cast<T>(static_cast<int>(d1) - d2); \
    }                                                     \
    constexpr T &operator+=(T &d1, int d2) {              \
        return d1 = d1 + d2;                              \
    }                                                     \
    constexpr T &operator-=(T &d1, int d2) {              \
        return d1 = d1 - d2;                              \
    }

    INCR_OPERATORS(Piece)
    INCR_OPERATORS(PieceType)
    INCR_OPERATORS(File)
    INCR_OPERATORS(Rank)
    INCR_OPERATORS(Direction)
    INCR_OPERATORS(Square)

    BASE_OPERATORS(File)
    BASE_OPERATORS(Rank)
    BASE_OPERATORS(Direction)
    BASE_OPERATORS(Square)

#undef INCR_OPERATORS
#undef BASE_OPERATORS

} // namespace clovis
