#include "evaluate.h"

namespace Clovis {

    namespace Eval {

    Score pawn_table[] = {
	Score(0,0), Score(0,0), Score(0,0), Score(0,0),
	Score(141,263), Score(175,274), Score(145,219), Score(166,197),
	Score(97,111), Score(115,110), Score(148,103), Score(139,98),
	Score(79,98), Score(100,94), Score(100,97), Score(119,87),
	Score(66,90), Score(76,90), Score(95,89), Score(111,86),
	Score(77,82), Score(84,84), Score(96,89), Score(97,95),
	Score(67,85), Score(92,84), Score(90,99), Score(85,100),
	Score(0,0), Score(0,0), Score(0,0), Score(0,0),
	};
Score knight_table[] = {
	Score(171,241), Score(243,265), Score(233,293), Score(316,279),
	Score(279,273), Score(307,294), Score(403,274), Score(338,306),
	Score(327,272), Score(394,280), Score(380,308), Score(393,305),
	Score(354,284), Score(357,306), Score(362,322), Score(363,327),
	Score(333,287), Score(356,302), Score(360,320), Score(358,325),
	Score(331,281), Score(359,295), Score(364,302), Score(372,316),
	Score(333,263), Score(330,284), Score(353,295), Score(365,296),
	Score(298,260), Score(335,254), Score(333,282), Score(338,287),
	};
Score bishop_table[] = {
	Score(276,301), Score(291,301), Score(213,314), Score(234,316),
	Score(273,309), Score(330,303), Score(316,310), Score(298,309),
	Score(299,315), Score(336,306), Score(342,307), Score(329,305),
	Score(318,311), Score(321,311), Score(320,317), Score(339,315),
	Score(330,301), Score(328,306), Score(322,317), Score(342,314),
	Score(332,301), Score(345,306), Score(353,308), Score(333,319),
	Score(341,287), Score(364,291), Score(346,301), Score(342,306),
	Score(311,294), Score(326,307), Score(332,295), Score(335,304),
	};
Score rook_table[] = {
	Score(448,545), Score(467,538), Score(432,549), Score(477,537),
	Score(473,539), Score(472,541), Score(517,527), Score(516,524),
	Score(456,534), Score(486,529), Score(463,533), Score(460,533),
	Score(451,535), Score(454,531), Score(470,536), Score(469,527),
	Score(446,531), Score(461,528), Score(458,531), Score(471,523),
	Score(450,521), Score(465,522), Score(470,516), Score(471,517),
	Score(443,525), Score(472,515), Score(471,519), Score(482,517),
	Score(477,511), Score(470,520), Score(490,516), Score(492,513),
	};
Score queen_table[] = {
	Score(917,988), Score(901,1010), Score(905,1020), Score(917,1010),
	Score(914,988), Score(887,1009), Score(900,1026), Score(867,1053),
	Score(928,983), Score(921,992), Score(919,998), Score(894,1035),
	Score(905,1016), Score(896,1028), Score(892,1016), Score(893,1020),
	Score(923,985), Score(906,1019), Score(912,997), Score(910,1011),
	Score(921,994), Score(943,964), Score(925,992), Score(925,983),
	Score(920,975), Score(939,952), Score(954,948), Score(946,967),
	Score(945,948), Score(932,952), Score(936,959), Score(952,947),
	};
Score king_table[] = {
	Score(1,62), Score(178,65), Score(130,95), Score(97,87),
	Score(120,80), Score(129,113), Score(116,128), Score(157,114),
	Score(127,92), Score(190,121), Score(201,125), Score(145,116),
	Score(45,96), Score(101,125), Score(97,135), Score(73,141),
	Score(3,95), Score(61,113), Score(42,136), Score(4,151),
	Score(54,89), Score(92,104), Score(66,124), Score(50,139),
	Score(105,69), Score(112,89), Score(65,117), Score(42,128),
	Score(97,37), Score(126,62), Score(73,90), Score(89,82),
	};
Score passed_pawn_bonus[] = {
	 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
	 Score(0, 73),  Score(14, 37),  Score(50, 61),  Score(44, 65),
	 Score(17, 137),  Score(12, 126),  Score(13, 98),  Score(8, 80),
	 Score(18, 69),  Score(7, 65),  Score(12, 41),  Score(8, 33),
	 Score(22, 34),  Score(0, 36),  Score(0, 17),  Score(0, 14),
	 Score(15, 8),  Score(8, 12),  Score(0, 0),  Score(0, 0),
	 Score(4, 11),  Score(21, 4),  Score(19, 0),  Score(0, 0),
	 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
	};
Score double_pawn_penalty = Score(0, 8);
Score isolated_pawn_penalty = Score(18, 9);
Score bishop_pair_bonus = Score(35, 45);
Score rook_open_file_bonus = Score(40, 0);
Score rook_semi_open_file_bonus = Score(12, 18);
Score tempo_bonus = Score(20, 17);
Score king_safety_reduction_factor = Score(6, 8);
Score king_full_open_penalty = Score(50, 6);
Score king_semi_open_penalty = Score(17, 0);
Score king_adjacent_full_open_penalty = Score(13, 7);
Score king_adjacent_semi_open_penalty = Score(17, 0);
Score mobility[7] = { Score(0, 0), Score(0, 0), Score(3, 0), Score(6, 2), Score(3, 3), Score(2, 6), Score(0, 0),};
Score inner_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(3, 8), Score(7, 1), Score(6, 0), Score(5, 0), Score(0, 0),};
Score outer_ring_attack[7] = { Score(0, 0), Score(0, 0), Score(2, 4), Score(1, 3), Score(1, 3), Score(2, 1), Score(0, 0),};
Score outpost_bonus[2] = { Score(39, 13), Score(43, 1),};


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
