#pragma once

#include <string>
#include <memory>
#include <cassert>
#include <sstream>
#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "random.h"

namespace Clovis {

	class Position;
	struct ScoredMove;
	struct Score;

	namespace Eval {

		void test_eval();
		Score evaluate_all(const Position& pos);
		Score evaluate_pawns(const Position& pos);

	} // namespace Eval

	namespace MoveGen {

		template<typename T, MoveType mt> T* gen_moves(const Position& pos, T* moves);

	} // namespace MoveGen

	const extern std::string piece_str;

	// linked list implementation for board state info
	struct BoardState {
		BoardState* prev = NULL;
		Piece captured_piece = NO_PIECE;
		Square enpassant = SQ_NONE;
		Key key = 0ULL;
		Key pkey = 0ULL;
		int castle = 0;
		int hmc = 0;
		int fmc = 0;
		int ply_null = 0;
		int game_phase = 0;
	};

	class Position {
	public:
		Position(const char* fen) { set(fen); }
		static void init_position();
		void set(const char* fen);
		Key get_key() const { return bs->key; }
		Key get_pawn_key() const { return bs->pkey; }
		bool is_attacked(Square sq, Colour side) const;
		Bitboard attackers_to(Square sq, Bitboard occupied) const;
		PieceType get_smallest_attacker(Bitboard attackers, Colour stm) const;
		Bitboard consider_xray(Bitboard occ, Square to, PieceType pt) const;
		bool see(Move move) const;
		bool do_move(Move move);
		void undo_move(Move move);
		bool is_repeat() const;
		void print_position() const;
		void print_bitboards();
		void print_attacked_squares(Colour side);
		Colour stm() const;
		Piece piece_on(Square sq) const;
		bool empty(Square sq) const;
		bool is_king_in_check(Colour side) const;
		void change() { side = other_side(side); }
		bool stm_has_promoted() const;
		bool is_material_draw() const;
		template <Colour c> bool is_insufficient() const;
		bool is_draw_50() const;
		bool move_is_ok(Move move) const;
		int get_game_phase() const { return std::min(bs->game_phase, MAX_GAMEPHASE); }
	private:
		Key make_key();
		Key make_pawn_key();
		void put_piece(Piece pc, Square s);
		void remove_piece(Square s);
		Piece piece_board[SQ_N];
		Bitboard piece_bitboard[15];
		Bitboard occ_bitboard[COLOUR_N + 1];
		BoardState* bs;
		Colour side;
		template<typename T, MoveType mt> friend T* MoveGen::gen_moves(const Position& pos, T* moves);
		friend void Eval::test_eval();
		friend Score Eval::evaluate_all(const Position& pos);
		friend Score Eval::evaluate_pawns(const Position& pos);
	};

	inline Colour Position::stm() const {
		return side;
	}

	inline Piece Position::piece_on(Square sq) const {
		assert(is_valid(sq));
		return piece_board[sq];
	}

	inline bool Position::empty(Square sq) const {
		return piece_on(sq) == NO_PIECE;
	}

	inline bool Position::is_king_in_check(Colour side) const {
		return is_attacked(lsb(piece_bitboard[make_piece(KING, side)]), other_side(side));
	}

	inline bool Position::stm_has_promoted() const {
		return bool(piece_bitboard[make_piece(KNIGHT, side)]
			| piece_bitboard[make_piece(BISHOP, side)]
			| piece_bitboard[make_piece(ROOK, side)]
			| piece_bitboard[make_piece(QUEEN, side)]);
	}
	inline bool Position::is_material_draw() const {
		return is_insufficient<WHITE>() && is_insufficient<BLACK>();
	}

	inline bool Position::is_draw_50() const {
		return (bs->hmc >= 100);
	}

	template <Colour c>
	bool Position::is_insufficient() const {
		return (popcnt(piece_bitboard[c == WHITE ? W_PAWN : B_PAWN]) == 0
			&& popcnt(piece_bitboard[c == WHITE ? W_QUEEN : B_QUEEN]) == 0
			&& popcnt(piece_bitboard[c == WHITE ? W_ROOK : B_ROOK]) == 0
			&& (popcnt(piece_bitboard[c == WHITE ? W_KNIGHT : B_KNIGHT]) < 3)
			&& (popcnt(piece_bitboard[c == WHITE ? W_BISHOP : B_BISHOP])
				+ popcnt(piece_bitboard[c == WHITE ? W_KNIGHT : B_KNIGHT]) < 2));
	}

} // namespace Clovis
