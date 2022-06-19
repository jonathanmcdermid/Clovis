#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        enum GamePhase : int {
            MG, EG,
            PHASE_N = 2
        };

        Score piece_value[7] = { Score(0,0), Score(83,98), Score(328,273), Score(365,303), Score(473,522), Score(968,976), Score(10000,10000) };

        Score pawn_table[SQ_N] = {
            Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),
            Score(13,52),   Score(36,40),   Score(-13,22),  Score(9,-4),    Score(-2,7),    Score(33,-1),   Score(-64,41),  Score(-65,62),
            Score(1,34),    Score(-7,34),   Score(12,11),   Score(-2,-18),  Score(35,-30),  Score(48,-7),   Score(20,14),   Score(-5,20),
            Score(-13,15),  Score(3,6),     Score(4,-6),    Score(25,-24),  Score(23,-17),  Score(17,-11),  Score(6,1),     Score(-19,4),
            Score(-22,3),   Score(-19,2),   Score(-1,-12),  Score(14,-20),  Score(19,-18),  Score(4,-13),   Score(-10,-6),  Score(-22,-9),
            Score(-14,-8),  Score(-20,-2),  Score(-3,-14),  Score(-4,-11),  Score(6,-8),    Score(5,-10),   Score(13,-15),  Score(-5,-17),
            Score(-20,-2),  Score(-16,-4),  Score(-22,0),   Score(-12,-10), Score(-8,1),    Score(20,-8),   Score(18,-12),  Score(-11,-19),
            Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),     Score(0,0),
        };

        Score knight_table[SQ_N] = {
            Score(-153,-35),Score(-52,-26), Score(-33,5),   Score(-26,-17),  Score(23,-10),  Score(-85,-15), Score(-36,-40), Score(-96,-73),
            Score(-88,1),   Score(-41,10),  Score(43,-11),  Score(12,13),    Score(4,3),     Score(34,-13),  Score(-6,-7),   Score(-27,-28),  
            Score(-44,-6),  Score(30,-6),   Score(17,15),   Score(30,12),    Score(40,7),    Score(59,5),    Score(40,-6),   Score(23,-22),   
            Score(-18,6),   Score(7,13),    Score(-11,27),  Score(30,24),    Score(12,25),   Score(44,17),   Score(5,15),    Score(16,0),     
            Score(-8,5),    Score(5,7),     Score(13,24),   Score(5,32),     Score(21,23),   Score(11,24),   Score(12,21),   Score(-8,3),  
            Score(-18,0),   Score(-3,9),    Score(13,1),    Score(17,20),    Score(27,14),   Score(23,-3),   Score(26,-12),  Score(-9,-3), 
            Score(-15,-12), Score(-33,1),   Score(-5,6),    Score(12,4),     Score(15,8),    Score(23,-8),   Score(-5,-2),   Score(-2,-23),  
            Score(-70,-6),  Score(-6,-23),  Score(-33,-2),  Score(-17,9),    Score(6,-2),    Score(-6,-2),   Score(-4,-26),  Score(-15,-38),    
        };

        Score bishop_table[SQ_N] = {
            Score(-31,-3),  Score(-8,-9),   Score(-88,7),   Score(-41,0),   Score(-33,7),    Score(-33,-2),  Score(-7,-3),   Score(-1,-15),
            Score(-34,7),   Score(-1,4),    Score(-27,14),  Score(-34,-1),  Score(10,2),     Score(10,-2),   Score(26,-2),   Score(-58,-3),
            Score(-23,13),  Score(22,-1),   Score(30,-1),   Score(18,0),    Score(17,-3),    Score(30,2),    Score(19,6),    Score(-8,12),
            Score(-7,8),    Score(-2,14),   Score(2,14),    Score(30,7),    Score(20,8),     Score(18,6),    Score(2,4),     Score(-10,12),
            Score(-4,7),    Score(11,5),    Score(2,14),    Score(21,13),   Score(24,0),     Score(2,7),     Score(4,0),     Score(10,2),
            Score(3,2),     Score(19,4),    Score(19,8),    Score(7,10),    Score(16,11),    Score(31,-2),   Score(20,3),    Score(5,4),
            Score(13,-1),   Score(25,-8),   Score(17,-1),   Score(12,1),    Score(17,6),     Score(27,-1),   Score(38,-3),   Score(11,-15),
            Score(-20,-4),  Score(12,2),    Score(7,-3),    Score(4,5),     Score(11,3),     Score(3,1),     Score(-13,6),   Score(-16,1),
        };

        Score rook_table[SQ_N] = {
            Score(10,15),   Score(13,11),   Score(-9,20),   Score(14,15),   Score(13,17),    Score(-14,18),  Score(0,15),    Score(0,13),
            Score(10,14),   Score(8,17),    Score(25,16),   Score(29,13),   Score(32,5),     Score(31,10),   Score(0,15),    Score(8,13),
            Score(-14,13),  Score(6,10),    Score(1,10),    Score(5,10),    Score(-11,8),    Score(13,3),    Score(34,-1),   Score(-5,-2),
            Score(-26,12),  Score(-19,9),   Score(0,14),    Score(2,6),     Score(-2,6),     Score(10,8),    Score(-23,6),   Score(-26,13),
            Score(-33,12),  Score(-24,12),  Score(-13,13),  Score(-10,7),   Score(-3,2),     Score(-17,4),   Score(-9,2),    Score(-31,4),
            Score(-32,5),   Score(-20,8),   Score(-6,-1),   Score(-9,3),    Score(-3,0),     Score(-2,-3),   Score(-13,1),   Score(-27,-4),
            Score(-30,4),   Score(-9,0),    Score(-6,2),    Score(3,5),     Score(9,-5),     Score(6,-3),    Score(-5,-5),   Score(-54,7),
            Score(-6,-5),   Score(1,1),     Score(12,2),    Score(22,-1),   Score(22,-4),    Score(18,-6),   Score(-23,3),   Score(-8,-18),
        };

        Score queen_table[SQ_N] = {
            Score(-22,-13), Score(-16,17),  Score(-6,16),   Score(-8,14),   Score(33,14),   Score(23,13),    Score(10,8),    Score(25,22),
            Score(-26,-10), Score(-46,3),   Score(-16,19),  Score(5,21),    Score(-27,35),  Score(4,25),     Score(3,18),    Score(22,12),
            Score(-15,-9),  Score(-16,-12), Score(-5,-12),  Score(-19,28),  Score(8,29),    Score(32,21),    Score(11,27),   Score(26,20),
            Score(-28,14),  Score(-31,17),  Score(-26,8),   Score(-26,16),  Score(-16,30),  Score(-9,25),    Score(-12,44),  Score(-18,41),
            Score(-11,-6),  Score(-28,17),  Score(-14,8),   Score(-18,27),  Score(-10,14),  Score(-10,21),   Score(-11,34),  Score(-11,33),
            Score(-19,12),  Score(4,-20),   Score(-5,10),   Score(-1,-2),   Score(-2,5),    Score(1,16),     Score(6,22),    Score(1,17),
            Score(-21,1),   Score(-3,-12),  Score(13,-14),  Score(14,-12),  Score(19,-8),   Score(22,-15),   Score(2,-24),   Score(14,-18),
            Score(4,-17),   Score(2,-24),   Score(13,-19),  Score(24,-13),  Score(6,1),     Score(-8,-12),   Score(-6,-9),   Score(-31,-30),
        };

        Score king_table[SQ_N] = {
            Score(-54,-71),     Score(30,-40),     Score(48,-23),     Score(29,-22),     Score(-21,-8),     Score(2,15),     Score(17,2),     Score(15,-17),
            Score(36,-14),     Score(33,16),     Score(20,14),     Score(54,12),     Score(34,15),     Score(30,33),     Score(-1,23),     Score(-15,10),
            Score(31,5),     Score(35,17),     Score(48,19),     Score(26,16),     Score(32,17),     Score(46,44),     Score(50,40),     Score(9,9),
            Score(20,-13),     Score(28,19),     Score(28,24),     Score(22,28),     Score(24,25),     Score(26,31),     Score(30,23),     Score(-2,1),
            Score(-14,-19),     Score(34,-3),     Score(12,23),     Score(7,27),     Score(9,27),     Score(7,24),     Score(4,10),     Score(-20,-10),
            Score(0,-18),     Score(10,0),     Score(8,14),     Score(9,20),     Score(9,23),     Score(8,17),     Score(17,6),     Score(-16,-4),
            Score(0,-24),     Score(16,-7),     Score(-1,10),     Score(-35,17),     Score(-14,17),     Score(-5,11),     Score(12,0),     Score(3,-12),
            Score(-38,-45),     Score(24,-29),     Score(13,-17),     Score(-61,1),     Score(-3,-15),     Score(-29,-4),     Score(19,-21),     Score(-6,-39),
        };

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

        Score double_pawn_penalty = Score(-1,-17);

        Score isolated_pawn_penalty = Score(-19,-5);

        Score passed_pawn_bonus[RANK_N] = { Score(0,0), Score(3,1), Score(4,5), Score(8,25), Score(13,50), Score(48,103), Score(109,149), Score(0,0) };

        Score bishop_pair_bonus = Score(20, 36);

        //int semi_open_file_score = 10;
        //
        Score rook_open_file_score = Score(15,1);

        int king_shield_bonus = 1;

        Bitboard file_masks[SQ_N];

        Bitboard rank_masks[SQ_N];

        Bitboard isolated_masks[SQ_N];

        Bitboard passed_masks[COLOUR_N][SQ_N];

        void init_eval() {
            for (PieceType pt = PAWN; pt <= KING; ++pt) 
            {
                for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) 
                {
                    score_table[make_piece(pt, WHITE)][sq] = piece_value[pt] + piece_table[pt][flip_square(sq)];
                    score_table[make_piece(pt, BLACK)][sq] = piece_value[pt] + piece_table[pt][sq];
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
                if (pt == BISHOP)
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
                else
                {
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                }
                if (side == WHITE) {
                    piece = make_piece(pt, BLACK);
                    goto repeat;
                }
            }

            int game_phase = pos.get_game_phase();

            Score score_comb = score[pos.side] - score[!pos.side];

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
                    score[side] += double_pawn_penalty * double_pawns;
                }

                if ((pos.piece_bitboard[piece] & isolated_masks[sq]) == 0)
                {
                    score[side] += isolated_pawn_penalty;
                }

                if ((passed_masks[side][sq] & pos.piece_bitboard[make_piece(PAWN, Colour(!side))]) == 0) {
                    score[side] += passed_pawn_bonus[relative_rank(side, sq)];
                }

                pop_bit(bb, sq);
            }

            if (side == WHITE) {
                piece = make_piece(PAWN, BLACK);
                goto repeat;
            }

            return score[WHITE] - score[BLACK];
        }

    } // namespace Eval

} // namespace Clovis