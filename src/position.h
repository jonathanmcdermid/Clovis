#pragma once

#include <string>
#include <memory>
#include <cassert>
#include <sstream>
#include <iostream>
#include <optional>

#include "types.h"
#include "bitboard.h"
#include "random.h"

namespace Clovis {

	const extern std::string piece_str;

	namespace Zobrist {

		void init_zobrist();

		extern Key piece_square[15][SQ_N], enpassant[SQ_N], castling[16], side;

	}

	// linked list implementation for board state info
	struct BoardState {
		constexpr BoardState() = default;

		BoardState* prev{ nullptr };
		Piece captured_piece{ NO_PIECE };
		Square enpassant{ SQ_NONE };
		Key key{ 0ULL }, pkey{ 0ULL };
		int castle{ 0 }, hmc{ 0 }, fmc{ 0 }, ply_null{ 0 }, game_phase{ 0 };
	};

	struct Position {
		Position(const char* fen) { set(fen); }
		std::string get_fen() const;
		void set(const char* fen);
		bool see_ge(Move move, int threshold) const;
		void do_null_move();
		void undo_null_move();
		bool do_move(Move move);
		void undo_move(Move move);
		void print_position() const;
		void print_bitboards() const;
		Key make_key() const;
		Key make_pawn_key() const;
		void put_piece(Piece pc, Square s);
		void replace_piece(Piece pc, Square s);
		void remove_piece(Square s);
		std::optional<Square> get_smallest_attacker(Bitboard attackers, Colour stm) const;
		bool is_repeat() const;

		template <bool NM> void new_board_state();
		template <Colour US> std::optional<Square> get_pinner(Square sq) const;
		template <Colour US> bool discovery_threat(Square sq) const;
		template <Colour US> bool is_insufficient() const;
		template <Colour US> bool is_attacked(Square sq) const;

		Bitboard consider_xray(Bitboard occ, Square to, PieceType pt) const;
		Bitboard attackers_to(Square sq) const;
		int get_game_phase() const;
		bool is_king_in_check() const;
		bool stm_has_promoted() const;
		bool is_draw_50() const;
		bool is_draw() const;
		bool is_material_draw() const;

		std::array<Piece, SQ_N> pc_table;
		std::array<Bitboard, 15> pc_bb;
		std::array<Bitboard, COLOUR_N + 1> occ_bb;
		BoardState* bs;
		Colour side;
	};

	// returns whether or not a square is attacked by opposing side
	template<Colour US>
	inline bool Position::is_attacked(Square sq) const {

		return ((pc_bb[make_piece(PAWN,   ~US)] & Bitboards::pawn_attacks[US][sq])
		     || (pc_bb[make_piece(KNIGHT, ~US)] & Bitboards::knight_attacks[sq])
		     || (pc_bb[make_piece(BISHOP, ~US)] & Bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq))
		     || (pc_bb[make_piece(ROOK,   ~US)] & Bitboards::get_attacks<ROOK>(occ_bb[BOTH], sq))
		     || (pc_bb[make_piece(QUEEN,  ~US)] & Bitboards::get_attacks<QUEEN>(occ_bb[BOTH], sq))
		     || (pc_bb[make_piece(KING,   ~US)] & Bitboards::king_attacks[sq]));
	}

	template <Colour US>
	inline bool Position::is_insufficient() const {

		return (std::popcount(pc_bb[make_piece(PAWN,   US)]) == 0
			&& (std::popcount(pc_bb[make_piece(ROOK,   US)]) == 0)
			&& (std::popcount(pc_bb[make_piece(QUEEN,  US)]) == 0)
			&& (std::popcount(pc_bb[make_piece(KNIGHT, US)]) < 3)
			&& (std::popcount(pc_bb[make_piece(BISHOP, US)])
			  + std::popcount(pc_bb[make_piece(KNIGHT, US)]) < 2));
	}

	// updates a bitboard of attackers after a piece has moved to include possible x ray attackers
	inline Bitboard Position::consider_xray(Bitboard occ, Square to, PieceType pt) const {

		return (pt == PAWN || pt == BISHOP) ? occ & (Bitboards::get_attacks<BISHOP>(occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]))
			: pt == ROOK ? occ & (Bitboards::get_attacks<ROOK>(occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK]))
			: pt == QUEEN ? consider_xray(occ, to, BISHOP) | consider_xray(occ, to, ROOK)
			: 0ULL;
	}

	inline Bitboard Position::attackers_to(Square sq) const {

		return (Bitboards::pawn_attacks[BLACK][sq] &  pc_bb[W_PAWN])
			 | (Bitboards::pawn_attacks[WHITE][sq] &  pc_bb[B_PAWN])
			 | (Bitboards::knight_attacks[sq]      & (pc_bb[W_KNIGHT] | pc_bb[B_KNIGHT]))
			 | (Bitboards::king_attacks[sq]        & (pc_bb[W_KING]   | pc_bb[B_KING]))
			 | (Bitboards::get_attacks<ROOK>  (occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK]   | pc_bb[B_ROOK]))
			 | (Bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]));
	}

	inline int Position::get_game_phase() const {
		return std::min(bs->game_phase, MAX_GAMEPHASE);
	}

	inline bool Position::is_king_in_check() const {

		return (side == WHITE)
			? is_attacked<WHITE>(lsb(pc_bb[W_KING]))
			: is_attacked<BLACK>(lsb(pc_bb[B_KING]));
	}

	inline bool Position::stm_has_promoted() const {

		return pc_bb[make_piece(KNIGHT, side)]
		     | pc_bb[make_piece(BISHOP, side)]
		     | pc_bb[make_piece(ROOK,   side)]
		     | pc_bb[make_piece(QUEEN,  side)];
	}

	inline bool Position::is_draw_50() const {
		return (bs->hmc >= 100);
	}

	inline bool Position::is_draw() const {
		return is_repeat() || is_material_draw() || is_draw_50();
	}

	inline bool Position::is_material_draw() const {
		return is_insufficient<WHITE>() && is_insufficient<BLACK>();
	}


} // namespace Clovis
