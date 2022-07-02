#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        enum GamePhase : int {
            MG, EG,
            PHASE_N = 2
        };

        // pawn, rook failed after tuning
        Score pawn_table[SQ_N] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(13,52), Score(36,40), Score(-13,22), Score(9,-4), Score(-2,7), Score(33,-1), Score(-64,41), Score(-65,62),
        Score(1,34), Score(-7,34), Score(12,11), Score(-2,-18), Score(35,-30), Score(48,-7), Score(20,14), Score(-5,20),
        Score(-13,15), Score(3,6), Score(4,-6), Score(25,-24), Score(23,-17), Score(17,-11), Score(6,1), Score(-19,4),
        Score(-22,3), Score(-19,2), Score(-1,-12), Score(14,-20), Score(19,-18), Score(4,-13), Score(-10,-6), Score(-22,-9),
        Score(-14,-8), Score(-20,-2), Score(-3,-14), Score(-4,-11), Score(6,-8), Score(5,-10), Score(13,-15), Score(-5,-17),
        Score(-20,-2), Score(-16,-4), Score(-22,0), Score(-12,-10), Score(-8,1), Score(20,-8), Score(18,-12), Score(-11,-19),
        Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[SQ_N] = {
                Score(-192,-34), Score(-97,15), Score(-94,35), Score(0,17), Score(35,9), Score(-84,47), Score(-5,-2), Score(-168,-21),
                Score(2,-25), Score(-24,4), Score(32,11), Score(43,36), Score(44,35), Score(90,-8), Score(-23,10), Score(24,-15),
                Score(-27,-3), Score(35,17), Score(44,38), Score(68,35), Score(92,26), Score(111,31), Score(80,10), Score(54,-7),
                Score(17,-1), Score(27,27), Score(44,40), Score(49,56), Score(40,57), Score(61,51), Score(38,39), Score(39,9),
                Score(2,-9), Score(1,26), Score(27,43), Score(29,48), Score(33,48), Score(26,47), Score(42,27), Score(13,8),
                Score(-8,-35), Score(9,6), Score(15,12), Score(22,34), Score(23,28), Score(18,12), Score(24,5), Score(3,-28),
                Score(-25,-17), Score(-15,-2), Score(4,0), Score(8,8), Score(11,10), Score(12,-15), Score(-17,0), Score(0,-23),
                Score(-101,-47), Score(-9,-59), Score(-27,-13), Score(-15,-10), Score(-8,-9), Score(-4,-19), Score(-2,-53), Score(-126,21),
        };
        Score bishop_table[SQ_N] = {
                Score(-87,12), Score(-66,23), Score(-115,22), Score(-108,27), Score(-100,18), Score(-138,25), Score(55,-7), Score(-43,35),
                Score(-45,4), Score(-14,10), Score(-18,10), Score(-15,10), Score(-9,9), Score(1,16), Score(-7,12), Score(-5,-3),
                Score(-12,-2), Score(6,8), Score(36,5), Score(17,13), Score(30,14), Score(55,20), Score(47,10), Score(15,5),
                Score(-17,-5), Score(13,11), Score(7,15), Score(38,20), Score(24,26), Score(33,14), Score(7,24), Score(-6,4),
                Score(-12,-11), Score(-2,8), Score(0,21), Score(20,19), Score(28,14), Score(-1,11), Score(-1,6), Score(6,-20),
                Score(-4,-17), Score(0,1), Score(-2,6), Score(5,9), Score(-1,8), Score(4,-5), Score(2,-10), Score(9,-16),
                Score(-10,-15), Score(5,-26), Score(5,-10), Score(-10,-10), Score(-7,-12), Score(-1,-35), Score(15,-21), Score(2,-54),
                Score(-9,-25), Score(-4,-30), Score(-22,-39), Score(-29,-21), Score(-34,-20), Score(-20,-27), Score(-8,-23), Score(-14,-31),
        };
        Score rook_table[SQ_N] = {
                Score(10,15), Score(13,11), Score(-9,20), Score(14,15), Score(13,17), Score(-14,18), Score(0,15), Score(0,13),
                Score(10,14), Score(8,17), Score(25,16), Score(29,13), Score(32,5), Score(31,10), Score(0,15), Score(8,13),
                Score(-14,13), Score(6,10), Score(1,10), Score(5,10), Score(-11,8), Score(13,3), Score(34,-1), Score(-5,-2),
                Score(-26,12), Score(-19,9), Score(0,14), Score(2,6), Score(-2,6), Score(10,8), Score(-23,6), Score(-26,13),
                Score(-33,12), Score(-24,12), Score(-13,13), Score(-10,7), Score(-3,2), Score(-17,4), Score(-9,2), Score(-31,4),
                Score(-32,5), Score(-20,8), Score(-6,-1), Score(-9,3), Score(-3,0), Score(-2,-3), Score(-13,1), Score(-27,-4),
                Score(-30,4), Score(-9,0), Score(-6,2), Score(3,5), Score(9,-5), Score(6,-3), Score(-5,-5), Score(-54,7),
                Score(-6,-5), Score(1,1), Score(12,2), Score(22,-1), Score(22,-4), Score(18,-6), Score(-23,3), Score(-8,-18),
        };
        Score queen_table[SQ_N] = {
                Score(-29,62), Score(17,41), Score(35,56), Score(38,56), Score(70,52), Score(91,56), Score(98,64), Score(85,45),
                Score(-13,20), Score(-56,80), Score(-20,73), Score(-57,121), Score(-16,120), Score(44,86), Score(4,113), Score(51,84),
                Score(-10,-3), Score(-5,20), Score(-15,60), Score(-7,74), Score(2,103), Score(92,82), Score(87,84), Score(37,120),
                Score(-12,6), Score(-13,38), Score(-13,50), Score(-5,62), Score(-11,115), Score(19,102), Score(23,98), Score(34,66),
                Score(-6,-8), Score(1,30), Score(0,23), Score(3,48), Score(-6,67), Score(16,51), Score(22,45), Score(26,41),
                Score(-8,-12), Score(7,-11), Score(1,23), Score(5,7), Score(6,5), Score(8,14), Score(22,5), Score(14,-4),
                Score(-12,-14), Score(5,-19), Score(7,-30), Score(6,-15), Score(12,-38), Score(23,-65), Score(27,-66), Score(1,-51),
                Score(9,-25), Score(-3,-40), Score(5,-49), Score(12,-37), Score(8,-45), Score(-12,-36), Score(11,-72), Score(-15,-36),
        };
        Score king_table[SQ_N] = {
                Score(-54,-71), Score(30,-40), Score(48,-23), Score(29,-22), Score(-21,-8), Score(2,15), Score(17,2), Score(15,-17),
                Score(36,-14), Score(33,16), Score(20,14), Score(54,12), Score(34,15), Score(30,33), Score(-1,23), Score(-15,10),
                Score(31,5), Score(35,17), Score(48,19), Score(26,16), Score(32,17), Score(46,44), Score(50,40), Score(9,9),
                Score(20,-13), Score(28,19), Score(28,24), Score(22,28), Score(24,25), Score(26,31), Score(30,23), Score(-2,1),
                Score(-14,-19), Score(34,-3), Score(12,23), Score(7,27), Score(9,27), Score(7,24), Score(4,10), Score(-20,-10),
                Score(0,-18), Score(10,0), Score(8,14), Score(9,20), Score(9,23), Score(8,17), Score(17,6), Score(-16,-4),
                Score(0,-24), Score(16,-7), Score(-1,10), Score(-35,17), Score(-14,17), Score(-5,11), Score(12,0), Score(3,-12),
                Score(-38,-45), Score(24,-29), Score(13,-17), Score(-61,1), Score(-3,-15), Score(-29,-4), Score(19,-21), Score(-6,-39),
        };
        Score piece_value[7] = { Score(0, 0), Score(83, 98), Score(328, 273), Score(365, 303), Score(473, 522), Score(968, 976), Score(10000, 10000), };
        Score passed_pawn_bonus[RANK_N] = { Score(0, 0), Score(8, 18), Score(0, 20), Score(0, 40), Score(20, 56), Score(64, 96), Score(131, 111), Score(0, 0), };
        Score double_pawn_penalty = Score(4, 2);
        Score isolated_pawn_penalty = Score(11, 14);
        Score bishop_pair_bonus = Score(9, 71);
        Score rook_open_file_bonus = Score(35, 16);
        Score rook_semi_open_file_bonus = Score(18, 11);
        Score pawn_connected_bonus = Score(3, 5);
        Score king_semi_open_file_penalty = Score(13, 3);
        Score trapped_rook_penalty = Score(13, 13);

        Score* piece_table[7] =
        {
            NULL,
            pawn_table,
            knight_table,
            bishop_table,
            rook_table,
            queen_table,
            king_table
        };

        Score score_table[15][SQ_N];

        Bitboard file_masks[SQ_N];

        Bitboard rank_masks[SQ_N];

        Bitboard isolated_masks[SQ_N];

        Bitboard passed_masks[COLOUR_N][SQ_N];

        Bitboard knight_outpost_masks[COLOUR_N][SQ_N];

        void init_eval() 
        {
            init_values();
            init_masks();
        }

        void init_values()
        {
            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
                {
                    score_table[make_piece(pt, WHITE)][sq] = piece_value[pt] + piece_table[pt][flip_square(sq)];
                    score_table[make_piece(pt, BLACK)][sq] = piece_value[pt] + piece_table[pt][sq];
                }
            }
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

            Score score[COLOUR_N];

            Piece piece; 
            Square sq;
            Bitboard bb;
            Colour side;

            int mobility;

            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                piece = make_piece(pt, WHITE);

            repeat:

                side = get_side(piece);
                bb = pos.piece_bitboard[piece];
                if (pt == KNIGHT)
                {
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                }
                else if (pt == BISHOP)
                {
                    if (count_bits(bb) >= 2)
                        score[side] += bishop_pair_bonus;

                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                }
                else if (pt == ROOK)
                {
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        if ((file_masks[sq] & (pos.piece_bitboard[W_PAWN] | pos.piece_bitboard[B_PAWN])) == 0)
                            score[side] += rook_open_file_bonus;
                        else if ((file_masks[sq] & pos.piece_bitboard[make_piece(PAWN, side)]) == 0)
                            score[side] += rook_semi_open_file_bonus;
                        //else
                        //{
                        //    if (count_bits(Bitboards::get_rook_attacks(pos.occ_bitboard[BOTH], sq)) <= 3)
                        //    {
                        //        File king_file = file_of(get_lsb_index(pos.piece_bitboard[make_piece(KING, side)]));
                        //        if ((file_of(sq) < king_file) == (king_file < FILE_E))
                        //            score[side] -= trapped_rook_penalty * (1 + !(pos.bs->castle & (1 << (side * 2) | 1 << (side * 2 + 1))));
                        //    }
                        //}
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                }
                else if (pt == KING)
                {
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        //if ((file_masks[sq] & pos.piece_bitboard[make_piece(PAWN, side)]) == 0)
                        //    score[side] -= king_semi_open_file_penalty;
                        //if (file_of(sq) != FILE_A && (file_masks[sq + WEST] & pos.piece_bitboard[make_piece(PAWN, side)]) == 0)
                        //    score[side] -= king_semi_open_file_penalty;
                        //if (file_of(sq) != FILE_H && (file_masks[sq + EAST] & pos.piece_bitboard[make_piece(PAWN, side)]) == 0)
                        //    score[side] -= king_semi_open_file_penalty;
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                }
                else
                {
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                }
                if (side == WHITE) 
                {
                    piece = make_piece(pt, BLACK);
                    goto repeat;
                }
            }

            int game_phase = pos.get_game_phase();

            Score score_comb = score[pos.side] - score[other_side(pos.side)];

            int total_score = (score_comb.mg * game_phase + score_comb.eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;

            return std::max(min_score, std::min(max_score, total_score));
        }

        Score evaluate_pawns(const Position& pos)
        {
            Piece piece;
            Square sq;
            Bitboard bb;
            Colour side;

            Score score[COLOUR_N];

            piece = make_piece(PAWN, WHITE);

        repeat:

            side = get_side(piece);
            bb = pos.piece_bitboard[piece];

            while (bb)
            {
                sq = get_lsb_index(bb);

                int double_pawns = count_bits(pos.piece_bitboard[piece] & file_masks[sq]);
                
                if (double_pawns > 1)
                { 
                    score[side] -= double_pawn_penalty * double_pawns;
                }

                if ((pos.piece_bitboard[piece] & isolated_masks[sq]) == 0)
                {
                    score[side] -= isolated_pawn_penalty;
                }

                if ((passed_masks[side][sq] & pos.piece_bitboard[make_piece(PAWN, Colour(!side))]) == 0) 
                {
                    score[side] += passed_pawn_bonus[relative_rank(side, sq)];
                }

                //int connected_pawns = count_bits(pos.piece_bitboard[piece] & Bitboards::pawn_attacks[side][sq]);
                //
                //score[side] += pawn_connected_bonus * connected_pawns;
                
                pop_bit(bb, sq);
            }

            if (side == WHITE) 
            {
                piece = make_piece(PAWN, BLACK);
                goto repeat;
            }

            return score[WHITE] - score[BLACK];
        }

    } // namespace Eval

} // namespace Clovis