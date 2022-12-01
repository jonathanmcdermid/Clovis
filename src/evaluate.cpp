#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        Score pawn_table[] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(140,264), Score(175,273), Score(144,219), Score(165,197),
        Score(96,111), Score(114,111), Score(145,105), Score(140,98),
        Score(80,99), Score(99,95), Score(100,97), Score(119,88),
        Score(66,90), Score(76,91), Score(94,90), Score(112,88),
        Score(77,82), Score(84,84), Score(96,90), Score(98,97),
        Score(67,87), Score(91,86), Score(90,100), Score(88,101),
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        };
Score knight_table[] = {
        Score(170,243), Score(239,268), Score(228,295), Score(316,280),
        Score(281,274), Score(310,293), Score(401,276), Score(338,306),
        Score(328,273), Score(395,281), Score(376,310), Score(393,305),
        Score(356,284), Score(356,306), Score(362,322), Score(363,328),
        Score(333,288), Score(355,303), Score(358,321), Score(358,326),
        Score(330,283), Score(357,294), Score(363,301), Score(371,316),
        Score(330,265), Score(323,286), Score(351,294), Score(364,297),
        Score(295,264), Score(335,255), Score(331,284), Score(339,286),
        };
Score bishop_table[] = {
        Score(277,300), Score(294,299), Score(212,314), Score(231,317),
        Score(277,307), Score(330,302), Score(316,312), Score(297,310),
        Score(298,317), Score(338,307), Score(341,307), Score(329,306),
        Score(319,311), Score(319,312), Score(321,316), Score(340,316),
        Score(330,301), Score(327,307), Score(321,319), Score(342,315),
        Score(331,301), Score(344,307), Score(352,308), Score(332,319),
        Score(337,289), Score(362,292), Score(344,302), Score(341,305),
        Score(309,295), Score(326,307), Score(332,294), Score(333,303),
        };
Score rook_table[] = {
        Score(450,543), Score(465,537), Score(432,548), Score(479,536),
        Score(474,540), Score(472,540), Score(515,527), Score(513,523),
        Score(455,534), Score(487,528), Score(463,532), Score(459,532),
        Score(451,534), Score(455,529), Score(469,535), Score(468,527),
        Score(447,529), Score(463,525), Score(457,530), Score(470,523),
        Score(448,520), Score(465,521), Score(469,516), Score(471,517),
        Score(443,523), Score(472,512), Score(470,518), Score(481,517),
        Score(477,510), Score(470,520), Score(492,514), Score(494,511),
        };
Score queen_table[] = {
        Score(913,989), Score(904,1007), Score(905,1021), Score(916,1010),
        Score(912,991), Score(884,1010), Score(900,1024), Score(869,1049),
        Score(929,987), Score(919,996), Score(916,998), Score(893,1034),
        Score(906,1014), Score(893,1029), Score(896,1007), Score(894,1016),
        Score(924,981), Score(905,1019), Score(911,997), Score(907,1008),
        Score(920,994), Score(943,961), Score(925,992), Score(924,982),
        Score(919,975), Score(939,949), Score(954,947), Score(944,966),
        Score(945,947), Score(930,953), Score(936,956), Score(952,947),
        };
Score king_table[] = {
        Score(3,66), Score(181,63), Score(123,93), Score(92,82),
        Score(121,85), Score(125,110), Score(117,125), Score(147,109),
        Score(132,97), Score(191,118), Score(199,121), Score(139,111),
        Score(50,101), Score(100,125), Score(94,132), Score(66,135),
        Score(6,99), Score(58,114), Score(42,134), Score(1,146),
        Score(58,95), Score(92,106), Score(63,123), Score(45,133),
        Score(109,75), Score(114,92), Score(64,116), Score(39,124),
        Score(100,41), Score(128,64), Score(73,89), Score(85,78),
        };
Score passed_pawn_bonus[] = {
         Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
         Score(0, 73),  Score(14, 37),  Score(50, 61),  Score(42, 65),
         Score(18, 139),  Score(11, 126),  Score(14, 98),  Score(9, 80),
         Score(18, 69),  Score(9, 65),  Score(11, 42),  Score(9, 33),
         Score(23, 35),  Score(0, 37),  Score(0, 19),  Score(0, 14),
         Score(17, 11),  Score(9, 14),  Score(0, 2),  Score(0, 0),
         Score(4, 11),  Score(21, 4),  Score(19, 0),  Score(0, 0),
         Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
	Score double_pawn_penalty = Score(1, 9);
	Score isolated_pawn_penalty = Score(19, 9);
	Score bishop_pair_bonus = Score(34, 46);
	Score rook_open_file_bonus = Score(39, 0);
	Score rook_semi_open_file_bonus = Score(11, 18);
	Score tempo_bonus = Score(20, 16);
	Score king_safety_reduction_factor = Score(6, 6);
	Score mobility[7] = { Score(0, 0), Score(0, 0), Score(3, 0), Score(6, 2), Score(3, 3), Score(2, 6), Score(0, 0),};
	Score inner_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(3, 7), Score(7, 0), Score(6, 1), Score(5, 0), Score(0, 0),};
	Score outer_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(2, 4), Score(1, 4), Score(2, 0), Score(2, 1), Score(0, 0),};
	Score outpost_bonus[2] = { Score(39, 12), Score(40, 0),};

        Score* piece_table[7] = { NULL, pawn_table, knight_table,bishop_table,rook_table,queen_table,king_table };

        Score* score_table[15][SQ_N];

	Score* passed_table[SQ_N];

        Bitboard file_masks[SQ_N];
        Bitboard rank_masks[SQ_N];
        Bitboard isolated_masks[SQ_N];
        Bitboard passed_masks[COLOUR_N][SQ_N];
        Bitboard outpost_masks[COLOUR_N];
        Bitboard outpost_pawn_masks[COLOUR_N][SQ_N];

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

        inline bool outpost(Bitboard enemy_pawns, Bitboard friendly_pawns, Square sq, Colour side) {
            return (Bitboards::pawn_attacks[other_side(side)][sq] & friendly_pawns) && (outpost_masks[side] & sq) && !(enemy_pawns & outpost_pawn_masks[side][sq]);
        }

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
            for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
            {
                outpost_pawn_masks[WHITE][sq] = passed_masks[WHITE][sq] & ~file_masks[sq];
                outpost_pawn_masks[BLACK][sq] = passed_masks[BLACK][sq] & ~file_masks[sq];
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
                        else if (pt == KNIGHT || pt == BISHOP)
                        {
                            if (outpost(pos.piece_bitboard[make_piece(PAWN, them)], pos.piece_bitboard[make_piece(PAWN, us)], sq, us))
                            {
                                score[us] += outpost_bonus[pt % KNIGHT];
                            }
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
