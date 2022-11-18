#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        Score pawn_table[32] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(142,265), Score(174,275), Score(151,218), Score(162,199),
        Score(96,114), Score(108,117), Score(143,109), Score(139,107),
        Score(81,99), Score(99,97), Score(101,97), Score(119,88),
        Score(67,90), Score(77,91), Score(94,90), Score(112,90),
        Score(77,82), Score(84,84), Score(95,90), Score(98,98),
        Score(67,87), Score(91,87), Score(91,101), Score(88,103),
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[32] = {
                Score(171,246), Score(242,264), Score(239,295), Score(328,278),
                Score(270,276), Score(303,296), Score(395,278), Score(341,306),
                Score(325,272), Score(405,283), Score(380,313), Score(396,310),
                Score(354,285), Score(356,313), Score(365,327), Score(365,335),
                Score(334,290), Score(354,306), Score(358,325), Score(357,332),
                Score(330,283), Score(357,294), Score(363,304), Score(367,317),
                Score(330,265), Score(321,286), Score(349,296), Score(362,298),
                Score(290,264), Score(335,256), Score(329,283), Score(336,288),
        };
        Score bishop_table[32] = {
                Score(291,297), Score(306,298), Score(228,312), Score(257,311),
                Score(273,309), Score(319,308), Score(313,312), Score(302,307),
                Score(307,314), Score(346,306), Score(348,308), Score(337,306),
                Score(319,310), Score(322,315), Score(328,318), Score(342,317),
                Score(327,305), Score(329,309), Score(321,320), Score(343,316),
                Score(329,304), Score(345,306), Score(350,310), Score(332,319),
                Score(339,290), Score(361,293), Score(345,301), Score(340,308),
                Score(305,296), Score(326,306), Score(331,295), Score(328,305),
        };
        Score rook_table[32] = {
                Score(467,537), Score(481,533), Score(445,544), Score(496,531),
                Score(479,535), Score(476,536), Score(513,526), Score(520,519),
                Score(457,530), Score(489,526), Score(471,528), Score(464,529),
                Score(449,532), Score(458,526), Score(476,531), Score(473,524),
                Score(447,527), Score(464,524), Score(460,527), Score(470,522),
                Score(449,519), Score(467,520), Score(472,515), Score(471,516),
                Score(443,522), Score(475,511), Score(470,517), Score(478,518),
                Score(477,510), Score(470,521), Score(492,514), Score(493,511),
        };
        Score queen_table[32] = {
                Score(915,984), Score(907,1002), Score(916,1008), Score(933,996),
                Score(907,988), Score(870,1013), Score(893,1022), Score(860,1050),
                Score(932,977), Score(923,983), Score(912,993), Score(891,1028),
                Score(906,1012), Score(893,1022), Score(895,1002), Score(889,1014),
                Score(923,984), Score(906,1011), Score(910,991), Score(904,1002),
                Score(919,996), Score(941,963), Score(922,992), Score(924,979),
                Score(919,972), Score(937,950), Score(954,945), Score(944,963),
                Score(942,944), Score(930,953), Score(936,957), Score(952,950),
        };
        Score king_table[32] = {
                Score(-54,-39), Score(112,-41), Score(45,-12), Score(21,-23),
                Score(-4,-4), Score(26,11), Score(34,19), Score(66,5),
                Score(32,-2), Score(109,14), Score(110,17), Score(44,9),
                Score(-37,-2), Score(-3,25), Score(2,30), Score(-25,32),
                Score(-79,-5), Score(-41,12), Score(-65,34), Score(-101,45),
                Score(-33,-9), Score(-10,6), Score(-38,23), Score(-67,35),
                Score(11,-25), Score(15,-8), Score(-37,15), Score(-62,24),
                Score(1,-60), Score(28,-37), Score(-30,-10), Score(-14,-22),
        };
        Score passed_pawn_bonus[32] = {
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(4, 72),  Score(10, 36),  Score(45, 61),  Score(41, 67),
                 Score(22, 143),  Score(15, 131),  Score(22, 102),  Score(12, 78),
                 Score(18, 75),  Score(4, 71),  Score(12, 48),  Score(10, 38),
                 Score(24, 41),  Score(0, 41),  Score(0, 23),  Score(0, 16),
                 Score(17, 13),  Score(10, 17),  Score(0, 5),  Score(0, 0),
                 Score(6, 13),  Score(21, 7),  Score(21, 0),  Score(0, 2),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
        Score double_pawn_penalty = Score(2, 9);
        Score isolated_pawn_penalty = Score(21, 9);
        Score bishop_pair_bonus = Score(34, 48);
        Score rook_open_file_bonus = Score(43, 0);
        Score rook_semi_open_file_bonus = Score(14, 14);
        Score king_safety_reduction_factor = Score(6, 6);
        Score mobility[7] = { Score(0, 0), Score(0, 0), Score(3, 0), Score(6, 2), Score(2, 4), Score(2, 7), Score(0, 0), };
        Score inner_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(3, 9), Score(7, 2), Score(6, 1), Score(5, 0), Score(0, 0), };
        Score outer_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(3, 1), Score(1, 3), Score(2, 0), Score(2, 1), Score(0, 0), };

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

        Score* passed_table[SQ_N];

        Score* score_table[15][SQ_N];

        Bitboard file_masks[SQ_N];

        Bitboard rank_masks[SQ_N];

        Bitboard isolated_masks[SQ_N];

        Bitboard passed_masks[COLOUR_N][SQ_N];

        KingZone king_zones[SQ_N];

        inline bool doubled_pawn(Bitboard bb, Square sq) {
            return popcnt(bb & file_masks[sq]) >= 2;
        }

        inline bool isolated_pawn(Bitboard bb, Square sq) {
            return !(bb & isolated_masks[sq]);
        }

        inline bool passed_pawn(Bitboard bb, Square sq, Colour side) {
            return !(bb & passed_masks[side][sq]);
        }

        //this is an old attempt at knight outpost conditions that did not produce an improvement
        //inline bool knight_outpost(Bitboard enemy_pawns, Bitboard friendly_pawns, Square sq, Colour side) {
        //    return (Bitboards::pawn_attacks[other_side(side)][sq] & friendly_pawns) && (outpost_masks[side] & sq) && !(enemy_pawns & passed_masks[side][sq]);
        //}

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

                    score_table[make_piece(pt, WHITE)][8 * (7 - r) + f] = &piece_table[pt][sq];
                    score_table[make_piece(pt, WHITE)][8 * (7 - r) + (7 - f)] = &piece_table[pt][sq];

                    score_table[make_piece(pt, BLACK)][8 * r + f] = &piece_table[pt][sq];
                    score_table[make_piece(pt, BLACK)][8 * r + (7 - f)] = &piece_table[pt][sq];
                }
            }
            for (Square sq = SQ_ZERO; sq < 32; ++sq)
            {
                int r = sq / 4;
                int f = sq & 0x3;

                passed_table[8 * (7 - r) + f] = &passed_pawn_bonus[sq];
                passed_table[8 * (7 - r) + (7 - f)] = &passed_pawn_bonus[sq];
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
            for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
            {
                king_zones[sq].inner_ring = Bitboards::get_attacks(0ULL, sq, KING);
                Bitboard bb = king_zones[sq].inner_ring;

                // this probably isnt the most efficient way of generating outer king ring
                while (bb)
                {
                    Square sq2 = pop_lsb(bb);

                    king_zones[sq].outer_ring |= Bitboards::get_attacks(0ULL, sq2, KING);
                }

                king_zones[sq].outer_ring = king_zones[sq].outer_ring & ~(king_zones[sq].inner_ring | sq);
            }
        }

        Score evaluate(const Position& pos)
        {
            Score score[COLOUR_N];

            int n_king_attackers[COLOUR_N] = { 0,0 };
            Score king_attack_weight[COLOUR_N];
            Square king_sq[COLOUR_N] = { lsb(pos.piece_bitboard[W_KING]), lsb(pos.piece_bitboard[B_KING]) };

            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                Square sq;
                Colour us = WHITE, them;
                Piece piece;

            repeat:

                them = other_side(us);
                piece = make_piece(pt, us);

                Bitboard bb = pos.piece_bitboard[piece];

                while (bb)
                {
                    sq = pop_lsb(bb);
                    score[us] += *score_table[piece][sq];
                    if (pt > PAWN && pt < KING)
                    {
                        Bitboard attacks;

                        if (pt == BISHOP)
                            attacks = Bitboards::get_attacks(pos.occ_bitboard[BOTH] & ~(pos.piece_bitboard[W_QUEEN] | pos.piece_bitboard[B_QUEEN]), sq, BISHOP);
                        else if (pt == ROOK)
                            attacks = Bitboards::get_attacks(pos.occ_bitboard[BOTH] & ~(pos.piece_bitboard[W_QUEEN] | pos.piece_bitboard[B_QUEEN] | pos.piece_bitboard[W_ROOK] | pos.piece_bitboard[B_ROOK]), sq, ROOK);
                        else
                            attacks = Bitboards::get_attacks(pos.occ_bitboard[BOTH], sq, pt);

                        score[us] += mobility[pt] * (popcnt(attacks & ~pos.occ_bitboard[us]));

                        if (pt == ROOK)
                        {
                            if (!(file_masks[sq] & (pos.piece_bitboard[W_PAWN] | pos.piece_bitboard[B_PAWN])))
                                score[us] += rook_open_file_bonus;
                            else if (!(file_masks[sq] & pos.piece_bitboard[make_piece(PAWN, us)]))
                                score[us] += rook_semi_open_file_bonus;
                        }

                        Bitboard or_att_bb = attacks & king_zones[king_sq[them]].outer_ring;
                        Bitboard ir_att_bb = attacks & king_zones[king_sq[them]].inner_ring;

                        if (or_att_bb || ir_att_bb) {
                            ++n_king_attackers[us];
                            king_attack_weight[us] += inner_ring_attack[pt] * popcnt(ir_att_bb) + outer_ring_attack[pt] * popcnt(or_att_bb);
                        }
                    }
                }

                if (us == WHITE)
                {
                    us = BLACK;
                    goto repeat;
                }
            }

            if (popcnt(pos.piece_bitboard[W_BISHOP]) >= 2)
                score[WHITE] += bishop_pair_bonus;

            if (popcnt(pos.piece_bitboard[B_BISHOP]) >= 2)
                score[BLACK] += bishop_pair_bonus;

            if (n_king_attackers[BLACK] >= 2 && pos.piece_bitboard[B_QUEEN]) 
                score[WHITE] -= (king_attack_weight[BLACK] * king_attack_weight[BLACK]) / (king_safety_reduction_factor + 1);
            
            if (n_king_attackers[WHITE] >= 2 && pos.piece_bitboard[W_QUEEN])
                score[BLACK] -= (king_attack_weight[WHITE] * king_attack_weight[WHITE]) / (king_safety_reduction_factor + 1);

            return score[WHITE] - score[BLACK];
        }

        Score evaluate_pawns(const Position& pos)
        {
            Piece piece;
            Bitboard bb;
            Colour us = WHITE, them;
            Bitboard enemy_pawns;

            Score score[COLOUR_N];

        repeat:

            them = other_side(us);
            enemy_pawns = pos.piece_bitboard[make_piece(PAWN, them)];
            piece = make_piece(PAWN, us);
            bb = pos.piece_bitboard[piece];

            while (bb)
            {
                Square sq = pop_lsb(bb);

                if (doubled_pawn(pos.piece_bitboard[piece], sq))
                    score[us] -= double_pawn_penalty;

                if (isolated_pawn(pos.piece_bitboard[piece], sq))
                    score[us] -= isolated_pawn_penalty;
                
                if (passed_pawn(enemy_pawns, sq, us))
                    score[us] += *passed_table[relative_square(us, sq)];
            }

            if (us == WHITE)
            {
                us = BLACK;
                goto repeat;
            }

            return score[WHITE] - score[BLACK];
        }

        void test_eval()
        {
            //test pawn bonus eval

            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   |   |   |   |   | 8
            //  +---+---+---+---+---+---+---+---+
            //  |   |   | P |   |   |   |   |   | 7
            //  +---+---+---+---+---+---+---+---+
            //  |   |   | P | p |   |   |   |   | 6
            //  +---+---+---+---+---+---+---+---+
            //  | K | P |   |   |   |   |   | r | 5
            //  +---+---+---+---+---+---+---+---+
            //  |   | R |   |   |   | p |   | k | 4
            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   |   | P |   |   | 3
            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   | P |   | P |   | 2
            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   |   |   |   |   | 1
            //  +---+---+---+---+---+---+---+---+
            //    a   b   c   d   e   f   g   h

            // 3 white passed pawns
            // 1 white double pawns
            // 2 black isolated pawns

            cout << "Running evaluate tests..." << endl;

            Position pos("8/2P5/2Pp4/KP5r/1R3p1k/5P2/4P1P1/8 w - - ");

            assert(!doubled_pawn(pos.piece_bitboard[W_PAWN], B5));
            assert(doubled_pawn(pos.piece_bitboard[W_PAWN], C6));
            assert(doubled_pawn(pos.piece_bitboard[W_PAWN], C7));

            assert(!isolated_pawn(pos.piece_bitboard[W_PAWN], B5));
            assert(isolated_pawn(pos.piece_bitboard[B_PAWN], D6));
            assert(isolated_pawn(pos.piece_bitboard[B_PAWN], F4));
            assert(!isolated_pawn(pos.piece_bitboard[W_PAWN], F4));

            assert(passed_pawn(pos.piece_bitboard[B_PAWN], B5, WHITE));
            assert(passed_pawn(pos.piece_bitboard[B_PAWN], C6, WHITE));
            assert(passed_pawn(pos.piece_bitboard[B_PAWN], C7, WHITE));
            assert(!passed_pawn(pos.piece_bitboard[W_PAWN], D6, BLACK));

            Score expected_result;

            expected_result -= double_pawn_penalty * 2;

            expected_result -= isolated_pawn_penalty * -2;

            expected_result += *passed_table[relative_square(WHITE, B5)];
            expected_result += *passed_table[relative_square(WHITE, C6)];
            expected_result += *passed_table[relative_square(WHITE, C7)];

            assert(evaluate_pawns(pos) == expected_result);

            cout << "Evaluate tests complete!" << endl;
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
                            mask |= sq;
                    }

                    else if (rank_number != RANK_NONE)
                    {
                        if (r == rank_number)
                            mask |= sq;
                    }
                }
            }

            return mask;
        }

    } // namespace Eval

} // namespace Clovis