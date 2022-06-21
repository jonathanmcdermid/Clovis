#pragma once

#include <string>
#include <memory>
#include <cassert>
#include <sstream>
#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "movelist.h"
#include "random.h"

namespace Clovis {

	class Position;

	struct ScoredMove;
	struct Score;

	namespace Eval {

		int evaluate(const Position& pos);
		Score evaluate_pawns(const Position& pos);

	} // namespace Eval

	ScoredMove* gen_moves(const Position& pos, ScoredMove* ml);

	std::string sq2str(Square s);

	const extern std::string piece_str;

	// linked list implementation for board state info
	struct BoardState {
		BoardState* prev = NULL;
		Piece captured_piece = NO_PIECE;
		Square enpassant = SQ_NONE;
		Key key;
		Key pkey;
		int castle = 0;
		int hmc = 0;
		int fmc = 0;
		int ply_null = 0;
		int game_phase = 0;
	};

	class Position {
	public:
		static void init();
		void set(const char* fen);
		Key get_key() const { return bs->key; }
		Key get_pawn_key() const { return bs->pkey; }
		bool is_attacked(Square sq, Colour s) const;
		Bitboard attackers_to(Square sq, Bitboard occupied) const;
		bool see(Move m, int threshold) const;
		bool do_move(Move m, bool only_captures = false);
		void undo_move(Move m);
		bool is_repeat() const;
		void print_position() const;
		void print_bitboards();
		void print_attacked_squares(Colour s);
		Colour side_to_move() const;
		Piece piece_on(Square s) const;
		bool empty(Square s) const;
		bool is_king_in_check(Colour c) const;
		void change() { side = other_side(side); }
		bool has_promoted(Colour c) const;
		bool is_material_draw() const;
		bool is_insufficient(Colour c) const;
		bool is_draw_50() const;
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
		friend ScoredMove* gen_moves(const Position& pos, ScoredMove* ml);
		friend ScoredMove* gen_cap_moves(const Position& pos, ScoredMove* ml);
		friend ScoredMove* gen_quiet_moves(const Position& pos, ScoredMove* ml);
		friend int Eval::evaluate(const Position& pos);
		friend Score Eval::evaluate_pawns(const Position& pos);
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

	inline bool Position::is_king_in_check(Colour c) const {
		return is_attacked(get_lsb_index(piece_bitboard[make_piece(KING, c)]), other_side(c));
	}

	inline bool Position::has_promoted(Colour c) const {
		return bool(
			piece_bitboard[make_piece(KNIGHT, c)] |
			piece_bitboard[make_piece(BISHOP, c)] |
			piece_bitboard[make_piece(ROOK, c)] |
			piece_bitboard[make_piece(QUEEN, c)]);
	}

	inline bool Position::is_material_draw() const {
		return is_insufficient(WHITE) && is_insufficient(BLACK);
	}

	inline bool Position::is_insufficient(Colour c) const {
		return 
			(count_bits(piece_bitboard[make_piece(PAWN, c)]) == 0 &&
			count_bits(piece_bitboard[make_piece(QUEEN, c)]) == 0 &&
			count_bits(piece_bitboard[make_piece(ROOK, c)]) == 0 &&
			(count_bits(piece_bitboard[make_piece(KNIGHT, c)]) < 3) &&
			(count_bits(piece_bitboard[make_piece(BISHOP, c)]) + count_bits(piece_bitboard[make_piece(KNIGHT, c)]) < 2));
	}

	inline bool Position::is_draw_50() const {
		return (bs->hmc >= 100);
	}

} // namespace Clovis