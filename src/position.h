#pragma once

#include <string>
#include <memory>
#include <cassert>

#include "types.h"
#include "bitboard.h"
#include "evaluate.h"
#include "movelist.h"

namespace Clovis {

	class Position;

	struct ScoredMove;

	namespace Eval {

		int evaluate(const Position& pos);

	} // namespace Eval

	ScoredMove* gen_moves(const Position& pos, ScoredMove* ml);

	std::string sq2str(Square s);

	const extern std::string piece_str;

	// linked list implementation for board state info
	struct BoardState {
		BoardState* prev = NULL;
		Piece captured_piece = NO_PIECE;
		Square enpassant = SQ_NONE;
		int castle = 0;
		int hmc = 0;
		int fmc = 0;
	};

	class Position {
	public:
		void set(const char* fen);
		bool is_attacked(Square sq, Colour s) const;
		bool do_move(Move m, bool only_captures = false);
		void undo_move(Move m);
		void print_position();
		void print_bitboards();
		void print_attacked_squares(Colour s);
		Colour side_to_move() const;
		Piece piece_on(Square s) const;
		bool empty(Square s) const;
		Colour change_side(Colour c) const;
		bool is_king_in_check(Colour c) const;
	private:
		void put_piece(Piece pc, Square s);
		void remove_piece(Square s);
		Piece piece_board[SQ_N];
		Bitboard piece_bitboard[15];
		Bitboard occ_bitboard[COLOUR_N + 1];
		BoardState* bs;
		Colour side;
		friend ScoredMove* gen_moves(const Position& pos, ScoredMove* ml);
		friend int Eval::evaluate(const Position& pos);
	};

	inline Colour Position::side_to_move() const {
		return side;
	}

	inline Piece Position::piece_on(Square sq) const {
		assert(is_valid(sq));
		return piece_board[sq];
	}

	inline bool Position::empty(Square sq) const {
		return piece_on(sq) == NO_PIECE;
	}

	inline Colour Position::change_side(Colour c) const {
		return c == WHITE ? BLACK : WHITE;
	}

	inline bool Position::is_king_in_check(Colour c) const {
		return is_attacked(get_lsb_index(piece_bitboard[make_piece(KING, c)]), change_side(c));
	}

} // namespace Clovis