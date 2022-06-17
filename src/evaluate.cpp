#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        // using blunder tuned psqts until I make my own

        int mg_value[7] = { 0, 83, 328, 365, 473, 968, 10000, };

        int eg_value[7] = { 0, 98, 273, 303, 522, 976, 10000, };

        int mg_pawn_table[SQ_N] = {
            0, 0, 0, 0, 0, 0, 0, 0,
        13, 36, -13, 9, -2, 33, -64, -65,
        1, -7, 12, -2, 35, 48, 20, -5,
        -13, 3, 4, 25, 23, 17, 6, -19,
        -22, -19, -1, 14, 19, 4, -10, -22,
        -14, -20, -3, -4, 6, 5, 13, -5,
        -20, -16, -22, -12, -8, 20, 18, -11,
        0, 0, 0, 0, 0, 0, 0, 0,
        };

        int eg_pawn_table[SQ_N] = {
           0, 0, 0, 0, 0, 0, 0, 0,
        52, 40, 22, -4, 7, -1, 41, 62,
        34, 34, 11, -18, -30, -7, 14, 20,
        15, 6, -6, -24, -17, -11, 1, 4,
        3, 2, -12, -20, -18, -13, -6, -9,
        -8, -2, -14, -11, -8, -10, -15, -17,
        -2, -4, 0, -10, 1, -8, -12, -19,
        0, 0, 0, 0, 0, 0, 0, 0,
        };

        int mg_knight_table[SQ_N] = {
            -153, -52, -33, -26, 23, -85, -36, -96,
        -88, -41, 43, 12, 4, 34, -6, -27,
        -44, 30, 17, 30, 40, 59, 40, 23,
        -18, 7, -11, 30, 12, 44, 5, 16,
        -8, 5, 13, 5, 21, 11, 12, -8,
        -18, -3, 13, 17, 27, 23, 26, -9,
        -15, -33, -5, 12, 15, 23, -5, -2,
        -70, -6, -33, -17, 6, -6, -4, -15,
        };

        int eg_knight_table[SQ_N] = {
            -35, -26, 5, -17, -10, -15, -40, -73,
        1, 10, -11, 13, 3, -13, -7, -28,
        -6, -6, 15, 12, 7, 5, -6, -22,
        6, 13, 27, 24, 25, 17, 15, 0,
        5, 7, 24, 32, 23, 24, 21, 3,
        0, 9, 1, 20, 14, -3, -12, -3,
        -12, 1, 6, 4, 8, -8, -2, -23,
        -6, -23, -2, 9, -2, -2, -26, -38,
        };

        int mg_bishop_table[SQ_N] = {
           -31, -8, -88, -41, -33, -33, -7, -1,
        -34, -1, -27, -34, 10, 26, 7, -58,
        -23, 22, 30, 18, 17, 30, 19, -8,
        -7, -2, 2, 30, 20, 18, 2, -10,
        -4, 11, 2, 21, 24, 2, 4, 10,
        3, 19, 19, 7, 16, 31, 20, 5,
        13, 25, 17, 12, 17, 27, 38, 11,
        -20, 12, 7, 4, 11, 3, -13, -16,
        };

        int eg_bishop_table[SQ_N] = {
         -3, -9, 7, 0, 7, -2, -3, -15,
        7, 4, 14, -1, 2, -2, -2, -3,
        13, -1, -1, 0, -3, 2, 6, 12,
        8, 14, 14, 7, 8, 6, 4, 12,
        7, 5, 14, 13, 0, 7, 0, 2,
        2, 4, 8, 10, 11, -2, 3, 4,
        -1, -8, -1, 1, 6, -1, -3, -15,
        -4, 2, -3, 5, 3, 1, 6, 1,
        };

        int mg_rook_table[SQ_N] = {
             10, 13, -9, 14, 13, -14, 0, 0,
        10, 8, 25, 29, 32, 31, 0, 8,
        -14, 6, 1, 5, -11, 13, 34, -5,
        -26, -19, 0, 2, -2, 10, -23, -26,
        -33, -24, -13, -10, -3, -17, -9, -31,
        -32, -20, -6, -9, -3, -2, -13, -27,
        -30, -9, -6, 3, 9, 6, -5, -54,
        -6, 1, 12, 22, 22, 18, -23, -8,
        };

        int eg_rook_table[SQ_N] = {
         15, 11, 20, 15, 17, 18, 15, 13,
        14, 17, 16, 13, 5, 10, 15, 13,
        13, 10, 10, 10, 8, 3, -1, 2,
        12, 9, 14, 6, 6, 8, 6, 13,
        12, 12, 13, 7, 2, 4, 2, 4,
        5, 8, -1, 3, 0, -3, 1, -4,
        4, 0, 2, 5, -5, -3, -5, 7,
        -5, 1, 2, -1, -4, -6, 3, -18,
        };

        int mg_queen_table[SQ_N] = {
          -22, -16, -6, -8, 33, 23, 10, 25,
        -26, -46, -16, 5, -27, 4, 3, 22,
        -15, -16, -5, -19, 8, 32, 11, 26,
        -28, -31, -26, -26, -16, -9, -12, -18,
        -11, -28, -14, -18, -10, -10, -11, -11,
        -19, 4, -5, -1, -2, 1, 6, 1,
        -21, -3, 13, 14, 19, 22, 2, 14,
        4, 2, 13, 24, 6, -8, -6, -31,
        };

        int eg_queen_table[SQ_N] = {
           -13, 17, 16, 14, 14, 13, 8, 22,
        -10, 3, 19, 21, 35, 25, 18, 12,
        -9, -12, -12, 28, 29, 21, 27, 20,
        14, 17, 8, 16, 30, 25, 44, 41,
        -6, 17, 8, 27, 14, 21, 34, 33,
        12, -20, 10, -2, 5, 16, 22, 17,
        1, -12, -14, -12, -8, -15, -24, -18,
        -17, -24, -19, -13, 1, -12, -9, -30,
        };

        int mg_king_table[SQ_N] = {
           -54, 30, 48, 29, -21, 2, 17, 15,
        36, 33, 20, 54, 34, 30, -1, -15,
        31, 35, 48, 26, 32, 46, 50, 9,
        20, 28, 28, 22, 24, 26, 30, -2,
        -14, 34, 12, 7, 9, 7, 4, -20,
        0, 10, 8, 9, 9, 8, 17, -16,
        0, 16, -1, -35, -14, -5, 12, 3,
        -38, 24, 13, -61, -3, -29, 19, -6,
        };

        int eg_king_table[SQ_N] = {
           -71, -40, -23, -22, -8, 15, 2, -17,
        -14, 16, 14, 12, 15, 33, 23, 10,
        5, 17, 19, 16, 17, 44, 40, 9,
        -13, 19, 24, 28, 25, 31, 23, 1,
        -19, -3, 23, 27, 27, 24, 10, -10,
        -18, 0, 14, 20, 23, 17, 6, -4,
        -24, -7, 10, 17, 17, 11, 0, -12,
        -45, -29, -17, 1, -15, -4, -21, -39,
        };

        int* mg_piece_table[7] =
        {
            NULL,
            mg_pawn_table,
            mg_knight_table,
            mg_bishop_table,
            mg_rook_table,
            mg_queen_table,
            mg_king_table
        };

        int* eg_piece_table[7] =
        {
            NULL,
            eg_pawn_table,
            eg_knight_table,
            eg_bishop_table,
            eg_rook_table,
            eg_queen_table,
            eg_king_table
        };

        int mg_table[15][SQ_N];
        int eg_table[15][SQ_N];

        int game_phase_inc[PIECETYPE_N + PAWN] = { 0, 0, 1, 1, 2, 4, 0};

        int mg_double_pawn_penalty = -1;
        int eg_double_pawn_penalty = -17;

        int mg_isolated_pawn_penalty = -19;
        int eg_isolated_pawn_penalty = -5;

        int mg_passed_pawn_bonus[RANK_N] = { 0, 9, 4, 1, 13, 48, 109, 0 };
        int eg_passed_pawn_bonus[RANK_N] = { 0, 1, 5, 25, 50, 103, 149, 0 };

        int semi_open_file_score = 10;

        int open_file_score = 15;

        int king_shield_bonus = 2;

        Bitboard file_masks[SQ_N];

        Bitboard rank_masks[SQ_N];

        Bitboard isolated_masks[SQ_N];

        Bitboard passed_masks[COLOUR_N][SQ_N];

        void init_eval() {
            for (PieceType pt = PAWN; pt <= KING; ++pt) {
                for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) {
                    mg_table[make_piece(pt, WHITE)][sq] = mg_value[pt] + mg_piece_table[pt][flip_square(sq)];
                    eg_table[make_piece(pt, WHITE)][sq] = eg_value[pt] + eg_piece_table[pt][flip_square(sq)];
                    mg_table[make_piece(pt, BLACK)][sq] = mg_value[pt] + mg_piece_table[pt][sq];
                    eg_table[make_piece(pt, BLACK)][sq] = eg_value[pt] + eg_piece_table[pt][sq];
                }
            }
            init_masks();
        }

        void init_masks()
        {
            // file and rank masks
            for (File f = FILE_A; f <= FILE_H; ++f)
            {
                for (Rank r = RANK_1; r <= RANK_8; ++r)
                {
                    Square sq = make_square(f, r);

                    file_masks[sq] = set_file_rank_mask(f, RANK_NONE);
                    rank_masks[sq] = set_file_rank_mask(FILE_NONE, r);

                    isolated_masks[sq] |= set_file_rank_mask(f - 1, RANK_NONE);
                    isolated_masks[sq] |= set_file_rank_mask(f + 1, RANK_NONE);
                }
            }
            for (Rank r = RANK_1; r <= RANK_8; ++r)
            {
                for (File f = FILE_A; f <= FILE_H; ++f)
                {
                    Square sq = make_square(f, r);

                    passed_masks[WHITE][sq] |= set_file_rank_mask(f - 1, RANK_NONE);
                    passed_masks[WHITE][sq] |= set_file_rank_mask(f, RANK_NONE);
                    passed_masks[WHITE][sq] |= set_file_rank_mask(f + 1, RANK_NONE);

                    for (int i = 0; i < r + 1; ++i)
                        passed_masks[WHITE][sq] &= ~rank_masks[i * 8 + f];

                    passed_masks[BLACK][sq] |= set_file_rank_mask(f - 1, RANK_NONE);
                    passed_masks[BLACK][sq] |= set_file_rank_mask(f, RANK_NONE);
                    passed_masks[BLACK][sq] |= set_file_rank_mask(f + 1, RANK_NONE);

                    for (int i = 0; i < 8 - r; ++i)
                        passed_masks[BLACK][sq] &= ~rank_masks[(7 - i) * 8 + f];
                }
            }
        }

        Bitboard set_file_rank_mask(File file_number, Rank rank_number)
        {
            Bitboard mask = 0ULL;

            for (Rank r = RANK_1; r <= RANK_8; ++r)
            {
                for (File f = FILE_A; f <= FILE_H; ++f)
                {
                    Square sq = make_square(f, r);

                    if (file_number != FILE_NONE)
                    {
                        if (f == file_number)
                            set_bit(mask, sq);
                    }

                    else if (rank_number != RANK_NONE)
                    {
                        if (r == rank_number)
                            set_bit(mask, sq);
                    }
                }
            }

            return mask;
        }

        int evaluate(const Position& pos)
        {
            int max_score = POS_INF;
            int min_score = NEG_INF;
            if (pos.is_insufficient(pos.side))
            {
                max_score = 0;
                if (pos.is_insufficient(Colour(!pos.side)))
                    return 0;
            }
            else if (pos.is_insufficient(Colour(!pos.side)))
                min_score = 0;

            int mg_score[COLOUR_N] = { 0 };
            int eg_score[COLOUR_N] = { 0 };

            Piece piece; 
            Square sq;
            Bitboard bb;
            Colour side;

            int game_phase = 0;
            int mobility;

            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                piece = make_piece(pt, WHITE);

            repeat:

                side = get_side(piece);
                bb = pos.piece_bitboard[piece];
                //else if (pt == KING)
                //{
                //    while (bb)
                //    {
                //        sq = get_lsb_index(bb);
                //        mg_score[side] += mg_table[piece][sq];
                //        eg_score[side] += eg_table[piece][sq];
                //        game_phase += game_phase_inc[pt];
                //        //mg_score[side] += count_bits(Bitboards::king_attacks[sq] & pos.occ_bitboard[side]) * king_shield_bonus;
                //        pop_bit(bb, sq);
                //    }
                //}
                while (bb)
                {
                    sq = get_lsb_index(bb);
                    mg_score[side] += mg_table[piece][sq];
                    eg_score[side] += eg_table[piece][sq];
                    game_phase += game_phase_inc[pt];
                    pop_bit(bb, sq);
                }
                if (side == WHITE) {
                    piece = make_piece(pt, BLACK);
                    goto repeat;
                }
            }

            game_phase = std::max(game_phase, MAX_GAMEPHASE);

            int mg_score_comb = mg_score[pos.side] - mg_score[!pos.side];
            int eg_score_comb = eg_score[pos.side] - eg_score[!pos.side];

            int total_score = (mg_score_comb * game_phase + eg_score_comb * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;

            return std::max(min_score, std::min(max_score, total_score));
        }

        int evaluate_pawns(const Position& pos)
        {
            Piece piece;
            Square sq;
            Bitboard bb;
            Colour side;

            int mg_score[COLOUR_N] = { 0 };

            piece = make_piece(PAWN, WHITE);

        repeat:

            side = get_side(piece);
            bb = pos.piece_bitboard[piece];

            while (bb)
            {
                sq = get_lsb_index(bb);

                //int double_pawns = count_bits(pos.piece_bitboard[piece] & file_masks[sq]);
                //
                //if (double_pawns > 1)
                //{ 
                //    mg_score[side] += double_pawns * mg_double_pawn_penalty;
                //    eg_score[side] += double_pawns * eg_double_pawn_penalty;
                //}

                if ((pos.piece_bitboard[piece] & isolated_masks[sq]) == 0)
                {
                    mg_score[side] += mg_isolated_pawn_penalty;
                }

                if ((passed_masks[side][sq] & pos.piece_bitboard[make_piece(PAWN, Colour(!side))]) == 0) {
                    mg_score[side] += mg_passed_pawn_bonus[relative_rank(side, sq)];
                }

                pop_bit(bb, sq);
            }

            if (side == WHITE) {
                piece = make_piece(PAWN, BLACK);
                goto repeat;
            }

            int mg_score_comb = mg_score[WHITE] - mg_score[BLACK];

            return mg_score_comb;
        }

    } // namespace Eval

} // namespace Clovis