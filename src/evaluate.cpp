#include "evaluate.h"
#include <iomanip>

namespace Clovis {

    namespace Eval {

        Score pawn_table[] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(141,264), Score(170,276), Score(144,219), Score(166,198),
        Score(97,111), Score(109,112), Score(142,107), Score(138,103),
        Score(80,99), Score(99,95), Score(100,97), Score(119,88),
        Score(66,90), Score(76,91), Score(94,90), Score(112,88),
        Score(77,82), Score(84,84), Score(96,90), Score(98,97),
        Score(67,87), Score(91,86), Score(91,101), Score(88,101),
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[] = {
                Score(170,243), Score(238,267), Score(233,294), Score(323,279),
                Score(280,274), Score(309,295), Score(399,279), Score(336,307),
                Score(329,274), Score(404,283), Score(380,312), Score(399,307),
                Score(355,286), Score(358,312), Score(369,327), Score(367,332),
                Score(336,289), Score(356,307), Score(360,325), Score(359,332),
                Score(330,285), Score(357,294), Score(363,301), Score(369,317),
                Score(328,269), Score(324,287), Score(351,294), Score(363,299),
                Score(295,264), Score(335,255), Score(328,284), Score(337,288),
        };
        Score bishop_table[] = {
                Score(278,299), Score(289,301), Score(211,314), Score(234,315),
                Score(273,309), Score(330,304), Score(312,312), Score(297,310),
                Score(299,316), Score(340,307), Score(344,309), Score(332,306),
                Score(319,309), Score(320,315), Score(320,321), Score(342,318),
                Score(328,303), Score(327,309), Score(321,319), Score(342,316),
                Score(331,301), Score(344,307), Score(351,310), Score(332,319),
                Score(335,290), Score(362,292), Score(344,302), Score(340,306),
                Score(307,297), Score(327,305), Score(332,294), Score(329,307),
        };
        Score rook_table[] = {
                Score(457,540), Score(478,534), Score(438,545), Score(489,532),
                Score(482,536), Score(472,539), Score(514,527), Score(518,521),
                Score(456,533), Score(491,526), Score(472,529), Score(462,530),
                Score(451,534), Score(459,527), Score(477,532), Score(469,526),
                Score(447,530), Score(465,524), Score(458,529), Score(470,523),
                Score(449,520), Score(465,521), Score(469,516), Score(471,517),
                Score(443,523), Score(472,512), Score(470,518), Score(481,517),
                Score(477,510), Score(470,520), Score(492,514), Score(494,511),
        };
        Score queen_table[] = {
                Score(917,982), Score(908,1002), Score(920,1008), Score(925,1000),
                Score(914,986), Score(882,1015), Score(902,1023), Score(870,1049),
                Score(930,983), Score(923,991), Score(917,998), Score(894,1031),
                Score(906,1015), Score(895,1023), Score(896,1007), Score(893,1017),
                Score(923,983), Score(906,1015), Score(912,993), Score(908,1008),
                Score(919,996), Score(943,962), Score(924,992), Score(925,977),
                Score(917,977), Score(939,948), Score(954,947), Score(944,966),
                Score(946,947), Score(930,952), Score(936,956), Score(952,947),
        };
        Score king_table[] = {
                Score(28,61), Score(218,58), Score(146,88), Score(116,77),
                Score(103,92), Score(127,111), Score(134,121), Score(156,106),
                Score(128,98), Score(202,115), Score(213,118), Score(142,110),
                Score(60,99), Score(104,124), Score(96,131), Score(74,133),
                Score(19,96), Score(58,114), Score(37,135), Score(0,146),
                Score(66,91), Score(91,106), Score(63,123), Score(40,135),
                Score(109,75), Score(114,92), Score(64,116), Score(39,124),
                Score(100,41), Score(128,64), Score(71,91), Score(85,78),
        };
        Score passed_pawn_bonus[] = {
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(1, 73),  Score(13, 37),  Score(50, 61),  Score(41, 66),
                 Score(17, 140),  Score(13, 127),  Score(17, 97),  Score(10, 76),
                 Score(17, 70),  Score(6, 68),  Score(12, 43),  Score(10, 34),
                 Score(24, 37),  Score(0, 37),  Score(0, 20),  Score(0, 14),
                 Score(17, 11),  Score(9, 14),  Score(0, 2),  Score(0, 0),
                 Score(4, 12),  Score(21, 4),  Score(19, 0),  Score(0, 0),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
        Score double_pawn_penalty = Score(1, 9);
        Score isolated_pawn_penalty = Score(19, 9);
        Score bishop_pair_bonus = Score(34, 46);
        Score rook_open_file_bonus = Score(39, 0);
        Score rook_semi_open_file_bonus = Score(11, 18);
        Score tempo_bonus = Score(20, 16);
        Score king_safety_reduction_factor = Score(6, 6);
        Score mobility[7] = { Score(0, 0), Score(0, 0), Score(3, 0), Score(6, 2), Score(3, 3), Score(2, 6), Score(0, 0), };
        Score inner_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(3, 7), Score(7, 0), Score(6, 1), Score(5, 0), Score(0, 0), };
        Score outer_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(2, 4), Score(1, 4), Score(2, 0), Score(2, 1), Score(0, 0), };

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
        Bitboard outpost_masks[COLOUR_N];

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

        // outpost stockfish description:
        // Outpost[knight/bishop] contains bonuses for each knight or bishop occupying a
        // pawn protected square on rank 4 to 6 which is also safe from a pawn attack.

        //this is an old attempt at knight outpost conditions that did not produce an improvement
        //inline bool outpost(Bitboard enemy_pawns, Bitboard friendly_pawns, Square sq, Colour side) {
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
            outpost_masks[WHITE] = rank_masks[A4] | rank_masks[A5] | rank_masks[A6];
            outpost_masks[BLACK] = rank_masks[A3] | rank_masks[A4] | rank_masks[A5];
        }

        Score evaluate_all(const Position& pos)
        {
            Score score[COLOUR_N];

            int n_king_attackers[COLOUR_N] = { 0,0 };
            Score king_attack_weight[COLOUR_N];
            Square king_sq[COLOUR_N] = { lsb(pos.piece_bitboard[W_KING]), lsb(pos.piece_bitboard[B_KING]) };
            score[pos.stm()] += tempo_bonus;

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
                        //else if (pt == KNIGHT || pt == BISHOP)
                        //{
                        //    if (outpost(pos.piece_bitboard[make_piece(PAWN, them)], pos.piece_bitboard[make_piece(PAWN, us)], sq, us))
                        //        score[us] += outpost_bonus[pt % KNIGHT];
                        //}

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