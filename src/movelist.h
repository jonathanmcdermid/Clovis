#pragma once

#include <iostream>

#include "types.h"
#include "position.h"
#include "bitboard.h"

namespace Clovis {

    constexpr Bitboard castle_occ_ks[COLOUR_N] = { 96ULL, 6917529027641081856ULL };
    constexpr Bitboard castle_occ_qs[COLOUR_N] = { 14ULL, 1008806316530991104ULL };

	class Position;

	struct ScoredMove {
		Move m = MOVE_NONE;
		int score = 0;
		void operator=(Move m) { this->m = m; }
		bool operator==(Move m) { return this->m == m; }
		bool operator!=(Move m) { return this->m != m; }
		operator Move() const { return m; }
	};

	inline static bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
		return lhs.score > rhs.score;
	}

	template<typename T> T* gen_moves(const Position& pos, T* ml);
	template<typename T> T* gen_cap_moves(const Position& pos, T* ml);
	template<typename T> T* gen_quiet_moves(const Position& pos, T* ml);

	namespace MoveGen {

		struct MoveList {
			MoveList(const Position& pos) : last(gen_moves<Move>(pos, moves)) {}
			int size() const { return (last - moves); }
			void print();
			const Move* begin() const { return moves; }
			const Move* end() const { return last; }
		private:
			Move moves[MAX_MOVES], *last;
		};

	} // namespace MoveGen

    // gen all pseudo-legal moves for a position
    template<typename T> 
    T* gen_moves(const Position& pos, T* ml)
    {
        Colour us = pos.side, them = other_side(pos.side);
        Bitboard bb = pos.piece_bitboard[make_piece(PAWN, us)];
        Square king_origin = us == WHITE ? E1 : E8;
        Rank promo_rank = relative_rank(us, RANK_7);
        Rank spawn_rank = relative_rank(us, RANK_2);
        Direction push = pawn_push(us);

        // repeat until all friendly pawns are popped
        while (bb)
        {
            Square src = pop_lsb(bb);
            Square tar = src + push;

            if (is_valid(tar) && !(pos.occ_bitboard[BOTH] & tar))
            {
                // promotion
                if (rank_of(src) == promo_rank)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, us), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, us), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, us), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, us), 0, 0, 0, 0);
                }
                else
                {
                    // single push
                    *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    // double push
                    if (rank_of(src) == spawn_rank && !(pos.occ_bitboard[BOTH] & (tar + push)))
                        *ml++ = encode_move(src, tar + push, pos.piece_board[src], NO_PIECE, 0, 1, 0, 0);
                }
            }

            Bitboard att = Bitboards::pawn_attacks[us][src] & pos.occ_bitboard[them];

            while (att)
            {
                tar = pop_lsb(att);

                if (rank_of(src) == promo_rank)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, us), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, us), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, us), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, us), 1, 0, 0, 0);
                }
                else
                    *ml++ = encode_move(src, tar, make_piece(PAWN, us), NO_PIECE, 1, 0, 0, 0);
            }

            if (pos.bs->enpassant != SQ_NONE)
            {
                Bitboard enpassant_attacks = Bitboards::pawn_attacks[us][src] & pos.bs->enpassant;

                if (enpassant_attacks)
                    *ml++ = encode_move(src, pop_lsb(enpassant_attacks), pos.piece_board[src], NO_PIECE, 1, 0, 1, 0);
            }
        }

        if (!pos.is_attacked(king_origin, them))
        {
            if (pos.bs->castle & (1 << (us * 2))
                && !(pos.occ_bitboard[BOTH] & castle_occ_ks[us])
                && !pos.is_attacked(king_origin + EAST, them))
                *ml++ = encode_move(king_origin, king_origin + 2 * EAST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);

            if (pos.bs->castle & (1 << (us * 2 + 1))
                && !(pos.occ_bitboard[BOTH] & castle_occ_qs[us])
                && !pos.is_attacked(king_origin + WEST, them))
                *ml++ = encode_move(king_origin, king_origin + 2 * WEST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt)
        {
            bb = pos.piece_bitboard[make_piece(pt, us)];
            while (bb)
            {
                Square src = pop_lsb(bb);
                Bitboard att = ~pos.occ_bitboard[us] & Bitboards::get_attacks(pos.occ_bitboard[BOTH], src, pt);

                while (att)
                {
                    Square tar = pop_lsb(att);

                    if (!(pos.occ_bitboard[them] & tar))
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    else
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 0, 0);
                }
            }
        }

        return ml;
    }

    // gen all pseudo-legal capture moves for a position
    template<typename T>
    T* gen_cap_moves(const Position& pos, T* ml)
    {
        Colour us = pos.side, them = other_side(pos.side);
        Bitboard bb = pos.piece_bitboard[make_piece(PAWN, us)];
        Direction push = pawn_push(us);
        Rank promo_rank = relative_rank(us, RANK_7);

        // repeat until all friendly pawns are popped
        while (bb)
        {
            Square src = pop_lsb(bb);
            Bitboard att = Bitboards::pawn_attacks[us][src] & pos.occ_bitboard[them];

            while (att)
            {
                Square tar = pop_lsb(att);

                if (rank_of(src) == promo_rank)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, us), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, us), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, us), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, us), 1, 0, 0, 0);
                }
                else
                    *ml++ = encode_move(src, tar, make_piece(PAWN, us), NO_PIECE, 1, 0, 0, 0);
            }

            if (pos.bs->enpassant != SQ_NONE)
            {
                Bitboard enpassant_attacks = Bitboards::pawn_attacks[us][src] & pos.bs->enpassant;

                if (enpassant_attacks)
                    *ml++ = encode_move(src, pop_lsb(enpassant_attacks), pos.piece_board[src], NO_PIECE, 1, 0, 1, 0);
            }
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt)
        {
            bb = pos.piece_bitboard[make_piece(pt, us)];
            while (bb)
            {
                Square src = pop_lsb(bb);
                Bitboard att = ~pos.occ_bitboard[us] & Bitboards::get_attacks(pos.occ_bitboard[BOTH], src, pt);

                while (att)
                {
                    Square tar = pop_lsb(att);

                    if (pos.occ_bitboard[them] & tar)
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 0, 0);
                }
            }
        }

        return ml;
    }

    // gen all pseudo-legal quiet moves for a position
    template<typename T>
    T* gen_quiet_moves(const Position& pos, T* ml)
    {
        Colour us = pos.side, them = other_side(pos.side);
        Bitboard bb = pos.piece_bitboard[make_piece(PAWN, us)];
        Square king_origin = us == WHITE ? E1 : E8;
        Direction push = pawn_push(us);
        Rank promo_rank = relative_rank(us, RANK_7);
        Rank spawn_rank = relative_rank(us, RANK_2);

        // repeat until all friendly pawns are popped
        while (bb)
        {
            Square src = pop_lsb(bb);
            Square tar = src + push;

            if (is_valid(tar) && !(pos.occ_bitboard[BOTH] & tar))
            {
                // promotion
                if (rank_of(src) == promo_rank)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, us), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, us), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, us), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, us), 0, 0, 0, 0);
                }
                else
                {
                    // single push
                    *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    // double push
                    if (rank_of(src) == spawn_rank && !(pos.occ_bitboard[BOTH] & tar + push))
                        *ml++ = encode_move(src, tar + push, pos.piece_board[src], NO_PIECE, 0, 1, 0, 0);
                }
            }
        }

        if (!pos.is_attacked(king_origin, them))
        {
            if (pos.bs->castle & (1 << (us * 2))
                && !(pos.occ_bitboard[BOTH] & castle_occ_ks[us])
                && !pos.is_attacked(king_origin + EAST, them))
                *ml++ = encode_move(king_origin, king_origin + 2 * EAST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);

            if (pos.bs->castle & (1 << (us * 2 + 1))
                && !(pos.occ_bitboard[BOTH] & castle_occ_qs[us])
                && !pos.is_attacked(king_origin + WEST, them))
                *ml++ = encode_move(king_origin, king_origin + 2 * WEST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt)
        {
            bb = pos.piece_bitboard[make_piece(pt, us)];
            while (bb)
            {
                Square src = pop_lsb(bb);
                Bitboard att = ~pos.occ_bitboard[us] & Bitboards::get_attacks(pos.occ_bitboard[BOTH], src, pt);

                while (att)
                {
                    Square tar = pop_lsb(att);

                    if (!(pos.occ_bitboard[them] & tar))
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                }
            }
        }

        return ml;
    }

} // namsepace Clovis