#include "parse.h"

namespace Clovis {

	namespace Parse {

		Move parse(const Position& pos, string move)
		{
			if (move[move.length() - 1] == '+' || move[move.length() - 1] == '#')
				move = move.substr(0, move.length() - 1);

			if (move == "O-O-O")
			{
				return (pos.side == WHITE)
					? encode_move(E1, C1, W_KING, NO_PIECE, 0, 0, 0, 1) 
					: encode_move(E8, C8, B_KING, NO_PIECE, 0, 0, 0, 1);
			}

			if (move == "O-O")
			{
				return (pos.side == WHITE)
					? encode_move(E1, G1, W_KING, NO_PIECE, 0, 0, 0, 1)
					: encode_move(E8, G8, B_KING, NO_PIECE, 0, 0, 0, 1);
			}

			Piece promotion = move[move.length() - 2] == '='
				? move[move.length() - 1] == 'Q' 
				? make_piece(QUEEN, pos.side)
				: move[move.length() - 1] == 'R'
				? make_piece(ROOK, pos.side)
				: move[move.length() - 1] == 'B'
				? make_piece(BISHOP, pos.side)
				: make_piece(KNIGHT, pos.side)
				: NO_PIECE;

			if (islower(move[0]))
			{
				if (move[1] == 'x')
				{
					Square to = str2sq(move.substr(2, 2));
					Square from = make_square(File(move[0] - 'a'), rank_of(to - pawn_push(pos.side)));
					return encode_move(from, to, make_piece(PAWN, pos.side), promotion, 1, 0, pos.bs->enpassant == to, 0);
				}
				else
				{
					Square to = str2sq(move);
					Square from = pos.pc_table[to - pawn_push(pos.side)] == NO_PIECE 
						? to - 2 * pawn_push(pos.side) 
						: to - pawn_push(pos.side);
					return encode_move(from, to, make_piece(PAWN, pos.side), promotion, 0, abs(rank_of(to) - rank_of(from)) == 2, 0, 0);
				}
			}

			Piece piece = make_piece( 
				  move[0] == 'K'
				  ? KING
				  : move[0] == 'Q' 
				  ? QUEEN
				  : move[0] == 'R'
				  ? ROOK
				  : move[0] == 'B'
				  ? BISHOP
				  : KNIGHT, pos.side);

			if (move[1] == 'x')
			{
				Square to = str2sq(move.substr(2, 2));
				Square from = lsb(Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece]);
				return encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0);
			}
			else if (move[2] == 'x')
			{
				Square to = str2sq(move.substr(3, 2));
				Bitboard bb = Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
				Square from = pop_lsb(bb);
				if (isdigit(move[1]))
					while (rank_of(from) != Rank(move[1] - '1'))
						from = pop_lsb(bb);
				else
					while (file_of(from) != File(move[1] - 'a'))
						from = pop_lsb(bb);
				return encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0);
			}
			else if (move.length() == 3)
			{
				Square to = str2sq(move.substr(1, 2));
				Square from = lsb(Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece]);
				return encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0);
			}
			else if (move.length() == 4)
			{
				Square to = str2sq(move.substr(2, 2));
				Bitboard bb = Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
				Square from = pop_lsb(bb);
				while (file_of(from) != File(move[1] - 'a'))
					from = pop_lsb(bb);
				return encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0);
			}
			else if(move.length() == 5)
			{
				Square to = str2sq(move.substr(3, 2));
				Square from = str2sq(move.substr(1, 2));
				return encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0);
			}

			exit(EXIT_FAILURE);
		}

	} // namespace Parse

} // namespace Clovis
