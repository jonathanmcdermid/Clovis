#pragma once

#include <iostream>

#include "types.h"
#include "position.h"
#include "bitboard.h"

namespace Clovis {

	struct Position;

	struct ScoredMove {
		Move move = MOVE_NONE;
		int score = 0;
		void operator=(Move move) { this->move = move; }
		operator Move() const { return move; }
	};

	constexpr bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
		return lhs.score > rhs.score;
	}

	namespace MoveGen {

		template<typename T, MoveType M> T* generate(const Position& pos, T* moves);

		struct MoveList {
			MoveList(const Position& pos) : last(generate<Move, ALL_MOVES>(pos, moves)) {}
			int size() const { return (last - moves); }
			void print();
			const Move* begin() const { return moves; }
			const Move* end() const { return last; }
		private:
			Move moves[MAX_MOVES], *last;
		};

		template<typename T, MoveType M, PieceType P, Colour US>
		T* generate_majors(const Position& pos, T* moves)
		{
			constexpr bool QUIETS   = M != CAPTURE_MOVES;
			constexpr bool CAPTURES = M != QUIET_MOVES;
			constexpr Colour THEM = other_side(US);
            constexpr Piece PIECE = make_piece(P, US);

			Bitboard bb = pos.piece_bitboard[PIECE];

            Bitboard our_inv_occ = ~pos.occ_bitboard[US];

            while (bb)
            {
                Square src = pop_lsb(bb);
                Bitboard att = our_inv_occ & Bitboards::get_attacks<P>(pos.occ_bitboard[BOTH], src);

                while (att)
                {
                    Square tar = pop_lsb(att);

                    if (QUIETS && !(pos.occ_bitboard[THEM] & tar))
                        *moves++ = encode_move(src, tar, PIECE, NO_PIECE, 0, 0, 0, 0);
                    if (CAPTURES && (pos.occ_bitboard[THEM] & tar))
                        *moves++ = encode_move(src, tar, PIECE, NO_PIECE, 1, 0, 0, 0);
                }
            }

			return moves;
		}

        // gen pseudo-legal moves for a position
        template<typename T, MoveType M, Colour US>
        T* generate_all(const Position& pos, T* moves)
        {
            constexpr bool QUIETS   = M != CAPTURE_MOVES;
            constexpr bool CAPTURES = M != QUIET_MOVES;
            constexpr Colour THEM = other_side(US);
            constexpr Piece OUR_PAWN = make_piece(PAWN, US);
            constexpr Piece OUR_KNIGHT = make_piece(KNIGHT, US);
            constexpr Piece OUR_BISHOP = make_piece(BISHOP, US);
            constexpr Piece OUR_ROOK = make_piece(ROOK, US);
            constexpr Piece OUR_QUEEN = make_piece(QUEEN, US);
            constexpr Piece OUR_KING = make_piece(KING, US);
            constexpr Square KING_ORIGIN = relative_square(US, E1);
            constexpr Rank PROMO_RANK = relative_rank(US, RANK_7);
            constexpr Rank SPAWN_RANK = relative_rank(US, RANK_2);
            constexpr Direction PUSH = pawn_push(US);
            constexpr CastleRights KS_CASTLE = ks_castle_rights(US);
            constexpr CastleRights QS_CASTLE = qs_castle_rights(US);
            constexpr Bitboard KS_CASTLE_OCC = relative_square(US, F1) | relative_square(US, G1);
            constexpr Bitboard QS_CASTLE_OCC = relative_square(US, B1) | relative_square(US, C1) | relative_square(US, D1);

            Bitboard bb = pos.piece_bitboard[OUR_PAWN];

            while (bb)
            {
                Square src = pop_lsb(bb);
                Square tar = src + PUSH;

                if (QUIETS && is_valid(tar) && !(pos.occ_bitboard[BOTH] & tar))
                {
                    // promotion
                    if (rank_of(src) == PROMO_RANK)
                    {
                        *moves++ = encode_move(src, tar, OUR_PAWN, OUR_KNIGHT, 0, 0, 0, 0);
                        *moves++ = encode_move(src, tar, OUR_PAWN, OUR_BISHOP, 0, 0, 0, 0);
                        *moves++ = encode_move(src, tar, OUR_PAWN, OUR_ROOK, 0, 0, 0, 0);
                        *moves++ = encode_move(src, tar, OUR_PAWN, OUR_QUEEN, 0, 0, 0, 0);
                    }
                    else
                    {
                        // single push
                        *moves++ = encode_move(src, tar, OUR_PAWN, NO_PIECE, 0, 0, 0, 0);
                        // double push
                        if (rank_of(src) == SPAWN_RANK && !(pos.occ_bitboard[BOTH] & (tar + PUSH)))
                            *moves++ = encode_move(src, tar + PUSH, OUR_PAWN, NO_PIECE, 0, 1, 0, 0);
                    }
                }

                if (CAPTURES)
                {
                    Bitboard att = Bitboards::pawn_attacks[US][src] & pos.occ_bitboard[THEM];

                    while (att)
                    {
                        tar = pop_lsb(att);

                        if (rank_of(src) == PROMO_RANK)
                        {
                            *moves++ = encode_move(src, tar, OUR_PAWN, OUR_KNIGHT, 1, 0, 0, 0);
                            *moves++ = encode_move(src, tar, OUR_PAWN, OUR_BISHOP, 1, 0, 0, 0);
                            *moves++ = encode_move(src, tar, OUR_PAWN, OUR_ROOK, 1, 0, 0, 0);
                            *moves++ = encode_move(src, tar, OUR_PAWN, OUR_QUEEN, 1, 0, 0, 0);
                        }
                        else
                            *moves++ = encode_move(src, tar, OUR_PAWN, NO_PIECE, 1, 0, 0, 0);
                    }

                    if (pos.bs->enpassant != SQ_NONE && Bitboards::pawn_attacks[US][src] & pos.bs->enpassant)
                            *moves++ = encode_move(src, pos.bs->enpassant, OUR_PAWN, NO_PIECE, 1, 0, 1, 0);
                }
            }

            if (QUIETS && !pos.is_attacked(KING_ORIGIN, THEM))
            {
                if (pos.bs->castle & KS_CASTLE
                    && !(pos.occ_bitboard[BOTH] & KS_CASTLE_OCC)
                    && !pos.is_attacked(KING_ORIGIN + EAST, THEM))
                    *moves++ = encode_move(KING_ORIGIN, KING_ORIGIN + EAST + EAST, OUR_KING, NO_PIECE, 0, 0, 0, 1);

                if (pos.bs->castle & QS_CASTLE
                    && !(pos.occ_bitboard[BOTH] & QS_CASTLE_OCC)
                    && !pos.is_attacked(KING_ORIGIN + WEST, THEM))
                    *moves++ = encode_move(KING_ORIGIN, KING_ORIGIN + WEST + WEST, OUR_KING, NO_PIECE, 0, 0, 0, 1);
            }

			moves = generate_majors<T, M, KNIGHT,   US>(pos, moves);
			moves = generate_majors<T, M, BISHOP,   US>(pos, moves);
			moves = generate_majors<T, M, ROOK,     US>(pos, moves);
			moves = generate_majors<T, M, QUEEN,    US>(pos, moves);
			moves = generate_majors<T, M, KING,     US>(pos, moves);

            return moves;
        }

        template<typename T, MoveType M> 
        T* generate(const Position& pos, T* moves)
        {
            return (pos.side == WHITE)
                ? generate_all<T, M, WHITE>(pos, moves)
                : generate_all<T, M, BLACK>(pos, moves);
        }

	} // namespace MoveGen

} // namsepace Clovis
