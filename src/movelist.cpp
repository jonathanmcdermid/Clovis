#include "movelist.h"

namespace Clovis {

    // MVV-LVA lookup table [attacker][victim]
    constexpr int mvv_lva[15][15] = {
          0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
          0, 105, 205, 305, 405, 505, 605,    0,   0, 105, 205, 305, 405, 505, 605,
          0, 104, 204, 304, 404, 504, 604,    0,   0, 104, 204, 304, 404, 504, 604,
          0, 103, 203, 303, 403, 503, 603,    0,   0, 103, 203, 303, 403, 503, 603,
          0, 102, 202, 302, 402, 502, 602,    0,   0, 102, 202, 302, 402, 502, 602,
          0, 101, 201, 301, 401, 501, 601,    0,   0, 101, 201, 301, 401, 501, 601,
          0, 100, 200, 300, 400, 500, 600,    0,   0, 100, 200, 300, 400, 500, 600,
          0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0,   0,   0,
          0, 105, 205, 305, 405, 505, 605,    0,   0, 105, 205, 305, 405, 505, 605,
          0, 104, 204, 304, 404, 504, 604,    0,   0, 104, 204, 304, 404, 504, 604,
          0, 103, 203, 303, 403, 503, 603,    0,   0, 103, 203, 303, 403, 503, 603,
          0, 102, 202, 302, 402, 502, 602,    0,   0, 102, 202, 302, 402, 502, 602,
          0, 101, 201, 301, 401, 501, 601,    0,   0, 101, 201, 301, 401, 501, 601,
          0, 100, 200, 300, 400, 500, 600,    0,   0, 100, 200, 300, 400, 500, 600
    };

    // gen all pseudo-legal moves for a position
    ScoredMove* gen_moves(const Position& pos, ScoredMove* ml)
    {
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
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 0, 0, 0, 0));
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 0, 0, 0, 0));
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 0, 0, 0, 0));
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 0, 0, 0, 0));
                }
                else
                {
                    // single push
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0));
                    // double push
                    if ((rank_of(src)) == dprank && !get_bit(pos.occ_bitboard[BOTH], tar + dir))
                        *ml++ = score_move(pos, encode_move(src, tar + dir, pos.piece_board[src], NO_PIECE, 0, 1, 0, 0));
                }
            }

            att = Bitboards::pawn_attacks[pos.side][src] & pos.occ_bitboard[!pos.side];

            while (att)
            {
                tar = get_lsb_index(att);

                if (rank_of(src) == promorank)
                {
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(KNIGHT, pos.side), 1, 0, 0, 0));
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(BISHOP, pos.side), 1, 0, 0, 0));
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(ROOK, pos.side), 1, 0, 0, 0));
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], make_piece(QUEEN, pos.side), 1, 0, 0, 0));
                }
                else
                    *ml++ = score_move(pos, encode_move(src, tar, make_piece(PAWN, pos.side), NO_PIECE, 1, 0, 0, 0));
                pop_bit(att, tar);
            }

            if (pos.bs->enpassant != SQ_NONE)
            {
                Bitboard enpassant_attacks = Bitboards::pawn_attacks[pos.side][src] & (1ULL << pos.bs->enpassant);

                if (enpassant_attacks)
                {
                    tar = get_lsb_index(enpassant_attacks);
                    *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 1, 0));
                }
            }
            pop_bit(bb, src);
        }

        if (pos.bs->castle & crk)
        {
            if (!get_bit(pos.occ_bitboard[BOTH], kingpos + EAST) && !get_bit(pos.occ_bitboard[BOTH], kingpos + 2 * EAST))
            {
                if (!pos.is_attacked(kingpos, Colour(!pos.side)) && !pos.is_attacked(kingpos + EAST, Colour(!pos.side)))
                    *ml++ = score_move(pos, encode_move(kingpos, kingpos + 2 * EAST, pos.piece_board[kingpos], NO_PIECE, 0, 0, 0, 1));
            }
        }

        if (pos.bs->castle & crq)
        {
            if (!get_bit(pos.occ_bitboard[BOTH], kingpos + WEST) && !get_bit(pos.occ_bitboard[BOTH], kingpos + 2 * WEST) && !get_bit(pos.occ_bitboard[BOTH], kingpos + 3 * WEST))
            {
                if (!pos.is_attacked(kingpos, Colour(!pos.side)) && !pos.is_attacked(kingpos + WEST, Colour(!pos.side)))
                    *ml++ = score_move(pos, encode_move(kingpos, kingpos + 2 * WEST, pos.piece_board[kingpos], NO_PIECE, 0, 0, 0, 1));
            }
        }

        for (PieceType pt = KNIGHT; pt <= KING; ++pt) {
            bb = pos.piece_bitboard[make_piece(pt, pos.side)];
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
                        *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], NO_PIECE, 0, 0, 0, 0));
                    else
                        *ml++ = score_move(pos, encode_move(src, tar, pos.piece_board[src], NO_PIECE, 1, 0, 0, 0));
                    pop_bit(att, tar);
                }
                pop_bit(bb, src);
            }
        }

        return ml;
    }

    ScoredMove score_move(const Position& pos, const Move m) 
    {
        ScoredMove sm;
        sm.m = m;
        if (move_capture(m))
        {
            sm.score = mvv_lva[move_piece_type(m)][pos.piece_on(move_to_sq(m))];
        }
        else
        {
            sm.score = 0; // work to be done here
        }
        return sm;
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
