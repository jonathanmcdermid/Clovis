#include "movelist.h"

namespace Clovis {

    // gen all pseudo-legal moves for a position
    ScoredMove* gen_moves(const Position& pos, ScoredMove* ml)
    {
        Square src, tar, king_origin;
        Bitboard bb, att;
        bb = pos.piece_bitboard[make_piece(PAWN, pos.side)];
        king_origin = pos.side == WHITE ? E1 : E8;

        // repeat until all friendly pawns are popped
        while (bb)
        {
            src = get_lsb_index(bb);
            tar = src + pawn_push(pos.side);
            if (is_valid(tar) && !get_bit(pos.occ_bitboard[BOTH], tar))
            {
                // promotion
                if (relative_rank(pos.side, src) == RANK_7)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 0, 0, 0, 0);
                }
                else
                {
                    // single push
                    *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    // double push
                    if (relative_rank(pos.side, src) == RANK_2 && !get_bit(pos.occ_bitboard[BOTH], tar + pawn_push(pos.side)))
                        *ml++ = encode_move(src, tar + pawn_push(pos.side), pos.piece_board[src], NO_PIECE, 0, 1, 0, 0);
                }
            }

            att = Bitboards::pawn_attacks[pos.side][src] & pos.occ_bitboard[other_side(pos.side)];

