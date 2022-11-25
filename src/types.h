#pragma once

#ifdef NDEBUG 
#undef NDEBUG
#endif

#define NDEBUG

#include <assert.h>
#include <climits>
#include <string>

using namespace std;

namespace Clovis {

    typedef unsigned int U32;
    typedef unsigned long long U64;

    typedef U64 Key;
    typedef U64 Bitboard;

    constexpr int MAX_GAMEPHASE = 24;
    constexpr int MAX_PLY = 64;
    constexpr int MAX_MOVES = 256;
    constexpr int CHECKMATE_SCORE = 25000;
    constexpr int MIN_CHECKMATE_SCORE = CHECKMATE_SCORE - MAX_PLY;
    constexpr int DRAW_SCORE = 0;

    /*
                    MOVE BIT FORMATTING

    binary                                              hexidecimal

    0000 0000 0000 0000 0011 1111   from square         0x3f
    0000 0000 0000 1111 1100 0000   to square           0xfc0
    0000 0000 1111 0000 0000 0000   piece               0xf000
    0000 1111 0000 0000 0000 0000   promoted piece      0xf0000
    0001 0000 0000 0000 0000 0000   capture flag        0x100000
    0010 0000 0000 0000 0000 0000   double push flag    0x200000
    0100 0000 0000 0000 0000 0000   enpassant flag      0x400000
    1000 0000 0000 0000 0000 0000   castling flag       0x800000
    */

    enum Move : int {
        MOVE_NONE,
        MOVE_NULL = 65
    };

    enum Colour : int {
        WHITE, BLACK,
        BOTH,
        COLOUR_N = 2
    };

    enum PieceType : int {
        PIECETYPE_NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
        PIECETYPE_N = 6
    };

    enum MoveType : int {
        QUIET_MOVES, CAPTURE_MOVES, ALL_MOVES
    };

    enum StageType : int {
        TT_MOVE,
        INIT_CAPTURES,
        WINNING_CAPTURES,
        INIT_QUIETS,
        QUIETS,
        LOSING_CAPTURES,
        FINISHED
    };

    enum GamePhase : int {
        MG, EG,
        PHASE_N = 2
    };

    enum HashFlag : uint8_t {
        HASH_NONE,
        HASH_ALPHA,
        HASH_BETA,
        HASH_EXACT,
    };

    inline void operator++(PieceType& pt) {
        pt = static_cast<PieceType>(static_cast<int>(pt) + 1);
    }

    enum Piece {
        NO_PIECE,
        W_PAWN = PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
        B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    };

    enum Direction : int {
        NORTH = 8,
        EAST = 1,
        SOUTH = -NORTH,
        WEST = -EAST,
        NORTH_EAST = NORTH + EAST,
        SOUTH_EAST = SOUTH + EAST,
        SOUTH_WEST = SOUTH + WEST,
        NORTH_WEST = NORTH + WEST
    };

    enum Square : int {
        A1, B1, C1, D1, E1, F1, G1, H1,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A8, B8, C8, D8, E8, F8, G8, H8,
        SQ_NONE,
        SQ_ZERO = 0,
        SQ_N = 64
    };

    inline void operator++(Square& sq) {
        sq = static_cast<Square>(static_cast<int>(sq) + 1);
    }

    inline Square operator+(Square& sq, int i) {
        return static_cast<Square>(static_cast<int>(sq) + i);
    }

    constexpr Square operator+(Square sq, Direction dir) { return Square(int(sq) + int(dir)); }
    constexpr Square operator-(Square sq, Direction dir) { return Square(int(sq) - int(dir)); }
    inline Square& operator+=(Square& sq, Direction dir) { return sq = sq + dir; }
    inline Square& operator-=(Square& sq, Direction dir) { return sq = sq - dir; }

    enum File : int {
        FILE_NONE = -1,
        FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_N
    };

    inline void operator++(File& f) {
        f = static_cast<File>(static_cast<int>(f) + 1);
    }

    inline void operator--(File& f) {
        f = static_cast<File>(static_cast<int>(f) - 1);
    }

    inline File operator+(File& f, int i) {
        return static_cast<File>(static_cast<int>(f) + i);
    }

