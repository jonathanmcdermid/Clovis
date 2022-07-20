#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        enum GamePhase : int {
            MG, EG,
            PHASE_N = 2
        };
        
        Score pawn_table[32] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(23,162), Score(66,171), Score(50,117), Score(63,99),
        Score(-13,15), Score(-1,17), Score(29,11), Score(29,15),
        Score(-27,-3), Score(-7,-6), Score(-5,-5), Score(15,-13),
        Score(-37,-12), Score(-26,-11), Score(-10,-13), Score(9,-16),
        Score(-25,-21), Score(-18,-17), Score(-8,-12), Score(-6,-6),
        Score(-32,-18), Score(-11,-17), Score(-10,-2), Score(-17,1),
        Score(1,-1), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[32] = {
                Score(-150,-53), Score(-60,-40), Score(-68,-7), Score(26,-26),
                Score(-33,-24), Score(-3,-8), Score(91,-28), Score(36,1),
                Score(21,-29), Score(101,-22), Score(76,6), Score(96,2),
                Score(46,-15), Score(46,11), Score(58,21), Score(62,26),
                Score(21,-10), Score(46,2), Score(46,20), Score(45,25),
                Score(15,-16), Score(41,-8), Score(48,2), Score(51,13),
                Score(16,-36), Score(-3,-13), Score(33,-8), Score(46,-5),
                Score(-25,-41), Score(23,-43), Score(10,-18), Score(18,-11),
        };
        Score bishop_table[32] = {
                Score(-45,-11), Score(-14,-15), Score(-92,-3), Score(-65,1),
                Score(-36,-5), Score(4,-5), Score(3,-1), Score(0,-8),
                Score(0,-2), Score(40,-11), Score(48,-8), Score(37,-12),
                Score(9,-5), Score(10,0), Score(23,3), Score(43,1),
                Score(12,-9), Score(17,-4), Score(9,4), Score(35,0),
                Score(16,-12), Score(23,-5), Score(33,-3), Score(18,6),
                Score(17,-21), Score(42,-21), Score(27,-12), Score(20,-8),
                Score(-15,-17), Score(7,-6), Score(11,-17), Score(0,-5),
        };
        Score rook_table[32] = {
                Score(-22,37), Score(-22,37), Score(-51,47), Score(4,32),
                Score(-13,36), Score(-13,36), Score(29,24), Score(26,22),
                Score(-40,33), Score(-10,29), Score(-27,31), Score(-29,31),
                Score(-51,34), Score(-44,29), Score(-25,33), Score(-29,27),
                Score(-56,30), Score(-34,25), Score(-45,31), Score(-30,24),
                Score(-51,21), Score(-34,23), Score(-29,16), Score(-30,19),
                Score(-56,23), Score(-27,15), Score(-26,18), Score(-20,18),
                Score(-22,7), Score(-25,19), Score(-5,15), Score(0,11),
        };
        Score queen_table[32] = {
                Score(3,91), Score(21,95), Score(32,100), Score(56,88),
                Score(15,79), Score(-33,114), Score(23,101), Score(-18,135),
                Score(40,74), Score(35,70), Score(20,86), Score(7,121),
                Score(11,104), Score(-7,115), Score(2,95), Score(-3,104),
                Score(16,83), Score(7,103), Score(7,90), Score(2,95),
                Score(12,93), Score(31,59), Score(18,81), Score(21,66),
                Score(8,69), Score(27,44), Score(45,36), Score(38,53),
                Score(35,38), Score(30,35), Score(31,44), Score(44,43),
        };
        Score king_table[32] = {
                Score(-25,-43), Score(132,-43), Score(77,-19), Score(20,-22),
                Score(35,-11), Score(35,9), Score(27,20), Score(67,2),
                Score(35,-4), Score(107,12), Score(98,18), Score(38,9),
                Score(-35,-4), Score(-2,23), Score(-1,29), Score(-29,31),
                Score(-76,-6), Score(-46,11), Score(-68,33), Score(-118,46),
                Score(-38,-8), Score(-26,8), Score(-60,25), Score(-87,36),
                Score(11,-24), Score(2,-6), Score(-54,18), Score(-80,26),
                Score(11,-59), Score(31,-34), Score(-27,-13), Score(-11,-24),
        };
        Score passed_pawn_bonus[32] = {
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(7, 70),  Score(9, 37),  Score(42, 57),  Score(29, 64),
                 Score(25, 139),  Score(23, 124),  Score(28, 95),  Score(13, 67),
                 Score(18, 76),  Score(6, 71),  Score(14, 47),  Score(6, 37),
                 Score(21, 41),  Score(0, 40),  Score(0, 25),  Score(0, 20),
                 Score(15, 15),  Score(11, 18),  Score(0, 7),  Score(0, 4),
                 Score(4, 13),  Score(21, 9),  Score(23, 0),  Score(0, 3),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
        Score piece_value[7] = { Score(0, 0), Score(100, 100), Score(300, 300), Score(315, 315), Score(500, 500), Score(900, 900), Score(10000, 10000), };
        Score double_pawn_penalty = Score(1, 4);
        Score isolated_pawn_penalty = Score(21, 9);
        Score bishop_pair_bonus = Score(35, 42);
        Score rook_open_file_bonus = Score(36, 0);
        Score rook_semi_open_file_bonus = Score(8, 14);
        Score mobility[7] = { Score(0, 0), Score(0, 0), Score(3, 1), Score(5, 2), Score(4, 3), Score(2, 8), Score(0, 0), };

        Score passed_table[SQ_N];

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
                for (Square sq = SQ_ZERO; sq < 32; ++sq)
                {
                    int r = sq / 4;
                    int f = sq & 0x3;

                    score_table[make_piece(pt, WHITE)][8 * (7 - r) + f] = piece_value[pt] + piece_table[pt][sq];
                    score_table[make_piece(pt, WHITE)][8 * (7 - r) + (7 - f)] = piece_value[pt] + piece_table[pt][sq];

                    score_table[make_piece(pt, BLACK)][8 * r + f] = piece_value[pt] + piece_table[pt][sq];
                    score_table[make_piece(pt, BLACK)][8 * r + (7 - f)] = piece_value[pt] + piece_table[pt][sq];
                }
            }
            for (Square sq = SQ_ZERO; sq < 32; ++sq)
            {
                int r = sq / 4;
                int f = sq & 0x3;

                passed_table[8 * (7 - r) + f] = passed_pawn_bonus[sq];
                passed_table[8 * (7 - r) + (7 - f)] = passed_pawn_bonus[sq];
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

        Score evaluate(const Position& pos)
        {

            Score score[COLOUR_N];

            Piece piece; 
            Square sq;
            Bitboard bb;
            Colour side;

            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                side = WHITE;

            repeat:

                piece = make_piece(pt, side);

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
                        score[side] += mobility[KNIGHT] * (count_bits(Bitboards::knight_attacks[sq] & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])));
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
                        score[side] += mobility[BISHOP] * (count_bits(Bitboards::get_bishop_attacks(pos.occ_bitboard[BOTH], sq) & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])));
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                case ROOK:
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += mobility[ROOK] * (count_bits(Bitboards::get_rook_attacks(pos.occ_bitboard[BOTH], sq) & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])));
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
                        score[side] += mobility[QUEEN] * (count_bits(Bitboards::get_queen_attacks(pos.occ_bitboard[BOTH], sq) & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])));
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
                    side = BLACK;
                    goto repeat;
                }
            }

            return score[WHITE] - score[BLACK];
        }

        Score evaluate_pawns(const Position& pos)
        {
            Piece piece;
            Square sq;
            Bitboard bb;
            Colour side;

            Score score[COLOUR_N];

            side = WHITE;

        repeat:

            piece = make_piece(PAWN, side);

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
                    score[side] += passed_table[make_square(file_of(sq), relative_rank(side, sq))];
                }
                
                pop_bit(bb, sq);
            }

            if (side == WHITE)
            {
                side = BLACK;
                goto repeat;
            }

            return score[WHITE] - score[BLACK];
        }

    } // namespace Eval

} // namespace Clovis