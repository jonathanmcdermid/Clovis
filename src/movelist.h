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
		void operator=(Move m) { this->move = m; }
		operator Move() const { return move; }
	};

	constexpr bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
		return lhs.score > rhs.score;
	}

	namespace MoveGen {

		template<typename T, MoveType M> inline T* generate(const Position& pos, T* moves);

		struct MoveList {
			MoveList(const Position& pos) : last(generate<Move, ALL_MOVES>(pos, moves)) {}
			int size() const { return (last - moves); }
			const Move* begin() const { return moves; }
			const Move* end() const { return last; }
		private:
			Move moves[MAX_MOVES], *last;
		};

		template<typename T, MoveType M, PieceType PT, Colour US>
		T* generate_majors(const Position& pos, T* moves)
		{
			constexpr Colour THEM = other_side(US);

            constexpr Piece PIECE = make_piece(PT, US);

			Bitboard bb = pos.pc_bb[PIECE];

			Bitboard tar_bb = M == ALL_MOVES   ? ~pos.occ_bb[US] 
				            : M == QUIET_MOVES ? ~pos.occ_bb[BOTH]
				                               :  pos.occ_bb[THEM];

            while (bb)
            {
                Square src = pop_lsb(bb);
                Bitboard att = Bitboards::get_attacks<PT>(pos.occ_bb[BOTH], src) & tar_bb;

                while (att)
                {
                    Square tar = pop_lsb(att);

                    *moves++ = encode_move(src, tar, PIECE, NO_PIECE, bool(pos.occ_bb[BOTH] & tar), 0, 0, 0);
                }
            }

			return moves;
		}

		template<typename T, MoveType M, Colour US, int TC>
		T* generate_promotions(T* moves, Square src, Square tar)
		{
            constexpr bool CAPTURES = M != QUIET_MOVES;
			constexpr bool QUIETS	= M != CAPTURE_MOVES;

			constexpr Piece OUR_PAWN   = make_piece(PAWN,   US);
            constexpr Piece OUR_KNIGHT = make_piece(KNIGHT, US);
            constexpr Piece OUR_BISHOP = make_piece(BISHOP, US);
            constexpr Piece OUR_ROOK   = make_piece(ROOK,   US);
            constexpr Piece OUR_QUEEN  = make_piece(QUEEN,  US);

			if constexpr (CAPTURES)
				*moves++ = encode_move(src, tar, OUR_PAWN, OUR_QUEEN,  TC, 0, 0, 0);
			if constexpr (QUIETS)
			{
				*moves++ = encode_move(src, tar, OUR_PAWN, OUR_KNIGHT, TC, 0, 0, 0);
				*moves++ = encode_move(src, tar, OUR_PAWN, OUR_BISHOP, TC, 0, 0, 0);
				*moves++ = encode_move(src, tar, OUR_PAWN, OUR_ROOK,   TC, 0, 0, 0);
			}

			return moves;
		}

        // gen pseudo-legal moves for a position
        template<typename T, MoveType M, Colour US>
        T* generate_all(const Position& pos, T* moves)
        {
            constexpr bool CAPTURES = M != QUIET_MOVES;
            constexpr bool QUIETS	= M != CAPTURE_MOVES;

            constexpr Colour THEM = other_side(US);

            constexpr Piece OUR_PAWN = make_piece(PAWN, US);
            constexpr Piece OUR_KING = make_piece(KING, US);

            constexpr Square KING_ORIGIN = relative_square(US, E1);

            constexpr Rank PROMO_RANK = relative_rank(US, RANK_7);
            constexpr Rank SPAWN_RANK = relative_rank(US, RANK_2);

            constexpr Direction PUSH = pawn_push(US);

            constexpr CastleRights KS_CASTLE = ks_castle_rights(US);
            constexpr CastleRights QS_CASTLE = qs_castle_rights(US);

            constexpr Bitboard KS_CASTLE_OCC = relative_square(US, F1) | relative_square(US, G1);
            constexpr Bitboard QS_CASTLE_OCC = relative_square(US, B1) | relative_square(US, C1) | relative_square(US, D1);

            Bitboard bb = pos.pc_bb[OUR_PAWN];

            while (bb)
            {
                Square src = pop_lsb(bb);
                Square tar = src + PUSH;

				if (rank_of(src) == PROMO_RANK)
				{
					Bitboard att = Bitboards::pawn_attacks[US][src] & pos.occ_bb[THEM];

					if (!(pos.occ_bb[BOTH] & tar))
						moves = generate_promotions<T, M, US, 0>(moves, src, tar);

					while (att) 
					{
						tar = pop_lsb(att);

						moves = generate_promotions<T, M, US, 1>(moves, src, tar);
					}
				}
				else 
				{
					if (QUIETS && is_valid(tar) && !(pos.occ_bb[BOTH] & tar))
				    {
                            // single push
                            *moves++ = encode_move(src, tar, OUR_PAWN, NO_PIECE, 0, 0, 0, 0);
                            // double push
                            if (rank_of(src) == SPAWN_RANK && !(pos.occ_bb[BOTH] & (tar + PUSH)))
                                *moves++ = encode_move(src, tar + PUSH, OUR_PAWN, NO_PIECE, 0, 1, 0, 0);
                    }

                    if constexpr (CAPTURES)
                    {
                        Bitboard att = Bitboards::pawn_attacks[US][src] & pos.occ_bb[THEM];

                        while (att)
                        {
                            tar = pop_lsb(att);

                            *moves++ = encode_move(src, tar, OUR_PAWN, NO_PIECE, 1, 0, 0, 0);
                        }

                        if (pos.bs->enpassant != SQ_NONE && Bitboards::pawn_attacks[US][src] & pos.bs->enpassant)
                            *moves++ = encode_move(src, pos.bs->enpassant, OUR_PAWN, NO_PIECE, 1, 0, 1, 0);
                    }
				}
            }

            if (QUIETS && !pos.is_attacked(KING_ORIGIN, THEM))
            {
                if (pos.bs->castle & KS_CASTLE
                    && !(pos.occ_bb[BOTH] & KS_CASTLE_OCC)
                    && !pos.is_attacked(KING_ORIGIN + EAST, THEM))
                    *moves++ = encode_move(KING_ORIGIN, KING_ORIGIN + EAST + EAST, OUR_KING, NO_PIECE, 0, 0, 0, 1);

                if (pos.bs->castle & QS_CASTLE
                    && !(pos.occ_bb[BOTH] & QS_CASTLE_OCC)
                    && !pos.is_attacked(KING_ORIGIN + WEST, THEM))
                    *moves++ = encode_move(KING_ORIGIN, KING_ORIGIN + WEST + WEST, OUR_KING, NO_PIECE, 0, 0, 0, 1);
            }

			moves = generate_majors<T, M, KNIGHT, US>(pos, moves);
			moves = generate_majors<T, M, BISHOP, US>(pos, moves);
			moves = generate_majors<T, M, ROOK,   US>(pos, moves);
			moves = generate_majors<T, M, QUEEN,  US>(pos, moves);
			moves = generate_majors<T, M, KING,   US>(pos, moves);

            return moves;
        }

        template<typename T, MoveType M> 
        inline T* generate(const Position& pos, T* moves)
        {
            return (pos.side == WHITE)
                ? generate_all<T, M, WHITE>(pos, moves)
                : generate_all<T, M, BLACK>(pos, moves);
        }

		template<typename T>
		void print_moves(T* m, T* end) 
		{
			cout << "move\tpiece\tcapture\tdouble\tenpass\tcastle";

			if constexpr (is_same<T, ScoredMove>())
				cout << "\tscore";

			cout << endl;

            int count = 0;

            while (m != end)
            {
                cout << move_from_sq(*m)
                    << move_to_sq(*m)
                    << piece_str[move_promotion_type(*m)] << '\t'
                    << piece_str[move_piece_type(*m)]     << '\t'
                    << int(move_capture(*m))              << '\t'
                    << int(move_double(*m))               << '\t'
                    << int(move_enpassant(*m))            << '\t'
                    << int(move_castling(*m))             << '\t';

				if constexpr (is_same<T, ScoredMove>())
					cout << m->score;

				cout << endl;

				++m;
				++count;
            }

            cout << "Total move count:" << count << endl;
		}

	} // namespace MoveGen

} // namsepace Clovis
