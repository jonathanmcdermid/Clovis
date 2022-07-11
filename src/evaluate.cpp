#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        enum GamePhase : int {
            MG, EG,
            PHASE_N = 2
        };
        
        Score pawn_table[32] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(16,154), Score(59,163), Score(42,110), Score(57,92),
        Score(-17,13), Score(-9,16), Score(21,9), Score(26,11),
        Score(-29,-3), Score(-10,-8), Score(-7,-7), Score(12,-16),
        Score(-37,-13), Score(-29,-12), Score(-11,-15), Score(6,-18),
        Score(-24,-22), Score(-20,-19), Score(-9,-15), Score(-6,-10),
        Score(-30,-19), Score(-13,-18), Score(-11,-5), Score(-18,-3),
        Score(1,-1), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[32] = {
                Score(-151,-61), Score(-75,-43), Score(-69,-14), Score(12,-29),
                Score(-47,-31), Score(-10,-15), Score(81,-30), Score(31,-3),
                Score(13,-36), Score(88,-25), Score(75,4), Score(92,-1),
                Score(32,-21), Score(42,3), Score(57,18), Score(61,22),
                Score(11,-20), Score(38,-4), Score(44,16), Score(43,23),
                Score(5,-24), Score(31,-11), Score(43,-3), Score(48,9),
                Score(6,-43), Score(-10,-20), Score(25,-12), Score(36,-9),
                Score(-40,-44), Score(13,-51), Score(0,-23), Score(8,-17),
        };
        Score bishop_table[32] = {
                Score(-41,-3), Score(-14,-7), Score(-84,6), Score(-58,6),
                Score(-33,3), Score(8,-3), Score(3,1), Score(-1,-4),
                Score(0,7), Score(45,-8), Score(45,-7), Score(38,-11),
                Score(11,2), Score(11,4), Score(25,4), Score(40,1),
                Score(15,-4), Score(18,-1), Score(10,7), Score(35,1),
                Score(18,-3), Score(27,0), Score(35,-1), Score(18,9),
                Score(19,-12), Score(43,-13), Score(30,-7), Score(21,0),
                Score(-8,-9), Score(11,2), Score(16,-8), Score(8,2),
        };
        Score rook_table[32] = {
                Score(-22,37), Score(-19,35), Score(-50,47), Score(3,32),
                Score(-14,35), Score(-14,36), Score(29,25), Score(27,21),
                Score(-42,33), Score(-11,29), Score(-29,32), Score(-31,31),
                Score(-49,35), Score(-43,29), Score(-26,33), Score(-30,27),
                Score(-56,31), Score(-34,27), Score(-43,30), Score(-30,23),
                Score(-49,20), Score(-33,22), Score(-29,17), Score(-28,18),
                Score(-51,23), Score(-26,16), Score(-25,19), Score(-19,19),
                Score(-17,8), Score(-22,21), Score(-1,16), Score(4,13),
        };
        Score queen_table[32] = {
                Score(7,92), Score(22,96), Score(34,100), Score(57,87),
                Score(12,79), Score(-31,113), Score(22,102), Score(-16,136),
                Score(36,74), Score(32,72), Score(18,88), Score(8,120),
                Score(9,104), Score(-8,112), Score(1,92), Score(-4,104),
                Score(15,84), Score(6,103), Score(8,90), Score(2,94),
                Score(9,94), Score(30,58), Score(18,81), Score(19,66),
                Score(12,67), Score(28,44), Score(43,36), Score(36,52),
                Score(37,38), Score(31,37), Score(32,45), Score(44,44),
        };
        Score king_table[32] = {
                Score(-27,-41), Score(129,-44), Score(70,-20), Score(22,-23),
                Score(43,-13), Score(36,8), Score(28,19), Score(66,2),
                Score(34,-4), Score(105,11), Score(98,17), Score(36,9),
                Score(-37,-3), Score(-1,20), Score(-2,27), Score(-28,29),
                Score(-74,-6), Score(-47,11), Score(-68,32), Score(-118,45),
                Score(-38,-8), Score(-26,8), Score(-60,24), Score(-86,35),
                Score(11,-24), Score(2,-6), Score(-54,17), Score(-82,26),
                Score(11,-58), Score(30,-34), Score(-26,-11), Score(-11,-23),
        };
        Score passed_pawn_bonus[32] = {
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(0, 67),  Score(2, 34),  Score(37, 55),  Score(29, 61),
                 Score(25, 131),  Score(22, 117),  Score(26, 92),  Score(10, 64),
                 Score(18, 69),  Score(3, 68),  Score(11, 43),  Score(5, 36),
                 Score(18, 38),  Score(0, 37),  Score(0, 22),  Score(0, 18),
                 Score(14, 12),  Score(8, 15),  Score(0, 4),  Score(0, 2),
                 Score(3, 10),  Score(18, 6),  Score(20, 0),  Score(0, 4),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
        Score piece_value[7] = { Score(0, 0), Score(100, 100), Score(300, 300), Score(315, 315), Score(500, 500), Score(900, 900), Score(10000, 10000), };
        Score double_pawn_penalty = Score(1, 4);
        Score isolated_pawn_penalty = Score(20, 8);
        Score bishop_pair_bonus = Score(43, 45);
        Score rook_open_file_bonus = Score(29, 0);
        Score rook_semi_open_file_bonus = Score(5, 13);
        Score mobility[7] = { Score(0, 0), Score(0, 0), Score(0, 0), Score(5, 4), Score(6, 2), Score(2, 8), Score(0, 0), };
        short mobility_reduction[7] = { 0, 0, 4, 7, 7, 14, 0, };

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
                        score[side] += mobility[KNIGHT] * (count_bits(Bitboards::knight_attacks[sq] & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])) - mobility_reduction[KNIGHT]);
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
                        score[side] += mobility[BISHOP] * (count_bits(Bitboards::get_bishop_attacks(pos.occ_bitboard[BOTH], sq) & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])) - mobility_reduction[BISHOP]);
                        score[side] += score_table[piece][sq];
                        pop_bit(bb, sq);
                    }
                    break;
                case ROOK:
                    while (bb)
                    {
                        sq = get_lsb_index(bb);
                        score[side] += mobility[ROOK] * (count_bits(Bitboards::get_rook_attacks(pos.occ_bitboard[BOTH], sq) & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])) - mobility_reduction[ROOK]);
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
                        score[side] += mobility[QUEEN] * (count_bits(Bitboards::get_queen_attacks(pos.occ_bitboard[BOTH], sq) & ~pos.occ_bitboard[side] & ~(Bitboards::pawn_attacks[side][sq] & pos.piece_bitboard[make_piece(PAWN, other_side(side))])) - mobility_reduction[QUEEN]);
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