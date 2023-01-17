#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

        Score pawn_table[] = {
			S(0,0), S(0,0), S(0,0), S(0,0),
			S(140,334), S(176,312), S(182,281), S(203,261),
			S(94,111), S(112,109), S(141,101), S(136,97),
			S(76,99), S(94,93), S(97,95), S(112,83),
			S(66,90), S(74,89), S(94,87), S(105,83),
			S(75,82), S(84,83), S(94,87), S(96,92),
			S(68,85), S(92,83), S(91,96), S(85,97),
			S(0,0), S(0,0), S(0,0), S(0,0),
        };

        Score knight_table[] = {
            S(178,236), S(247,263), S(238,290), S(324,276),
            S(284,272), S(313,291), S(403,274), S(337,306),
            S(316,275), S(382,282), S(369,311), S(382,307),
            S(352,284), S(353,305), S(361,321), S(366,327),
            S(336,288), S(357,302), S(364,318), S(365,321),
            S(331,282), S(360,293), S(364,302), S(369,316),
            S(332,265), S(333,282), S(352,295), S(366,296),
            S(302,259), S(334,255), S(334,282), S(342,286),
        };

        Score bishop_table[] = {
            S(275,299), S(290,299), S(205,315), S(226,316),
            S(277,305), S(319,305), S(299,314), S(298,307),
            S(291,316), S(322,308), S(330,308), S(311,309),
            S(315,308), S(322,308), S(317,316), S(336,313),
            S(332,299), S(325,305), S(323,317), S(346,311),
            S(333,300), S(345,305), S(355,308), S(339,319),
            S(338,291), S(366,289), S(350,301), S(344,304),
            S(314,295), S(332,305), S(332,298), S(335,303),
        };

        Score rook_table[] = {
            S(445,544), S(463,539), S(431,548), S(473,539),
            S(468,542), S(468,543), S(507,531), S(512,525),
            S(456,536), S(487,532), S(468,535), S(469,534),
            S(446,540), S(455,536), S(479,538), S(473,531),
            S(443,535), S(467,529), S(462,533), S(472,527),
            S(446,524), S(468,524), S(474,518), S(471,520),
            S(442,523), S(471,516), S(472,519), S(482,517),
            S(474,509), S(469,519), S(490,516), S(493,512),
        };

        Score queen_table[] = {
            S(914,978), S(890,1009), S(900,1016), S(921,996),
            S(915,984), S(864,1024), S(883,1028), S(853,1059),
            S(933,980), S(920,997), S(920,1000), S(886,1044),
            S(911,1016), S(897,1034), S(898,1016), S(892,1029),
            S(927,981), S(909,1025), S(916,1009), S(913,1020),
            S(919,1000), S(943,976), S(928,1000), S(931,988),
            S(921,975), S(939,956), S(953,958), S(945,975),
            S(941,951), S(931,959), S(938,961), S(951,950),
        };

        Score king_table[] = {
            S(3,58), S(162,67), S(119,97), S(110,87),
            S(98,83), S(137,112), S(128,129), S(174,113),
            S(112,95), S(204,118), S(219,124), S(158,116),
            S(40,96), S(105,126), S(105,136), S(85,141),
            S(3,95), S(63,113), S(52,137), S(23,151),
            S(56,87), S(89,106), S(73,124), S(61,139),
            S(101,68), S(110,89), S(69,118), S(45,130),
            S(94,33), S(125,60), S(75,92), S(89,86),
        };

        Score passed_pawn_bonus[] = {
            S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
            S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
            S(17, 137),  S(4, 127),  S(11, 101),  S(5, 79),
            S(20, 67),  S(6, 65),  S(12, 40),  S(8, 37),
            S(18, 35),  S(0, 35),  S(0, 17),  S(0, 16),
            S(12, 8),  S(0, 12),  S(0, 0),  S(0, 0),
            S(4, 9),  S(6, 6),  S(4, 0),  S(0, 0),
            S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
        };

        Score double_pawn_penalty = S(0, 8);
        Score isolated_pawn_penalty = S(18, 9);
        Score bishop_pair_bonus = S(37, 47);
        Score rook_open_file_bonus = S(40, 0);
        Score rook_semi_open_file_bonus = S(12, 19);
        Score tempo_bonus = S(20, 17);
        Score king_full_open_penalty = S(51, 7);
        Score king_semi_open_penalty = S(17, 0);
        Score king_adjacent_full_open_penalty = S(12, 9);
        Score king_adjacent_semi_open_penalty = S(17, 0);
        Score mobility[7] = { S(0, 0), S(0, 0), S(4, 0), S(8, 2), S(4, 3), S(3, 5), S(0, 0), };
        Score inner_ring_attack[7] = { S(0, 0), S(2, 0), S(4, 10), S(7, 5), S(7, 0), S(4, 1), S(0, 0), };
        Score outer_ring_attack[7] = { S(0, 0), S(0, 0), S(6, 0), S(3, 0), S(2, 3), S(4, 0), S(0, 0), };
        Score knight_outpost_bonus = S(39, 13);
        Score bishop_outpost_bonus = S(46, 0);

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
