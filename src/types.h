#pragma once

#ifdef NDEBUG 
#undef NDEBUG
#endif

#define NDEBUG

#include <assert.h>
#include <climits>
#include <string>
#include <iostream>
#include <cmath>

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

using namespace std;

namespace Clovis {

	typedef unsigned int U32;
	typedef unsigned long long U64;

	typedef U64 Key;
	typedef U64 Bitboard;

	constexpr int MAX_SCALING = 32;
	constexpr int MAX_GAMEPHASE = 24;
	constexpr int MAX_PLY = 64;
	constexpr int MAX_MOVES = 256;
	constexpr int CHECKMATE_SCORE = 25000;
	constexpr int MIN_CHECKMATE_SCORE = CHECKMATE_SCORE - MAX_PLY;
	constexpr int DRAW_SCORE = 0;
	constexpr int DEFAULT_BENCH_MB = 16;
	constexpr int DEFAULT_BENCH_DEPTH = 13;
	constexpr int DEFAULT_BENCH_THREADS = 1;

	constexpr int game_phase_inc[15] = { 0, 0, 1, 1, 2, 4, 0, 0, 0, 0, 1, 1, 2, 4, 0 };
	constexpr int piece_value[15] = { 0, 100, 300, 300, 500, 900, 20000, 0, 0, 100, 300, 300, 500, 900, 20000 };

	struct Score {
	public:
		Score() : mg(0), eg(0) {}
		Score(double* param) : mg((short) round(param[0])), eg((short) round(param[1])) {}
		constexpr Score(int m, int e) : mg(m), eg(e) {}
		void operator+=(const Score& rhs) {
			this->mg += rhs.mg;
			this->eg += rhs.eg;
		}
		void operator-=(const Score& rhs) {
			this->mg -= rhs.mg;
			this->eg -= rhs.eg;
		}
		bool operator==(const Score& rhs) {
			return this->mg == rhs.mg && this->eg == rhs.eg;
		}
		short mg;
		short eg;
	};

	inline Score operator-(Score s)            { return Score(-s.mg, -s.eg); }
	inline Score operator+(Score s1, Score s2) { return Score(s1.mg + s2.mg, s1.eg + s2.eg); }
	inline Score operator+(Score s, int i)     { return Score(s.mg + i, s.eg + i); }
	inline Score operator-(Score s1, Score s2) { return Score(s1.mg - s2.mg, s1.eg - s2.eg); }
	inline Score operator-(Score s, int i)     { return Score(s.mg - i, s.eg - i); }
	inline Score operator*(Score s, int i)     { return Score(s.mg * i, s.eg * i); }
	inline Score operator*(Score s1, Score s2) { return Score(s1.mg * s2.mg, s1.eg * s2.eg); }
	inline Score operator/(Score s, int i)     { return Score(s.mg / i, s.eg / i); }
	inline Score operator/(Score s1, Score s2) { return Score(s1.mg / s2.mg, s1.eg / s2.eg); }
	inline Score operator<<(Score s, int i)    { return Score(s.mg << i, s.eg << i); }

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

	enum Piece {
		NO_PIECE,
		W_PAWN = PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
		B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
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

	enum NodeType : int {
		NODE_ROOT, NODE_PV, NODE_NON_PV
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

	constexpr Square operator+(Square sq, Direction dir) { return Square(int(sq) + int(dir)); }
	constexpr Square operator-(Square sq, Direction dir) { return Square(int(sq) - int(dir)); }
	inline Square& operator+=(Square& sq, Direction dir) { return sq = sq + dir; }
	inline Square& operator-=(Square& sq, Direction dir) { return sq = sq - dir; }

	enum File : int {
		FILE_NONE = -1,
		FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_N
	};

	enum Rank : int {
		RANK_NONE = -1,
		RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_N
	};
	
	enum TraceIndex : int {
	
		TI_NORMAL,
		
		PAWN_PSQT = TI_NORMAL,
		KNIGHT_PSQT = PAWN_PSQT + 32,
		BISHOP_PSQT = KNIGHT_PSQT + 32,
		ROOK_PSQT = BISHOP_PSQT + 16,
		QUEEN_PSQT = ROOK_PSQT + 32,
		KING_PSQT = QUEEN_PSQT + 32,
		PASSED_PAWN = KING_PSQT + 16,
		MOBILITY = PASSED_PAWN + 32,
		DOUBLE_PAWN = MOBILITY + 7,
		ISOLATED_PAWN,
		BISHOP_PAIR,
		ROOK_FULL,
		ROOK_SEMI,
		ROOK_CLOSED,
		TEMPO,
		KING_FULL,
		KING_SEMI,
		KING_ADJ_FULL,
		KING_ADJ_SEMI,
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
		
		TI_N
	};
	
	enum  EvalType : int {
		NORMAL, 
		SAFETY, EVALTYPE_N
	};

	enum CastleRights {
		NO_CASTLING,
		WHITE_KS = 1 << 0,
		WHITE_QS = 1 << 1,
		BLACK_KS = 1 << 2,
		BLACK_QS = 1 << 3
	};

	constexpr CastleRights ks_castle_rights(Colour c) {
		return CastleRights(1 << (c << 1));
	}

	constexpr CastleRights qs_castle_rights(Colour c) {
		return CastleRights(1 << ((c << 1) + 1));
	}

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
			rf = king_tar + WEST + WEST;
			rt = king_tar + EAST;
		}
	}

	constexpr char sq_names[64][3] = {
		"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
		"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
		"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
		"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
		"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
		"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
		"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
		"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
	};

	inline string sq2str(Square sq) {
		return sq_names[sq];
	}

	// convert move to string
	inline string move2str(Move m)
	{
		return (move_promotion_type(m))
		? sq2str(move_from_sq(m)) + sq2str(move_to_sq(m)) + " pnbrqk  pnbrqk"[move_promotion_type(m)]
		: sq2str(move_from_sq(m)) + sq2str(move_to_sq(m));
	}

	inline ostream& operator<<(ostream& os, const Square& sq)
	{
		os << sq2str(sq);
		return os;
	}

	inline ostream& operator<<(ostream& os, const Score& s)
	{
		os << "S(" << s.mg << ", " << s.eg << ")";
		return os;
	}

	inline ostream& operator<<(ostream& os, const Move& m)
	{
		os << move_from_sq(m) << move_to_sq(m); 
		if (move_promotion_type(m))
			os << " pnbrqk  pnbrqk"[move_promotion_type(m)];
		return os;
	}

	constexpr Colour operator~(Colour c) { return Colour(c ^ 1); }
    
#define INCR_OPERATORS(T)											\
inline T& operator++(T& d) { return d = T(int(d) + 1); }			\
inline T& operator--(T& d) { return d = T(int(d) - 1); }

#define BASE_OPERATORS(T)											\
constexpr T operator+(T d1, int d2) { return T(int(d1) + d2); }		\
constexpr T operator-(T d1, int d2) { return T(int(d1) - d2); }		\
constexpr T operator-(T d) { return T(-int(d)); }					\
inline T& operator+=(T& d1, int d2) { return d1 = d1 + d2; }		\
inline T& operator-=(T& d1, int d2) { return d1 = d1 - d2; }					

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

} // namespace Clovis
