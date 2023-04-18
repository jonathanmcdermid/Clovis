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
		string get_fen() const;
		void set(const char* fen);
		bool see_ge(Move move, int threshold) const;
		bool do_null_move();
		void undo_null_move();
		bool do_move(Move move);
		void undo_move(Move move);
		bool is_draw() const;
		void print_position() const;
		void print_bitboards();
		bool is_material_draw() const;
		Key make_key();
		Key make_pawn_key();
		void put_piece(Piece pc, Square s);
		void replace_piece(Piece pc, Square s);
		void remove_piece(Square s);

		template <bool NM> void new_board_state();
		template <Colour US> Square get_pinner(Square sq) const;
		template <Colour US> bool discovery_threat(Square sq) const;
		template <Colour US> bool is_insufficient() const;
		template <Colour US> constexpr bool is_attacked(Square sq) const;

		constexpr Square get_smallest_attacker(Bitboard attackers, Colour stm) const;
		constexpr Bitboard consider_xray(Bitboard occ, Square to, PieceType pt) const;
		constexpr Bitboard attackers_to(Square sq) const;
		constexpr bool is_king_in_check() const;
		constexpr bool stm_has_promoted() const;
		constexpr bool is_draw_50() const;
		constexpr bool is_repeat() const;
		constexpr int get_game_phase() const;

		Piece pc_table[SQ_N];
		Bitboard pc_bb[15];
		Bitboard occ_bb[COLOUR_N + 1];
		BoardState* bs;
		Colour side;
	};

	// returns the piece type of the least valuable piece on a bitboard of attackers
	constexpr Square Position::get_smallest_attacker(Bitboard attackers, const Colour stm) const {

		for (PieceType pt = PAWN; pt <= KING; ++pt)
			if (Bitboard bb = pc_bb[make_piece(pt, stm)] & attackers)
				return lsb(bb);

		return SQ_NONE;
	}

	// returns whether or not a square is attacked by opposing side
	template<Colour US>
	constexpr bool Position::is_attacked(Square sq) const {

		return ((pc_bb[make_piece(PAWN,   ~US)] & Bitboards::pawn_attacks[US][sq])
		     || (pc_bb[make_piece(KNIGHT, ~US)] & Bitboards::knight_attacks[sq])
		     || (pc_bb[make_piece(BISHOP, ~US)] & Bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq))
		     || (pc_bb[make_piece(ROOK,   ~US)] & Bitboards::get_attacks<ROOK>(occ_bb[BOTH], sq))
		     || (pc_bb[make_piece(QUEEN,  ~US)] & Bitboards::get_attacks<QUEEN>(occ_bb[BOTH], sq))
		     || (pc_bb[make_piece(KING,   ~US)] & Bitboards::king_attacks[sq]));
	}

	constexpr Bitboard Position::attackers_to(Square sq) const {

		return (Bitboards::pawn_attacks[BLACK][sq] & pc_bb[W_PAWN])
			| (Bitboards::pawn_attacks[WHITE][sq] & pc_bb[B_PAWN])
			| (Bitboards::knight_attacks[sq] & (pc_bb[W_KNIGHT] | pc_bb[B_KNIGHT]))
			| (Bitboards::king_attacks[sq] & (pc_bb[W_KING] | pc_bb[B_KING]))
			| (Bitboards::get_attacks<ROOK>(occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK]))
			| (Bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]));
	}

	constexpr bool Position::is_king_in_check() const {

		return (side == WHITE)
			? is_attacked<WHITE>(lsb(pc_bb[W_KING]))
			: is_attacked<BLACK>(lsb(pc_bb[B_KING]));
	}

	constexpr bool Position::stm_has_promoted() const {

		return bool(pc_bb[make_piece(KNIGHT, side)]
		          | pc_bb[make_piece(BISHOP, side)]
		          | pc_bb[make_piece(ROOK,   side)]
		          | pc_bb[make_piece(QUEEN,  side)]);
	}

	constexpr bool Position::is_draw_50() const {
		return (bs->hmc >= 100);
	}

	// updates a bitboard of attackers after a piece has moved to include possible x ray attackers
	constexpr Bitboard Position::consider_xray(Bitboard occ, Square to, PieceType pt) const {

		return (pt == PAWN || pt == BISHOP) ? occ & (Bitboards::get_attacks<BISHOP>(occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]))
			: pt == ROOK ? occ & (Bitboards::get_attacks<ROOK>(occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK]))
			: pt == QUEEN ? consider_xray(occ, to, BISHOP) | consider_xray(occ, to, ROOK)
			: 0ULL;
	}

	constexpr bool Position::is_repeat() const {

		BoardState* temp = bs;

		for (int end = min(bs->hmc, bs->ply_null); end >= 4; end -= 4) {

			assert(temp->prev->prev->prev->prev);
			temp = temp->prev->prev->prev->prev;

			if (temp->key == bs->key)
				return true;
		}
		return false;
	}

	constexpr int Position::get_game_phase() const {
		return min(bs->game_phase, MAX_GAMEPHASE);
	}

	template <Colour US>
	inline bool Position::is_insufficient() const {

		return (popcnt(pc_bb[make_piece(PAWN, US)]) == 0
			&& (popcnt(pc_bb[make_piece(ROOK, US)]) == 0)
			&& (popcnt(pc_bb[make_piece(QUEEN, US)]) == 0)
			&& (popcnt(pc_bb[make_piece(KNIGHT, US)]) < 3)
			&& (popcnt(pc_bb[make_piece(BISHOP, US)])
				+ popcnt(pc_bb[make_piece(KNIGHT, US)]) < 2));
	}

	inline bool Position::is_draw() const {
		return is_repeat() || is_material_draw() || is_draw_50();
	}

	inline bool Position::is_material_draw() const {
		return is_insufficient<WHITE>() && is_insufficient<BLACK>();
	}


} // namespace Clovis
