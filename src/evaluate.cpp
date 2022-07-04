#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        enum GamePhase : int {
            MG, EG,
            PHASE_N = 2
        };

        // pawn, rook, king, passed pawn failed after tuning
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
                Score(-211,-20), Score(-103,16), Score(-106,44), Score(-4,16), Score(30,12), Score(-97,41), Score(-7,-3), Score(-147,-37),
                Score(0,-20), Score(-21,7), Score(33,14), Score(51,36), Score(45,35), Score(83,-3), Score(-13,4), Score(26,-17),
                Score(-22,-1), Score(35,21), Score(47,40), Score(65,38), Score(88,32), Score(110,34), Score(66,23), Score(53,-6),
                Score(15,0), Score(28,32), Score(44,43), Score(51,58), Score(42,59), Score(63,51), Score(41,45), Score(40,9),
                Score(5,-3), Score(8,30), Score(30,46), Score(30,50), Score(35,50), Score(25,50), Score(41,31), Score(15,9),
                Score(-6,-28), Score(12,10), Score(16,20), Score(24,33), Score(27,32), Score(19,19), Score(23,9), Score(3,-25),
                Score(-21,-16), Score(-8,0), Score(2,-1), Score(10,10), Score(12,10), Score(13,-8), Score(-11,1), Score(0,-18),
                Score(-104,-39), Score(-8,-55), Score(-29,-13), Score(-11,-7), Score(-6,-7), Score(-4,-15), Score(-4,-43), Score(-94,-5),
        };
        Score bishop_table[SQ_N] = {
                Score(-67,19), Score(-65,24), Score(-117,28), Score(-103,31), Score(-99,22), Score(-133,31), Score(21,4), Score(-48,24),
                Score(-51,4), Score(-18,17), Score(-17,13), Score(-30,20), Score(-9,13), Score(-3,14), Score(-11,11), Score(-13,-2),
                Score(-12,0), Score(4,12), Score(27,12), Score(24,14), Score(25,18), Score(51,20), Score(41,14), Score(14,5),
                Score(-20,2), Score(12,15), Score(10,15), Score(36,24), Score(22,30), Score(29,15), Score(9,22), Score(-4,7),
                Score(-18,-5), Score(-1,12), Score(0,22), Score(18,19), Score(23,19), Score(-2,17), Score(3,8), Score(2,-13),
                Score(-7,-11), Score(-4,8), Score(-2,12), Score(5,12), Score(-2,12), Score(3,3), Score(1,-8), Score(6,-10),
                Score(-8,-18), Score(5,-20), Score(1,-8), Score(-11,-6), Score(-7,-9), Score(4,-27), Score(14,-16), Score(-1,-54),
                Score(-5,-28), Score(-5,-17), Score(-23,-35), Score(-31,-17), Score(-34,-14), Score(-22,-23), Score(-10,-24), Score(-9,-17),
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
                Score(-34,69), Score(3,58), Score(16,69), Score(40,59), Score(34,77), Score(92,59), Score(98,59), Score(98,50),
                Score(-23,42), Score(-60,88), Score(-20,78), Score(-45,119), Score(-14,125), Score(42,96), Score(-3,129), Score(47,86),
                Score(-21,21), Score(-10,31), Score(-25,67), Score(-4,74), Score(3,109), Score(81,87), Score(86,80), Score(42,111),
                Score(-5,3), Score(-10,42), Score(-15,49), Score(-11,75), Score(-9,108), Score(17,105), Score(29,98), Score(34,70),
                Score(-4,-5), Score(0,30), Score(-2,33), Score(4,57), Score(-4,65), Score(14,54), Score(23,34), Score(26,46),
                Score(-5,-13), Score(4,4), Score(1,26), Score(4,7), Score(6,5), Score(10,17), Score(27,-10), Score(15,-11),
                Score(-10,-16), Score(6,-16), Score(7,-30), Score(6,-14), Score(13,-34), Score(27,-70), Score(33,-73), Score(16,-55),
                Score(10,-27), Score(-3,-32), Score(5,-48), Score(12,-38), Score(13,-46), Score(-9,-40), Score(11,-70), Score(-7,-40),
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
        Score bishop_pair_bonus = Score(10, 68);
        Score rook_open_file_bonus = Score(34, 17);
        Score rook_semi_open_file_bonus = Score(18, 11);

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
                max_score = DRAW_SCORE;
                if (pos.is_insufficient(other_side(pos.side)))
                    return DRAW_SCORE;
            }
            else if (pos.is_insufficient(other_side(pos.side)))
                min_score = - DRAW_SCORE;

            Score score[COLOUR_N];

            Piece piece; 
            Square sq;
            Bitboard bb;
            Colour side;

            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                piece = make_piece(pt, WHITE);

            repeat:

                side = get_side(piece);
                bb = pos.piece_bitboard[piece];
                switch (pt) {
                case PAWN:
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                case KNIGHT:
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                case BISHOP:
                    if (count_bits(bb) >= 2)
                        score[side] += bishop_pair_bonus;

                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                case ROOK:
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        if ((file_masks[sq] & (pos.piece_bitboard[W_PAWN] | pos.piece_bitboard[B_PAWN])) == 0)
                            score[side] += rook_open_file_bonus;
                        else if ((file_masks[sq] & pos.piece_bitboard[make_piece(PAWN, side)]) == 0)
                            score[side] += rook_semi_open_file_bonus;
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                case QUEEN:
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                case KING:
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                default:
                    break;
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

                if ((passed_masks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))]) == 0) 
                {
                    score[side] += passed_pawn_bonus[relative_rank(side, sq)];
                }
                
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