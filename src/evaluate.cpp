#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(135, 337), S(178, 310), S(188, 279), S(203, 262),
	S(94, 112), S(114, 108), S(144, 98), S(132, 99),
	S(76, 98), S(94, 93), S(97, 93), S(113, 82),
	S(66, 90), S(74, 89), S(93, 84), S(105, 81),
	S(74, 81), S(84, 83), S(92, 85), S(93, 91),
	S(68, 85), S(91, 82), S(88, 93), S(79, 96),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score knight_table[] = {
	S(176, 241), S(253, 264), S(235, 291), S(313, 280),
	S(284, 272), S(316, 290), S(401, 275), S(345, 303),
	S(325, 271), S(383, 283), S(367, 310), S(379, 307),
	S(350, 284), S(351, 305), S(359, 320), S(363, 326),
	S(340, 283), S(356, 303), S(364, 316), S(362, 322),
	S(333, 282), S(359, 291), S(363, 300), S(368, 314),
	S(335, 262), S(328, 282), S(351, 292), S(363, 296),
	S(301, 262), S(337, 253), S(323, 283), S(339, 284),
};
Score bishop_table[] = {
	S(316, 293), S(334, 301), S(331, 298), S(331, 303),
	S(316, 299), S(367, 294), S(346, 304), S(346, 305),
	S(323, 307), S(346, 306), S(357, 309), S(343, 317),
	S(325, 307), S(330, 308), S(328, 318), S(349, 315),
};
Score rook_table[] = {
	S(468, 540), S(487, 534), S(449, 545), S(495, 533),
	S(476, 541), S(471, 545), S(514, 532), S(522, 524),
	S(461, 537), S(493, 533), S(475, 535), S(476, 535),
	S(453, 539), S(465, 533), S(487, 536), S(481, 530),
	S(447, 536), S(468, 531), S(469, 533), S(477, 527),
	S(450, 524), S(471, 526), S(477, 520), S(473, 521),
	S(443, 525), S(474, 516), S(474, 520), S(483, 519),
	S(477, 511), S(470, 520), S(484, 518), S(485, 515),
};
Score queen_table[] = {
	S(925, 976), S(908, 1005), S(921, 1011), S(945, 991),
	S(928, 969), S(871, 1021), S(897, 1026), S(866, 1060),
	S(935, 979), S(925, 997), S(923, 1007), S(892, 1050),
	S(913, 1014), S(904, 1028), S(900, 1021), S(894, 1038),
	S(930, 984), S(910, 1029), S(920, 1011), S(918, 1026),
	S(924, 996), S(945, 980), S(931, 1004), S(932, 997),
	S(925, 974), S(941, 957), S(955, 960), S(949, 974),
	S(943, 953), S(935, 955), S(937, 956), S(951, 945),
};
Score king_table[] = {
	S(108, 43), S(147, 69), S(109, 96), S(116, 93),
	S(111, 78), S(149, 93), S(117, 117), S(98, 128),
	S(83, 93), S(140, 109), S(137, 123), S(125, 134),
	S(49, 97), S(118, 118), S(113, 136), S(101, 146),
};
Score passed_pawn_bonus[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(16, 136), S(1, 129), S(5, 105), S(10, 78),
	S(18, 67), S(5, 66), S(12, 42), S(8, 36),
	S(17, 34), S(0, 35), S(0, 19), S(0, 17),
	S(7, 10), S(0, 12), S(0, 2), S(0, 0),
	S(1, 9), S(2, 8), S(4, 0), S(0, 1),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score mobility[] = {
	S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 3),
};
Score inner_ring_attack[] = {
	S(0, 0), S(2, 0), S(1, 7), S(4, 2), S(4, 0), S(3, 0),
};
Score outer_ring_attack[] = {
	S(0, 0), S(0, 0), S(4, 0), S(1, 0), S(1, 1), S(2, 0),
};
Score double_pawn_penalty = S(1, 8);
Score isolated_pawn_penalty = S(17, 8);
Score bishop_pair_bonus = S(41, 43);
Score rook_open_file_bonus = S(40, 0);
Score rook_semi_open_file_bonus = S(12, 18);
Score tempo_bonus = S(20, 17);
Score king_full_open_penalty = S(46, 8);
Score king_semi_open_penalty = S(10, 0);
Score king_adjacent_full_open_penalty = S(10, 10);
Score king_adjacent_semi_open_penalty = S(16, 0);
Score knight_outpost_bonus = S(38, 14);
Score bishop_outpost_bonus = S(47, 0);
Score virtual_king_m = S(2, 1);
Score virtual_king_b = S(3, 0);
Score rook_closed_file_penalty = S(10, 0);

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

		            if (sq < 16 || (pt != KING && pt != BISHOP))
		            {
						// horizontal mirror
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                        score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
                        score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];

						if (pt == KING || pt == BISHOP) 
						{
							// vertical mirror
							score_table[make_piece(pt, WHITE)][56 ^ (((7 - r) << 3) + f)]       = &piece_table[pt][sq];
							score_table[make_piece(pt, WHITE)][56 ^ (((7 - r) << 3) + (7 - f))] = &piece_table[pt][sq];

							score_table[make_piece(pt, BLACK)][56 ^ ((r << 3) + f)]             = &piece_table[pt][sq];
							score_table[make_piece(pt, BLACK)][56 ^ ((r << 3) + (7 - f))]       = &piece_table[pt][sq];
						}
					}
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
