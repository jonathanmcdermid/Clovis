#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(137, 334), S(180, 309), S(182, 279), S(201, 263),
	S(94, 112), S(114, 108), S(141, 99), S(136, 97),
	S(76, 98), S(94, 93), S(97, 93), S(113, 82),
	S(66, 90), S(74, 89), S(93, 84), S(105, 81),
	S(74, 81), S(84, 83), S(92, 85), S(93, 91),
	S(68, 85), S(91, 82), S(88, 93), S(79, 96),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score knight_table[] = {
	S(173, 240), S(246, 265), S(228, 292), S(317, 277),
	S(287, 272), S(309, 293), S(399, 276), S(338, 306),
	S(320, 273), S(381, 283), S(368, 310), S(377, 307),
	S(350, 284), S(352, 305), S(361, 320), S(363, 324),
	S(340, 286), S(358, 301), S(364, 316), S(364, 321),
	S(333, 282), S(359, 291), S(363, 301), S(367, 314),
	S(334, 263), S(327, 283), S(350, 293), S(362, 295),
	S(305, 260), S(337, 252), S(323, 283), S(339, 284),
};
Score bishop_table[] = {
	S(290, 297), S(300, 300), S(211, 316), S(235, 316),
	S(277, 308), S(328, 307), S(307, 316), S(303, 309),
	S(295, 317), S(331, 307), S(339, 308), S(318, 309),
	S(320, 309), S(330, 307), S(323, 317), S(341, 316),
	S(336, 301), S(329, 306), S(329, 318), S(354, 312),
	S(336, 301), S(349, 308), S(359, 309), S(344, 321),
	S(344, 290), S(370, 289), S(355, 300), S(348, 307),
	S(319, 293), S(337, 305), S(333, 299), S(334, 304),
};
Score rook_table[] = {
	S(460, 543), S(478, 537), S(439, 549), S(490, 535),
	S(476, 541), S(473, 544), S(514, 531), S(517, 526),
	S(459, 537), S(492, 533), S(473, 536), S(476, 535),
	S(451, 540), S(461, 534), S(486, 536), S(482, 530),
	S(447, 536), S(467, 531), S(465, 534), S(477, 527),
	S(450, 524), S(471, 525), S(477, 519), S(472, 521),
	S(443, 525), S(472, 517), S(473, 520), S(484, 519),
	S(476, 510), S(470, 520), S(483, 517), S(486, 516),
};
Score queen_table[] = {
	S(921, 980), S(910, 1005), S(911, 1019), S(938, 999),
	S(926, 972), S(870, 1024), S(894, 1032), S(872, 1055),
	S(935, 978), S(922, 1002), S(923, 1007), S(893, 1049),
	S(913, 1015), S(903, 1031), S(898, 1022), S(893, 1042),
	S(932, 980), S(911, 1029), S(921, 1011), S(918, 1027),
	S(924, 998), S(947, 978), S(934, 1002), S(934, 997),
	S(923, 980), S(941, 960), S(957, 958), S(949, 974),
	S(945, 952), S(934, 959), S(937, 956), S(951, 946),
};
Score king_table[] = {
	S(103, 46), S(147, 69), S(109, 96), S(116, 93),
	S(111, 78), S(149, 93), S(117, 117), S(98, 128),
	S(91, 90), S(156, 104), S(148, 120), S(129, 133),
	S(52, 96), S(118, 118), S(113, 136), S(103, 145),
};
Score passed_pawn_bonus[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(16, 136), S(6, 127), S(8, 103), S(4, 80),
	S(18, 67), S(5, 66), S(12, 42), S(8, 36),
	S(17, 34), S(0, 35), S(0, 20), S(0, 17),
	S(11, 9), S(0, 12), S(0, 2), S(0, 0),
	S(1, 9), S(4, 8), S(5, 0), S(0, 2),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score mobility[] = {
	S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 3),
};
Score inner_ring_attack[] = {
	S(0, 0), S(2, 0), S(2, 6), S(5, 0), S(4, 0), S(3, 0),
};
Score outer_ring_attack[] = {
	S(0, 0), S(0, 0), S(3, 0), S(1, 0), S(1, 1), S(2, 0),
};
Score double_pawn_penalty = S(1, 8);
Score isolated_pawn_penalty = S(17, 8);
Score bishop_pair_bonus = S(39, 44);
Score rook_open_file_bonus = S(40, 0);
Score rook_semi_open_file_bonus = S(12, 18);
Score tempo_bonus = S(20, 17);
Score king_full_open_penalty = S(46, 8);
Score king_semi_open_penalty = S(10, 0);
Score king_adjacent_full_open_penalty = S(10, 10);
Score king_adjacent_semi_open_penalty = S(16, 0);
Score knight_outpost_bonus = S(37, 14);
Score bishop_outpost_bonus = S(47, 0);
Score virtual_king_m = S(2, 1);
Score virtual_king_b = S(3, 0);

#undef S

		const Score* piece_table[7] = { NULL, pawn_table, knight_table, bishop_table, rook_table, queen_table, king_table };
        const Score* score_table[15][SQ_N];
		const Score* passed_table[COLOUR_N][SQ_N];

		void init_eval()
        {
            init_values();
        }

        void init_values()
        {
            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                for (Square sq = SQ_ZERO; sq < 32; ++sq)
                {
                    int r = sq / 4;
                    int f = sq & 0x3;

		            if (pt != KING)
		            {
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                        score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
                        score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];
                    }

                    if (pt == KING && sq < 16) 
                    {
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                        score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
                        score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];
						
			            score_table[make_piece(pt, WHITE)][56 ^ (((7 - r) << 3) + f)]       = &piece_table[pt][sq];
                        score_table[make_piece(pt, WHITE)][56 ^ (((7 - r) << 3) + (7 - f))] = &piece_table[pt][sq];

                        score_table[make_piece(pt, BLACK)][56 ^ ((r << 3) + f)]             = &piece_table[pt][sq];
                        score_table[make_piece(pt, BLACK)][56 ^ ((r << 3) + (7 - f))]       = &piece_table[pt][sq];
					}
                }
            }

			for(Square sq = SQ_ZERO; sq < SQ_N; ++sq)
			{
				cout << *score_table[B_KING][sq] << ", ";
				if(!((sq + 1) % 8))
					cout << endl;
			}
			
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
            {
                int r = sq / 4;
                int f = sq & 0x3;

                passed_table[WHITE][((7 - r) << 3) + f]       = &passed_pawn_bonus[sq];
                passed_table[WHITE][((7 - r) << 3) + (7 - f)] = &passed_pawn_bonus[sq];

				passed_table[BLACK][(r << 3) + f]             = &passed_pawn_bonus[sq];
                passed_table[BLACK][(r << 3) + (7 - f)]       = &passed_pawn_bonus[sq];
			}
		}

    } // namespace Eval

} // namespace Clovis
