#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0,0), S(0,0), S(0,0), S(0,0),
	S(138,333), S(176,310), S(186,279), S(205,261),
	S(93,111), S(113,108), S(141,99), S(136,96),
	S(76,98), S(94,93), S(97,93), S(112,83),
	S(66,90), S(74,89), S(93,84), S(105,81),
	S(74,81), S(84,83), S(92,85), S(93,91),
	S(68,85), S(91,82), S(88,93), S(79,96),
	S(0,0), S(0,0), S(0,0), S(0,0),
	};
Score knight_table[] = {
	S(172,239), S(246,264), S(240,288), S(321,275),
	S(284,273), S(316,291), S(407,272), S(338,305),
	S(318,275), S(383,282), S(367,310), S(379,307),
	S(351,284), S(353,304), S(361,320), S(364,326),
	S(339,284), S(358,301), S(363,316), S(362,320),
	S(332,282), S(358,292), S(362,300), S(367,314),
	S(335,262), S(328,283), S(349,292), S(362,294),
	S(307,258), S(336,257), S(325,282), S(337,286),
	};
Score bishop_table[] = {
	S(284,299), S(295,301), S(206,316), S(232,317),
	S(280,307), S(327,306), S(306,316), S(305,308),
	S(295,318), S(329,308), S(335,310), S(317,310),
	S(318,309), S(331,308), S(322,317), S(340,316),
	S(335,301), S(329,307), S(329,318), S(350,313),
	S(336,301), S(348,308), S(359,309), S(343,320),
	S(344,290), S(370,289), S(353,301), S(347,306),
	S(320,293), S(335,306), S(333,300), S(335,304),
	};
Score rook_table[] = {
	S(453,545), S(472,539), S(434,549), S(481,538),
	S(473,542), S(470,544), S(512,531), S(514,526),
	S(458,538), S(490,533), S(474,535), S(475,536),
	S(450,540), S(461,534), S(483,537), S(476,532),
	S(448,534), S(471,528), S(464,534), S(476,526),
	S(450,524), S(471,524), S(478,517), S(472,521),
	S(443,525), S(471,517), S(474,520), S(483,518),
	S(476,510), S(470,520), S(485,517), S(489,514),
	};
Score queen_table[] = {
	S(917,980), S(897,1010), S(908,1018), S(932,1001),
	S(921,977), S(865,1030), S(890,1033), S(861,1059),
	S(933,980), S(922,1002), S(921,1007), S(891,1050),
	S(912,1017), S(902,1033), S(899,1019), S(893,1040),
	S(931,982), S(909,1029), S(920,1013), S(917,1026),
	S(925,997), S(947,976), S(933,1001), S(934,997),
	S(923,979), S(940,960), S(957,959), S(949,974),
	S(945,951), S(935,957), S(938,957), S(952,947),
	};
Score king_table[] = {
	S(12,59), S(171,66), S(119,99), S(111,88),
	S(100,83), S(143,111), S(132,129), S(177,114),
	S(115,94), S(209,119), S(221,125), S(164,117),
	S(42,97), S(106,127), S(108,137), S(92,142),
	S(5,94), S(67,113), S(53,137), S(28,151),
	S(53,88), S(90,106), S(75,125), S(62,139),
	S(90,70), S(113,91), S(77,117), S(55,129),
	S(83,35), S(120,61), S(79,91), S(88,89),
	};
Score passed_pawn_bonus[] = {
	 S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
	 S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
	 S(16, 136),  S(5, 127),  S(11, 101),  S(5, 80),
	 S(19, 67),  S(6, 66),  S(11, 42),  S(8, 36),
	 S(18, 34),  S(0, 35),  S(0, 20),  S(0, 17),
	 S(11, 9),  S(1, 12),  S(0, 2),  S(0, 0),
	 S(2, 9),  S(5, 7),  S(5, 0),  S(0, 1),
	 S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
	};
Score double_pawn_penalty = S(1, 8);
Score isolated_pawn_penalty = S(17, 8);
Score bishop_pair_bonus = S(40, 45);
Score rook_open_file_bonus = S(40, 0);
Score rook_semi_open_file_bonus = S(12, 18);
Score tempo_bonus = S(20, 17);
Score king_full_open_penalty = S(49, 7);
Score king_semi_open_penalty = S(8, 0);
Score king_adjacent_full_open_penalty = S(10, 10);
Score king_adjacent_semi_open_penalty = S(16, 0);
Score knight_outpost_bonus = S(37, 14);
Score bishop_outpost_bonus = S(48, 0);
Score mobility[] = { S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 3), S(0, 0),};
Score inner_ring_attack[] = { S(0, 0), S(1, 0), S(1, 8), S(4, 4), S(4, 0), S(3, 0), S(0, 0),};
Score outer_ring_attack[] = { S(0, 0), S(0, 0), S(3, 0), S(1, 0), S(1, 1), S(2, 0), S(0, 0),};
Score virtual_king_m = S(2, 1);
Score virtual_king_b = S(3, 2);

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
