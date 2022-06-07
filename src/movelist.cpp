#include "movelist.h"

namespace Clovis {

    // gen all pseudo-legal moves for a position
    MoveList::MoveList(const Position& pos)
    {

        last = moveList;

        Square src, tar, kingpos;
        Bitboard bb, att;
        Direction dir;
        Rank promorank, dprank;
        CastleRights crk, crq; // this can be improved

        if (pos.side == WHITE) 
        {
            dir = NORTH;
            promorank = RANK_7;
            dprank = RANK_2;
            bb = pos.piece_bitboard[W_PAWN];
            kingpos = E1;
            crk = WHITE_KS;
            crq = WHITE_QS;
        }
        else 
        {
            dir = SOUTH;
            promorank = RANK_2;
            dprank = RANK_7;
            bb = pos.piece_bitboard[B_PAWN];
            kingpos = E8;
            crk = BLACK_KS;
            crq = BLACK_QS;
        }

        // repeat until all friendly pawns are popped
        while (bb)
        {
            src = get_lsb_index(bb);
            tar = src + dir;
            if (is_valid(tar) && !get_bit(pos.occ_bitboard[BOTH], tar))
            {
                // promotion
                if (rank_of(src) == promorank)
                {
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 0, 0, 0, 0);
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 0, 0, 0, 0);
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 0, 0, 0, 0);
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 0, 0, 0, 0);
                }
                else
                {
                    // single push
                    *last++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    // double push
                    if ((rank_of(src)) == dprank && !get_bit(pos.occ_bitboard[BOTH], tar + dir))
                        *last++ = encode_move(src, tar + dir, pos.piece_board[src], NO_PIECE, 0, 1, 0, 0);
                }
            }

            att = Bitboards::pawn_attacks[pos.side][src] & pos.occ_bitboard[!pos.side];

            while (att)
            {
                tar = get_lsb_index(att);

                if (rank_of(src) == promorank)
                {
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 1, 0, 0, 0);
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 1, 0, 0, 0);
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 1, 0, 0, 0);
                    *last++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 1, 0, 0, 0);
                }
                else
                    *last++ = encode_move(src, tar, make_piece(PAWN, pos.side), NO_PIECE, 1, 0, 0, 0);
                pop_bit(att, tar);
            }

            if (pos.bs->enpassant != SQ_NONE)
            {
                Bitboard enpassant_attacks = Bitboards::pawn_attacks[pos.side][src] & (1ULL << pos.bs->enpassant);

                if (enpassant_attacks)
                {
                    tar = get_lsb_index(enpassant_attacks);
                    *last++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 1, 0);
                }
            }
            pop_bit(bb, src);
        }
        
        if (pos.bs->castle & crk)
        {
            if (!get_bit(pos.occ_bitboard[BOTH], kingpos + EAST) && !get_bit(pos.occ_bitboard[BOTH], kingpos + 2 * EAST))
            {
                if (!pos.is_attacked(kingpos, Colour(!pos.side)) && !pos.is_attacked(kingpos + EAST, Colour(!pos.side)))
                    *last++ = encode_move(kingpos, kingpos + 2 * EAST, pos.piece_board[kingpos], NO_PIECE, 0, 0, 0, 1);
            }
        }

        if (pos.bs->castle & crq)
        {
            if (!get_bit(pos.occ_bitboard[BOTH], kingpos + WEST) && !get_bit(pos.occ_bitboard[BOTH], kingpos + 2 * WEST) && !get_bit(pos.occ_bitboard[BOTH], kingpos + 3 * WEST))
            {
                if (!pos.is_attacked(kingpos, Colour(!pos.side)) && !pos.is_attacked(kingpos + WEST, Colour(!pos.side)))
                    *last++ = encode_move(kingpos, kingpos + 2 * WEST, pos.piece_board[kingpos], NO_PIECE, 0, 0, 0, 1);
            }
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt) {
            bb = pos.piece_bitboard[make_piece(pt, Colour(pos.side))];
            while (bb)
            {
                src = get_lsb_index(bb);
                att = ~pos.occ_bitboard[pos.side];

                switch (pt) {
                case KNIGHT: att &= Bitboards::knight_attacks[src]; break;
                case BISHOP: att &= Bitboards::get_bishop_attacks(pos.occ_bitboard[BOTH], src); break;
                case ROOK: att &= Bitboards::get_rook_attacks(pos.occ_bitboard[BOTH], src); break;
                case QUEEN: att &= Bitboards::get_queen_attacks(pos.occ_bitboard[BOTH], src); break;
                default: att &= Bitboards::king_attacks[src];
                }

                while (att)
                {
                    tar = get_lsb_index(att);

                    if (!get_bit((pos.occ_bitboard[!pos.side]), tar))
                        *last++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    else
                        *last++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 0, 0);
                    pop_bit(att, tar);
                }
                pop_bit(bb, src);
            }
        }
    }

    void MoveList::print()
    {
        std::cout << "\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\n\n";

        int move_count = 0;
        for(Move* move = moveList; move != last; ++move, ++move_count)
        {
            std::cout << sq2str(move_from_sq(*move))
                << sq2str(move_to_sq(*move)) 
                << piece_str[move_promotion_type(*move)] << '\t'
                << piece_str[move_piece_type(*move)] << '\t'
                << int(move_capture(*move)) << '\t'
                << int(move_double(*move)) << '\t'
                << int(move_enpassant(*move)) << '\t'
                << int(move_castling(*move)) << '\n';

        }
        std::cout << "\n\nTotal move count:" << move_count;
    }

} // namespace Clovis