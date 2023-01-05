#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        Score pawn_table[] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(140,334), Score(176,312), Score(182,281), Score(203,261),
        Score(94,111), Score(112,109), Score(141,101), Score(136,97),
        Score(76,99), Score(94,93), Score(97,95), Score(112,83),
        Score(66,90), Score(74,89), Score(94,87), Score(105,83),
        Score(75,82), Score(84,83), Score(94,87), Score(96,92),
        Score(68,85), Score(92,83), Score(91,96), Score(85,97),
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[] = {
                Score(178,236), Score(247,263), Score(238,290), Score(324,276),
                Score(284,272), Score(313,291), Score(403,274), Score(337,306),
                Score(316,275), Score(382,282), Score(369,311), Score(382,307),
                Score(352,284), Score(353,305), Score(361,321), Score(366,327),
                Score(336,288), Score(357,302), Score(364,318), Score(365,321),
                Score(331,282), Score(360,293), Score(364,302), Score(369,316),
                Score(332,265), Score(333,282), Score(352,295), Score(366,296),
                Score(302,259), Score(334,255), Score(334,282), Score(342,286),
        };
        Score bishop_table[] = {
                Score(275,299), Score(290,299), Score(205,315), Score(226,316),
                Score(277,305), Score(319,305), Score(299,314), Score(298,307),
                Score(291,316), Score(322,308), Score(330,308), Score(311,309),
                Score(315,308), Score(322,308), Score(317,316), Score(336,313),
                Score(332,299), Score(325,305), Score(323,317), Score(346,311),
                Score(333,300), Score(345,305), Score(355,308), Score(339,319),
                Score(338,291), Score(366,289), Score(350,301), Score(344,304),
                Score(314,295), Score(332,305), Score(332,298), Score(335,303),
        };
        Score rook_table[] = {
                Score(445,544), Score(463,539), Score(431,548), Score(473,539),
                Score(468,542), Score(468,543), Score(507,531), Score(512,525),
                Score(456,536), Score(487,532), Score(468,535), Score(469,534),
                Score(446,540), Score(455,536), Score(479,538), Score(473,531),
                Score(443,535), Score(467,529), Score(462,533), Score(472,527),
                Score(446,524), Score(468,524), Score(474,518), Score(471,520),
                Score(442,523), Score(471,516), Score(472,519), Score(482,517),
                Score(474,509), Score(469,519), Score(490,516), Score(493,512),
        };
        Score queen_table[] = {
                Score(914,978), Score(890,1009), Score(900,1016), Score(921,996),
                Score(915,984), Score(864,1024), Score(883,1028), Score(853,1059),
                Score(933,980), Score(920,997), Score(920,1000), Score(886,1044),
                Score(911,1016), Score(897,1034), Score(898,1016), Score(892,1029),
                Score(927,981), Score(909,1025), Score(916,1009), Score(913,1020),
                Score(919,1000), Score(943,976), Score(928,1000), Score(931,988),
                Score(921,975), Score(939,956), Score(953,958), Score(945,975),
                Score(941,951), Score(931,959), Score(938,961), Score(951,950),
        };
        Score king_table[] = {
                Score(3,58), Score(162,67), Score(119,97), Score(110,87),
                Score(98,83), Score(137,112), Score(128,129), Score(174,113),
                Score(112,95), Score(204,118), Score(219,124), Score(158,116),
                Score(40,96), Score(105,126), Score(105,136), Score(85,141),
                Score(3,95), Score(63,113), Score(52,137), Score(23,151),
                Score(56,87), Score(89,106), Score(73,124), Score(61,139),
                Score(101,68), Score(110,89), Score(69,118), Score(45,130),
                Score(94,33), Score(125,60), Score(75,92), Score(89,86),
        };
        Score passed_pawn_bonus[] = {
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(17, 137),  Score(4, 127),  Score(11, 101),  Score(5, 79),
                 Score(20, 67),  Score(6, 65),  Score(12, 40),  Score(8, 37),
                 Score(18, 35),  Score(0, 35),  Score(0, 17),  Score(0, 16),
                 Score(12, 8),  Score(0, 12),  Score(0, 0),  Score(0, 0),
                 Score(4, 9),  Score(6, 6),  Score(4, 0),  Score(0, 0),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
        Score double_pawn_penalty = Score(0, 8);
        Score isolated_pawn_penalty = Score(18, 9);
        Score bishop_pair_bonus = Score(37, 47);
        Score rook_open_file_bonus = Score(40, 0);
        Score rook_semi_open_file_bonus = Score(12, 19);
        Score tempo_bonus = Score(20, 17);
        Score king_full_open_penalty = Score(51, 7);
        Score king_semi_open_penalty = Score(17, 0);
        Score king_adjacent_full_open_penalty = Score(12, 9);
        Score king_adjacent_semi_open_penalty = Score(17, 0);
        Score mobility[7] = { Score(0, 0), Score(0, 0), Score(4, 0), Score(8, 2), Score(4, 3), Score(3, 5), Score(0, 0), };
        Score inner_ring_attack[7] = { Score(0, 0), Score(2, 0), Score(4, 10), Score(7, 5), Score(7, 0), Score(4, 1), Score(0, 0), };
        Score outer_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(6, 0), Score(3, 0), Score(2, 3), Score(4, 0), Score(0, 0), };
        Score knight_outpost_bonus = Score(39, 13);
        Score bishop_outpost_bonus = Score(46, 0);

		Score* piece_table[7] = { NULL, pawn_table, knight_table, bishop_table, rook_table, queen_table, king_table };

        Score* score_table[15][SQ_N];

		Score* passed_table[SQ_N];

        Bitboard file_masks[SQ_N];
        Bitboard rank_masks[SQ_N];
        Bitboard isolated_masks[SQ_N];
        Bitboard passed_masks[COLOUR_N][SQ_N];
        Bitboard outpost_masks[COLOUR_N];
        Bitboard outpost_pawn_masks[COLOUR_N][SQ_N];

        KingZone king_zones[SQ_N];

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

                    score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f] = &piece_table[pt][sq];
                    score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                    score_table[make_piece(pt, BLACK)][(r << 3) + f] = &piece_table[pt][sq];
                    score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)] = &piece_table[pt][sq];
                }
            }
			
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
            {
                int r = sq / 4;
                int f = sq & 0x3;

                passed_table[((7 - r) << 3) + f] = &passed_pawn_bonus[sq];
                passed_table[((7 - r) << 3) + (7 - f)] = &passed_pawn_bonus[sq];
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
                        passed_masks[WHITE][sq] &= ~rank_masks[(i << 3) + f];

                    passed_masks[BLACK][sq] |= set_file_rank_mask(f - 1, RANK_NONE);
                    passed_masks[BLACK][sq] |= set_file_rank_mask(f, RANK_NONE);
                    passed_masks[BLACK][sq] |= set_file_rank_mask(f + 1, RANK_NONE);

                    for (int i = 0; i < 8 - r; ++i)
                        passed_masks[BLACK][sq] &= ~rank_masks[((7 - i) << 3) + f];
                }
            }
            
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
            {
                king_zones[sq].inner_ring = Bitboards::get_attacks<KING>(0ULL, sq);
                Bitboard bb = king_zones[sq].inner_ring;

                // this probably isnt the most efficient way of generating outer king ring
                while (bb)
                {
                    Square sq2 = pop_lsb(bb);

                    king_zones[sq].outer_ring |= Bitboards::get_attacks<KING>(0ULL, sq2);
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
