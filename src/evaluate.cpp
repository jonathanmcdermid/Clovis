#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        Score pawn_table[] = {
                Score(0,0), Score(0,0), Score(0,0), Score(0,0),
                Score(140,336), Score(190,311), Score(196,280), Score(211,262),
                Score(97,111), Score(115,110), Score(148,103), Score(139,98),
                Score(79,98), Score(100,94), Score(100,97), Score(119,87),
                Score(66,90), Score(76,90), Score(95,89), Score(111,86),
                Score(77,82), Score(84,84), Score(96,89), Score(97,95),
                Score(67,85), Score(92,84), Score(90,99), Score(84,101),
                Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[] = {
                Score(170,240), Score(244,266), Score(231,293), Score(317,279),
                Score(282,272), Score(307,294), Score(401,274), Score(339,306),
                Score(329,271), Score(395,280), Score(381,308), Score(392,305),
                Score(353,283), Score(357,306), Score(361,322), Score(362,326),
                Score(335,287), Score(358,301), Score(360,319), Score(359,325),
                Score(331,281), Score(359,295), Score(364,302), Score(371,316),
                Score(333,263), Score(328,283), Score(352,294), Score(365,296),
                Score(299,261), Score(335,254), Score(333,282), Score(337,288),
        };
        Score bishop_table[] = {
                Score(279,301), Score(293,301), Score(214,314), Score(234,316),
                Score(274,309), Score(330,303), Score(317,310), Score(298,309),
                Score(299,315), Score(336,306), Score(342,307), Score(329,305),
                Score(319,311), Score(322,311), Score(320,317), Score(339,315),
                Score(331,301), Score(328,306), Score(322,317), Score(342,314),
                Score(332,302), Score(347,307), Score(353,308), Score(333,319),
                Score(342,287), Score(365,292), Score(346,302), Score(342,306),
                Score(312,293), Score(327,306), Score(332,296), Score(334,304),
        };
        Score rook_table[] = {
                Score(449,545), Score(467,538), Score(431,549), Score(477,537),
                Score(473,539), Score(472,541), Score(516,527), Score(515,524),
                Score(455,534), Score(486,529), Score(463,533), Score(460,533),
                Score(451,535), Score(454,531), Score(470,536), Score(469,528),
                Score(446,531), Score(461,528), Score(458,531), Score(470,523),
                Score(450,520), Score(465,523), Score(470,518), Score(471,517),
                Score(443,525), Score(472,515), Score(471,519), Score(482,517),
                Score(476,510), Score(470,520), Score(490,516), Score(492,513),
        };
        Score queen_table[] = {
                Score(915,986), Score(902,1008), Score(903,1018), Score(915,1008),
                Score(913,987), Score(883,1011), Score(898,1025), Score(866,1051),
                Score(927,982), Score(919,991), Score(916,996), Score(890,1031),
                Score(904,1016), Score(896,1024), Score(892,1015), Score(889,1018),
                Score(926,982), Score(905,1019), Score(913,1001), Score(910,1009),
                Score(922,995), Score(943,966), Score(926,994), Score(926,984),
                Score(920,975), Score(940,952), Score(955,949), Score(946,968),
                Score(946,948), Score(930,953), Score(937,960), Score(953,949),
        };
        Score king_table[] = {
                Score(1,61), Score(177,64), Score(128,93), Score(98,87),
                Score(117,79), Score(130,112), Score(117,129), Score(158,115),
                Score(126,90), Score(191,120), Score(203,125), Score(146,118),
                Score(45,96), Score(102,126), Score(97,135), Score(73,142),
                Score(2,94), Score(62,113), Score(42,136), Score(5,152),
                Score(54,89), Score(92,104), Score(66,124), Score(50,139),
                Score(105,69), Score(112,89), Score(66,118), Score(43,129),
                Score(96,35), Score(126,61), Score(73,90), Score(90,83),
        };
        Score passed_pawn_bonus[] = {
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(17, 137),  Score(12, 126),  Score(13, 98),  Score(8, 79),
                 Score(18, 69),  Score(7, 65),  Score(12, 41),  Score(8, 33),
                 Score(22, 34),  Score(0, 36),  Score(0, 17),  Score(0, 14),
                 Score(15, 8),  Score(8, 12),  Score(0, 0),  Score(0, 0),
                 Score(5, 10),  Score(21, 3),  Score(12, 0),  Score(0, 0),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
        Score double_pawn_penalty = Score(0, 8);
        Score isolated_pawn_penalty = Score(18, 9);
        Score bishop_pair_bonus = Score(35, 45);
        Score rook_open_file_bonus = Score(40, 0);
        Score rook_semi_open_file_bonus = Score(12, 18);
        Score tempo_bonus = Score(20, 17);
        Score king_full_open_penalty = Score(50, 6);
        Score king_semi_open_penalty = Score(17, 0);
        Score king_adjacent_full_open_penalty = Score(13, 8);
        Score king_adjacent_semi_open_penalty = Score(17, 0);
        Score mobility[7] = { Score(0, 0), Score(0, 0), Score(3, 0), Score(6, 2), Score(3, 3), Score(2, 5), Score(0, 0), };
        Score inner_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(2, 6), Score(6, 3), Score(6, 0), Score(4, 0), Score(0, 0), };
        Score outer_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(2, 5), Score(1, 3), Score(1, 2), Score(2, 1), Score(0, 0), };
        Score outpost_bonus[2] = { Score(39, 13), Score(44, 0), };


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