    inline File operator-(File& f, int i) {
        return static_cast<File>(static_cast<int>(f) - i);
    }

    enum Rank : int {
        RANK_NONE = -1,
        RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_N
    };

    inline void operator++(Rank& r) {
        r = static_cast<Rank>(static_cast<int>(r) + 1);
    }

    inline void operator--(Rank& r) {
        r = static_cast<Rank>(static_cast<int>(r) - 1);
    }

    inline Rank operator+(Rank& r, int i) {
        return static_cast<Rank>(static_cast<int>(r) + i);
    }

    inline Rank operator-(Rank& r, int i) {
        return static_cast<Rank>(static_cast<int>(r) - i);
    }

    enum CastleRights {
        NO_CASTLING,
        WHITE_KS = 1 << 0,
        WHITE_QS = 1 << 1,
        BLACK_KS = 1 << 2,
        BLACK_QS = 1 << 3
    };

    constexpr Move encode_move(Square from, Square to, Piece piece, Piece promo, int cap, int dpush, int enpassant, int castling) {
        return Move(from | (to << 6) | (piece << 12) | (promo << 16) | (cap << 20) | (dpush << 21) | (enpassant << 22) | (castling << 23));
    }

    constexpr Square move_from_sq(Move m) {
        return Square(m & 0x3f);
    }

    constexpr Square move_to_sq(Move m) {
        return Square((m & 0xfc0) >> 6);
    }

    constexpr Piece move_piece_type(Move m) {
        return Piece((m & 0xf000) >> 12);
    }

    constexpr Piece move_promotion_type(Move m) {
        return Piece((m & 0xf0000) >> 16);
    }

    constexpr bool move_capture(Move m) {
        return m & 0x100000;
    }

    constexpr bool move_double(Move m) {
        return m & 0x200000;
    }

    constexpr bool move_enpassant(Move m) {
        return m & 0x400000;
    }

    constexpr bool move_castling(Move m) {
        return m & 0x800000;
    }

    constexpr Direction pawn_push(Colour c) {
        return Direction((8 ^ (c * 0xffffffff)) + c);
    }

    constexpr File file_of(Square sq) {
        return File(sq & 7);
    }

    constexpr Rank rank_of(Square sq) {
        return Rank(sq >> 3);
    }

    constexpr Rank relative_rank(Colour c, Rank r) {
        return Rank(r ^ (c * 7));
    }

    constexpr Square relative_square(Colour c, Square sq) {
        return Square(sq ^ (c * 56));
    }

    constexpr Square make_square(File f, Rank r) {
        return Square((r << 3) + f);
    }

    constexpr Square flip_square(Square sq) {
        return Square(sq ^ 56);
    }

    constexpr bool is_valid(Square sq) {
        return !(sq & 0xffffffc0);
    }

    constexpr Colour get_side(Piece pc) {
        return Colour(pc >> 3);
    }

    constexpr Colour other_side(Colour c) {
        return Colour(c ^ 1);
    }

    constexpr Piece make_piece(PieceType pt, Colour c) {
        return Piece((c << 3) + pt);
    }

    constexpr PieceType piece_type(Piece pc) {
        return PieceType(pc & 7);
    }

    constexpr bool king_side_castle(Square to) {
        return file_of(to) == FILE_G;
    }

    constexpr void get_castle_rook_squares(Square king_tar, Square& rf, Square& rt)
    {
        if (king_side_castle(king_tar))
        {
            rf = king_tar + EAST;
            rt = king_tar + WEST;
        }
        else
        {
            rf = king_tar + 2 * WEST;
            rt = king_tar + EAST;
        }
    }

    // convert square to string
    inline string sq2str(Square sq) {
        return string{ char('a' + file_of(sq)), char('1' + rank_of(sq)) };
    }

    // convert move to string
    inline string move2str(Move m)
    {
        return (move_promotion_type(m))
            ? sq2str(move_from_sq(m)) + sq2str(move_to_sq(m)) + " pnbrqk  pnbrqk"[move_promotion_type(m)]
            : sq2str(move_from_sq(m)) + sq2str(move_to_sq(m));
    }

} // namespace Clovis