#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0,0), S(0,0), S(0,0), S(0,0),
	S(138,334), S(178,309), S(184,279), S(200,263),
	S(93,111), S(113,108), S(141,99), S(135,96),
	S(76,98), S(94,93), S(97,93), S(113,82),
	S(66,90), S(74,89), S(93,84), S(105,81),
	S(74,81), S(84,83), S(92,85), S(93,91),
	S(68,85), S(91,82), S(88,93), S(79,96),
	S(0,0), S(0,0), S(0,0), S(0,0),
	};
Score knight_table[] = {
	S(172,240), S(245,266), S(228,292), S(316,278),
	S(288,271), S(309,293), S(399,276), S(337,306),
	S(318,273), S(380,284), S(368,310), S(378,307),
	S(350,284), S(353,304), S(361,320), S(364,325),
	S(339,285), S(359,300), S(363,316), S(362,320),
	S(332,281), S(359,292), S(362,300), S(367,314),
	S(335,261), S(328,283), S(349,292), S(362,295),
	S(304,261), S(337,253), S(321,283), S(339,285),
	};
Score bishop_table[] = {
	S(289,297), S(298,300), S(209,316), S(237,316),
	S(278,308), S(327,306), S(307,316), S(301,310),
	S(295,318), S(330,307), S(338,309), S(318,309),
	S(320,309), S(330,307), S(323,317), S(340,316),
	S(337,302), S(329,306), S(329,318), S(354,312),
	S(336,301), S(348,308), S(359,309), S(344,321),
	S(344,289), S(370,289), S(355,300), S(347,306),
	S(319,293), S(336,304), S(333,299), S(334,304),
	};
Score rook_table[] = {
	S(460,543), S(478,537), S(438,549), S(489,536),
	S(476,541), S(473,543), S(513,531), S(517,527),
	S(459,537), S(492,533), S(473,537), S(475,535),
	S(450,540), S(461,534), S(486,536), S(482,530),
	S(446,536), S(467,531), S(464,534), S(476,526),
	S(451,524), S(472,524), S(478,519), S(473,521),
	S(443,525), S(472,517), S(473,520), S(484,519),
	S(476,510), S(470,520), S(483,517), S(486,516),
	};
Score queen_table[] = {
	S(919,979), S(909,1005), S(910,1021), S(937,1001),
	S(926,972), S(871,1025), S(896,1031), S(872,1056),
	S(935,979), S(924,1003), S(924,1007), S(893,1050),
	S(914,1016), S(905,1033), S(900,1023), S(894,1041),
	S(932,981), S(911,1029), S(922,1010), S(918,1027),
	S(924,997), S(947,977), S(933,1002), S(934,997),
	S(923,980), S(940,960), S(957,958), S(949,974),
	S(945,952), S(934,959), S(937,956), S(951,946),
	};
Score king_table[] = {
	S(10,58), S(172,67), S(122,98), S(115,87),
	S(98,84), S(142,111), S(138,127), S(176,113),
	S(119,93), S(214,118), S(224,124), S(170,116),
	S(47,95), S(107,127), S(108,137), S(96,140),
	S(5,94), S(68,113), S(53,137), S(28,151),
	S(53,88), S(90,106), S(78,124), S(65,138),
	S(84,72), S(118,88), S(83,113), S(60,127),
	S(77,39), S(119,63), S(79,91), S(88,90),
	};
Score passed_pawn_bonus[] = {
	 S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
	 S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
	 S(16, 136),  S(5, 127),  S(8, 103),  S(5, 80),
	 S(19, 67),  S(6, 66),  S(11, 42),  S(8, 36),
	 S(17, 34),  S(0, 35),  S(0, 20),  S(0, 17),
	 S(11, 9),  S(0, 12),  S(0, 2),  S(0, 0),
	 S(1, 9),  S(4, 8),  S(5, 0),  S(0, 2),
	 S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
	};
Score double_pawn_penalty = S(1, 8);
Score isolated_pawn_penalty = S(17, 8);
Score bishop_pair_bonus = S(39, 44);
Score rook_open_file_bonus = S(40, 0);
Score rook_semi_open_file_bonus = S(12, 18);
Score tempo_bonus = S(20, 17);
Score king_full_open_penalty = S(46, 8);
Score king_semi_open_penalty = S(9, 0);
Score king_adjacent_full_open_penalty = S(10, 10);
Score king_adjacent_semi_open_penalty = S(16, 0);
Score knight_outpost_bonus = S(37, 14);
Score bishop_outpost_bonus = S(47, 0);
Score mobility[] = { S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 3), S(0, 0),};
Score inner_ring_attack[] = { S(0, 0), S(1, 0), S(2, 6), S(5, 1), S(4, 0), S(3, 0), S(0, 0),};
Score outer_ring_attack[] = { S(0, 0), S(0, 0), S(3, 0), S(1, 0), S(1, 1), S(2, 0), S(0, 0),};
Score virtual_king_m = S(2, 1);
Score virtual_king_b = S(4, 0);

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

                passed_table[WHITE][((7 - r) << 3) + f] = &passed_pawn_bonus[sq];
                passed_table[WHITE][((7 - r) << 3) + (7 - f)] = &passed_pawn_bonus[sq];

				passed_table[BLACK][(r << 3) + f] = &passed_pawn_bonus[sq];
                passed_table[BLACK][(r << 3) + (7 - f)] = &passed_pawn_bonus[sq];
			}
		}

    } // namespace Eval

} // namespace Clovis
