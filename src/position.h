#pragma once

#include <string>
#include <memory>
#include <cassert>
#include <sstream>
#include <iostream>

#include "types.h"
#include "bitboard.h"
#include "random.h"

using namespace std;

namespace Clovis {

	const extern string piece_str;

	namespace Zobrist {

		void init_zobrist();

        extern Key piece_square[15][SQ_N];
        extern Key enpassant[SQ_N];
        extern Key castling[16];
        extern Key side;

	}

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

	struct Position {
		Position(const char* fen) { set(fen); }
		void set(const char* fen);
		bool is_attacked(Square sq, Colour side) const;
		Bitboard attackers_to(Square sq) const;
		Square get_smallest_attacker(Bitboard attackers, Colour stm) const;
		Bitboard consider_xray(Bitboard occ, Square to, PieceType pt) const;
		bool see_ge(Move move, int threshold) const;
		bool do_move(Move move);
		void undo_move(Move move);
		bool is_repeat() const;
		void print_position() const;
		void print_bitboards();
		bool is_king_in_check(Colour side) const;
		bool stm_has_promoted() const;
		bool is_material_draw() const;
		template <Colour c> bool is_insufficient() const;
		bool is_draw_50() const;
		int get_game_phase() const { return min(bs->game_phase, MAX_GAMEPHASE); }
		Key make_key();
		Key make_pawn_key();
		void put_piece(Piece pc, Square s);
		void remove_piece(Square s);
		Piece pc_table[SQ_N];
		Bitboard pc_bb[15];
		Bitboard occ_bb[COLOUR_N + 1];
		BoardState* bs;
		Colour side;
	};

	inline bool Position::is_king_in_check(Colour c) const {
		return is_attacked(lsb(pc_bb[make_piece(KING, c)]), other_side(c));
	}

	inline bool Position::stm_has_promoted() const {
		return bool(
			  pc_bb[make_piece(KNIGHT, side)]
			| pc_bb[make_piece(BISHOP, side)]
			| pc_bb[make_piece(ROOK,   side)]
			| pc_bb[make_piece(QUEEN,  side)]);
	}

	inline bool Position::is_material_draw() const {
		return is_insufficient<WHITE>() && is_insufficient<BLACK>();
	}

	inline bool Position::is_draw_50() const {
		return (bs->hmc >= 100);
	}

	template <Colour US>
	inline bool Position::is_insufficient() const {
		return (popcnt(pc_bb[make_piece(PAWN,   US)]) == 0
			&& (popcnt(pc_bb[make_piece(ROOK,   US)]) == 0)
			&& (popcnt(pc_bb[make_piece(QUEEN,  US)]) == 0)
			&& (popcnt(pc_bb[make_piece(KNIGHT, US)])  < 3)
			&& (popcnt(pc_bb[make_piece(BISHOP, US)])
			  + popcnt(pc_bb[make_piece(KNIGHT, US)])  < 2));
	}

} // namespace Clovis