            while (att)
            {
                tar = get_lsb_index(att);

                if (relative_rank(pos.side, src) == RANK_7)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 1, 0, 0, 0);
                }
                else
                    *ml++ = encode_move(src, tar, make_piece(PAWN, pos.side), NO_PIECE, 1, 0, 0, 0);
                pop_bit(att, tar);
            }

            if (pos.bs->enpassant != SQ_NONE)
            {
                Bitboard enpassant_attacks = Bitboards::pawn_attacks[pos.side][src] & (1ULL << pos.bs->enpassant);

                if (enpassant_attacks)
                {
                    tar = get_lsb_index(enpassant_attacks);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 1, 0);
                }
            }
            pop_bit(bb, src);
        }

        if (pos.bs->castle & (1 << (pos.side * 2)))
        {
            if (!get_bit(pos.occ_bitboard[BOTH], king_origin + EAST) && !get_bit(pos.occ_bitboard[BOTH], king_origin + 2 * EAST))
            {
                if (!pos.is_attacked(king_origin, other_side(pos.side)) && !pos.is_attacked(king_origin + EAST, other_side(pos.side)))
                    *ml++ = encode_move(king_origin, king_origin + 2 * EAST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);
            }
        }

        if (pos.bs->castle & (1 << (pos.side * 2 + 1)))
        {
            if (!get_bit(pos.occ_bitboard[BOTH], king_origin + WEST) && !get_bit(pos.occ_bitboard[BOTH], king_origin + 2 * WEST) && !get_bit(pos.occ_bitboard[BOTH], king_origin + 3 * WEST))
            {
                if (!pos.is_attacked(king_origin, other_side(pos.side)) && !pos.is_attacked(king_origin + WEST, other_side(pos.side)))
                    *ml++ = encode_move(king_origin, king_origin + 2 * WEST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);
            }
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt)
        {
            bb = pos.piece_bitboard[make_piece(pt, pos.side)];
            while (bb)
            {
                src = get_lsb_index(bb);
                att = ~pos.occ_bitboard[pos.side];

                switch (pt)
                {
                case KNIGHT: 
                    att &= Bitboards::knight_attacks[src]; 
                    break;
                case BISHOP: 
                    att &= Bitboards::get_bishop_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                case ROOK: 
                    att &= Bitboards::get_rook_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                case QUEEN: 
                    att &= Bitboards::get_queen_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                default: 
                    att &= Bitboards::king_attacks[src];
                }

                while (att)
                {
                    tar = get_lsb_index(att);

                    if (!get_bit((pos.occ_bitboard[other_side(pos.side)]), tar))
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    else
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 0, 0);

                    pop_bit(att, tar);
                }
                pop_bit(bb, src);
            }
        }

        return ml;
    }

    // gen all pseudo-legal capture moves for a position
    ScoredMove* gen_cap_moves(const Position& pos, ScoredMove* ml)
    {
        Square src, tar;
        Bitboard bb, att;
        bb = pos.piece_bitboard[make_piece(PAWN, pos.side)];

        // repeat until all friendly pawns are popped
        while (bb)
        {
            src = get_lsb_index(bb);
            tar = src + pawn_push(pos.side);

            att = Bitboards::pawn_attacks[pos.side][src] & pos.occ_bitboard[other_side(pos.side)];

            while (att)
            {
                tar = get_lsb_index(att);

                if (relative_rank(pos.side, src) == RANK_7)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 1, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 1, 0, 0, 0);
                }
                else
                    *ml++ = encode_move(src, tar, make_piece(PAWN, pos.side), NO_PIECE, 1, 0, 0, 0);
                pop_bit(att, tar);
            }

            if (pos.bs->enpassant != SQ_NONE)
            {
                Bitboard enpassant_attacks = Bitboards::pawn_attacks[pos.side][src] & (1ULL << pos.bs->enpassant);

                if (enpassant_attacks)
                {
                    tar = get_lsb_index(enpassant_attacks);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 1, 0);
                }
            }
            pop_bit(bb, src);
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt)
        {
            bb = pos.piece_bitboard[make_piece(pt, pos.side)];
            while (bb)
            {
                src = get_lsb_index(bb);
                att = ~pos.occ_bitboard[pos.side];

                switch (pt)
                {
                case KNIGHT: 
                    att &= Bitboards::knight_attacks[src]; 
                    break;
                case BISHOP: 
                    att &= Bitboards::get_bishop_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                case ROOK: 
                    att &= Bitboards::get_rook_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                case QUEEN: 
                    att &= Bitboards::get_queen_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                default: 
                    att &= Bitboards::king_attacks[src];
                }

                while (att)
                {
                    tar = get_lsb_index(att);

                    if (get_bit((pos.occ_bitboard[other_side(pos.side)]), tar))
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 0, 0);

                    pop_bit(att, tar);
                }
                pop_bit(bb, src);
            }
        }

        return ml;
    }

    // gen all pseudo-legal quiet moves for a position
    ScoredMove* gen_quiet_moves(const Position& pos, ScoredMove* ml)
    {
        Square src, tar, king_origin;
        Bitboard bb, att;
        bb = pos.piece_bitboard[make_piece(PAWN, pos.side)];
        king_origin = pos.side == WHITE ? E1 : E8;

        // repeat until all friendly pawns are popped
        while (bb)
        {
            src = get_lsb_index(bb);
            tar = src + pawn_push(pos.side);
            if (is_valid(tar) && !get_bit(pos.occ_bitboard[BOTH], tar))
            {
                // promotion
                if (relative_rank(pos.side, src) == RANK_7)
                {
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 0, 0, 0, 0);
                    *ml++ = encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 0, 0, 0, 0);
                }
                else
                {
                    // single push
                    *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);
                    // double push
                    if (relative_rank(pos.side, src) == RANK_2 && !get_bit(pos.occ_bitboard[BOTH], tar + pawn_push(pos.side)))
                        *ml++ = encode_move(src, tar + pawn_push(pos.side), pos.piece_board[src], NO_PIECE, 0, 1, 0, 0);
                }
            }
            pop_bit(bb, src);
        }

        if (pos.bs->castle & (1 << (pos.side * 2)))
        {
            if (!get_bit(pos.occ_bitboard[BOTH], king_origin + EAST) && !get_bit(pos.occ_bitboard[BOTH], king_origin + 2 * EAST))
            {
                if (!pos.is_attacked(king_origin, other_side(pos.side)) && !pos.is_attacked(king_origin + EAST, other_side(pos.side)))
                    *ml++ = encode_move(king_origin, king_origin + 2 * EAST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);
            }
        }

        if (pos.bs->castle & (1 << (pos.side * 2 + 1)))
        {
            if (!get_bit(pos.occ_bitboard[BOTH], king_origin + WEST) && !get_bit(pos.occ_bitboard[BOTH], king_origin + 2 * WEST) && !get_bit(pos.occ_bitboard[BOTH], king_origin + 3 * WEST))
            {
                if (!pos.is_attacked(king_origin, other_side(pos.side)) && !pos.is_attacked(king_origin + WEST, other_side(pos.side)))
                    *ml++ = encode_move(king_origin, king_origin + 2 * WEST, pos.piece_board[king_origin], NO_PIECE, 0, 0, 0, 1);
            }
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt)
        {
            bb = pos.piece_bitboard[make_piece(pt, pos.side)];
            while (bb)
            {
                src = get_lsb_index(bb);
                att = ~pos.occ_bitboard[pos.side];

                switch (pt)
                {
                case KNIGHT: 
                    att &= Bitboards::knight_attacks[src]; 
                    break;
                case BISHOP: 
                    att &= Bitboards::get_bishop_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                case ROOK: 
                    att &= Bitboards::get_rook_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                case QUEEN: 
                    att &= Bitboards::get_queen_attacks(pos.occ_bitboard[BOTH], src); 
                    break;
                default: 
                    att &= Bitboards::king_attacks[src];
                }

                while (att)
                {
                    tar = get_lsb_index(att);

                    if (!get_bit((pos.occ_bitboard[other_side(pos.side)]), tar))
                        *ml++ = encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0);

                    pop_bit(att, tar);
                }
                pop_bit(bb, src);
            }
        }

        return ml;
    }

    namespace MoveGen {

        void MoveList::print()
        {
            std::cout << "\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\n\n";

            int move_count = 0;
            for (ScoredMove* m = moves; m != last; ++m, ++move_count)
            {
                std::cout << sq2str(move_from_sq(m->m))
                    << sq2str(move_to_sq(m->m))
                    << piece_str[move_promotion_type(m->m)] << '\t'
                    << piece_str[move_piece_type(m->m)] << '\t'
                    << int(move_capture(m->m)) << '\t'
                    << int(move_double(m->m)) << '\t'
                    << int(move_enpassant(m->m)) << '\t'
                    << int(move_castling(m->m)) << '\n';

            }
            std::cout << "\n\nTotal move count:" << move_count;
        }

    } // namespace MoveGen

} // namespace Clovis
