#pragma once

#include <iostream>

#include "types.h"
#include "position.h"
#include "bitboard.h"

namespace Clovis {

    constexpr Bitboard castle_occ_ks[COLOUR_N] = { 96ULL, 6917529027641081856ULL };
    constexpr Bitboard castle_occ_qs[COLOUR_N] = { 14ULL, 1008806316530991104ULL };

	struct Position;

	struct ScoredMove {
		Move move = MOVE_NONE;
		int score = 0;
		void operator=(Move move) { this->move = move; }
		operator Move() const { return move; }
	};

	inline bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
		return lhs.score > rhs.score;
	}

	namespace MoveGen {

		template<typename T, MoveType M> T* gen_moves(const Position& pos, T* moves);

		struct MoveList {
			MoveList(const Position& pos) : last(gen_moves<Move, ALL_MOVES>(pos, moves)) {}
			int size() const { return (last - moves); }
			void print();
			const Move* begin() const { return moves; }
			const Move* end() const { return last; }
		private:
			Move moves[MAX_MOVES], *last;
		};

		template<typename T, MoveType M, PieceType P>
		T* gen_majors(const Position& pos, T* moves)
		{
			constexpr bool quiets   = M != CAPTURE_MOVES;
			constexpr bool captures = M != QUIET_MOVES;

			Colour us = pos.side;
			Colour them = other_side(us);

			Bitboard bb = pos.piece_bitboard[make_piece(P, us)];

            while (bb)
            {
                Square src = pop_lsb(bb);
                Bitboard att = ~pos.occ_bitboard[us] & Bitboards::get_attacks<P>(pos.occ_bitboard[BOTH], src);

                while (att)
                {
                    Square tar = pop_lsb(att);

                    if (quiets && !(pos.occ_bitboard[them] & tar))
                        *moves++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    if (captures && (pos.occ_bitboard[them] & tar))
                        *moves++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 0, 0);
                }
            }

			return moves;
		}

        // gen pseudo-legal moves for a position
        template<typename T, MoveType M>
        T* gen_moves(const Position& pos, T* moves)
        {
            constexpr bool quiets   = M != CAPTURE_MOVES;
            constexpr bool captures = M != QUIET_MOVES;

            Colour us = pos.side, them = other_side(pos.side);
            Bitboard bb = pos.piece_bitboard[make_piece(PAWN, us)];
            Square king_origin = relative_square(us, E1);
            Rank promo_rank = relative_rank(us, RANK_7);
            Rank spawn_rank = relative_rank(us, RANK_2);
            Direction push = pawn_push(us);

            while (bb)
            {
                Square src = pop_lsb(bb);
                Square tar = src + push;

                if (quiets && is_valid(tar) && !(pos.occ_bitboard[BOTH] & tar))
                {
                    // promotion
                    if (rank_of(src) == promo_rank)
                    {
                        *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, us), 0, 0, 0, 0);
                        *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, us), 0, 0, 0, 0);
                        *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, us), 0, 0, 0, 0);
                        *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, us), 0, 0, 0, 0);
                    }
                    else
                    {
                        // single push
                        *moves++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                        // double push
                        if (rank_of(src) == spawn_rank && !(pos.occ_bitboard[BOTH] & (tar + push)))
                            *moves++ = encode_move(src, tar + push, pos.piece_board[src], NO_PIECE, 0, 1, 0, 0);
                    }
                }

                if (captures)
                {
                    Bitboard att = Bitboards::pawn_attacks[us][src] & pos.occ_bitboard[them];

                    while (att)
                    {
                        tar = pop_lsb(att);

                        if (rank_of(src) == promo_rank)
                        {
                            *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, us), 1, 0, 0, 0);
                            *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, us), 1, 0, 0, 0);
                            *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, us), 1, 0, 0, 0);
                            *moves++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, us), 1, 0, 0, 0);
                        }
                        else
                            *moves++ = encode_move(src, tar, make_piece(PAWN, us), NO_PIECE, 1, 0, 0, 0);
                    }

                    if (pos.bs->enpassant != SQ_NONE)
                    {
                        Bitboard enpassant_attacks = Bitboards::pawn_attacks[us][src] & pos.bs->enpassant;

                        if (enpassant_attacks)
                            *moves++ = encode_move(src, pop_lsb(enpassant_attacks), pos.piece_board[src], NO_PIECE, 1, 0, 1, 0);
                    }
                }
            }

            if (quiets && !pos.is_attacked(king_origin, them))
            {
                if (pos.bs->castle & (1 << (us * 2))
                    && !(pos.occ_bitboard[BOTH] & castle_occ_ks[us])
                    && !pos.is_attacked(king_origin + EAST, them))
                    *moves++ = encode_move(king_origin, king_origin + 2 * EAST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);

                if (pos.bs->castle & (1 << (us * 2 + 1))
                    && !(pos.occ_bitboard[BOTH] & castle_occ_qs[us])
                    && !pos.is_attacked(king_origin + WEST, them))
                    *moves++ = encode_move(king_origin, king_origin + 2 * WEST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);
            }

			moves = gen_majors<T, M, KNIGHT>(pos, moves);
			moves = gen_majors<T, M, BISHOP>(pos, moves);
			moves = gen_majors<T, M, ROOK>  (pos, moves);
			moves = gen_majors<T, M, QUEEN> (pos, moves);
			moves = gen_majors<T, M, KING>  (pos, moves);

            return moves;
        }

	} // namespace MoveGen

} // namsepace Clovis
