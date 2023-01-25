#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(137, 334), S(179, 309), S(183, 279), S(201, 263),
	S(93, 111), S(113, 108), S(141, 99), S(135, 96),
	S(76, 98), S(94, 93), S(97, 93), S(113, 82),
	S(66, 90), S(74, 89), S(93, 84), S(105, 81),
	S(74, 81), S(84, 83), S(92, 85), S(93, 91),
	S(68, 85), S(91, 82), S(88, 93), S(79, 96),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score knight_table[] = {
	S(172, 240), S(245, 266), S(227, 292), S(316, 278),
	S(287, 271), S(308, 293), S(399, 276), S(337, 306),
	S(320, 273), S(380, 283), S(369, 310), S(377, 307),
	S(350, 284), S(352, 305), S(361, 320), S(363, 324),
	S(339, 285), S(358, 300), S(363, 316), S(364, 321),
	S(333, 282), S(359, 291), S(363, 301), S(367, 314),
	S(334, 263), S(327, 283), S(350, 293), S(362, 295),
	S(304, 260), S(337, 252), S(322, 283), S(339, 285),
};
Score bishop_table[] = {
	S(290, 297), S(299, 300), S(209, 316), S(237, 316),
	S(278, 308), S(328, 307), S(307, 316), S(303, 309),
	S(295, 317), S(330, 307), S(339, 309), S(318, 309),
	S(320, 309), S(330, 307), S(322, 317), S(340, 316),
	S(337, 301), S(329, 306), S(329, 318), S(354, 312),
	S(336, 301), S(349, 308), S(359, 309), S(344, 321),
	S(344, 290), S(370, 289), S(355, 300), S(348, 307),
	S(319, 293), S(337, 305), S(333, 299), S(334, 304),
};
Score rook_table[] = {
	S(460, 543), S(478, 537), S(439, 549), S(490, 535),
	S(476, 541), S(473, 544), S(513, 531), S(517, 526),
	S(459, 537), S(492, 533), S(473, 537), S(476, 535),
	S(451, 540), S(461, 534), S(486, 536), S(482, 530),
	S(447, 536), S(467, 531), S(464, 534), S(476, 526),
	S(451, 524), S(471, 525), S(478, 519), S(473, 521),
	S(443, 525), S(472, 517), S(473, 520), S(484, 519),
	S(476, 510), S(470, 520), S(483, 517), S(486, 516),
};
Score queen_table[] = {
	S(920, 980), S(909, 1005), S(911, 1020), S(937, 999),
	S(926, 972), S(870, 1024), S(894, 1032), S(871, 1055),
	S(936, 978), S(923, 1003), S(924, 1007), S(893, 1050),
	S(914, 1016), S(904, 1032), S(900, 1023), S(893, 1042),
	S(931, 980), S(911, 1029), S(922, 1010), S(918, 1027),
	S(924, 998), S(947, 978), S(934, 1002), S(934, 997),
	S(923, 980), S(941, 960), S(957, 958), S(949, 974),
	S(945, 952), S(934, 959), S(937, 956), S(951, 946),
};
Score king_table[] = {
	S(8, 58), S(172, 66), S(122, 98), S(115, 87),
	S(98, 84), S(144, 110), S(139, 128), S(176, 113),
	S(119, 93), S(213, 118), S(224, 124), S(171, 116),
	S(48, 95), S(107, 126), S(108, 136), S(96, 140),
	S(6, 94), S(68, 113), S(53, 137), S(28, 151),
	S(53, 88), S(90, 106), S(78, 124), S(65, 138),
	S(84, 72), S(118, 88), S(83, 113), S(60, 127),
	S(77, 39), S(119, 63), S(79, 91), S(87, 89),
};
Score passed_pawn_bonus[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(17, 136), S(5, 127), S(8, 103), S(5, 80),
	S(19, 67), S(5, 66), S(12, 42), S(8, 36),
	S(17, 34), S(0, 35), S(0, 20), S(0, 17),
	S(11, 9), S(0, 13), S(0, 2), S(0, 0),
	S(1, 9), S(4, 8), S(5, 0), S(0, 1),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score mobility[] = {
	S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 3),
};
Score inner_ring_attack[] = {
	S(0, 0), S(1, 0), S(2, 6), S(5, 0), S(4, 0), S(3, 0),
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

                    score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
                    score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                    score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
                    score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];
                }
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
