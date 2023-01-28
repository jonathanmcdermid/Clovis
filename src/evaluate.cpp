#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(135, 337), S(179, 310), S(188, 279), S(202, 262),
	S(94, 112), S(115, 108), S(144, 98), S(132, 99),
	S(76, 98), S(95, 93), S(97, 93), S(113, 82),
	S(66, 90), S(74, 89), S(93, 84), S(105, 81),
	S(74, 81), S(84, 83), S(92, 85), S(93, 91),
	S(68, 85), S(91, 82), S(88, 93), S(79, 96),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score knight_table[] = {
	S(178, 241), S(255, 263), S(234, 292), S(312, 279),
	S(281, 272), S(315, 290), S(401, 274), S(346, 303),
	S(322, 272), S(383, 283), S(368, 309), S(379, 306),
	S(350, 284), S(352, 305), S(359, 320), S(363, 325),
	S(340, 283), S(356, 303), S(363, 316), S(362, 321),
	S(333, 281), S(359, 291), S(363, 299), S(368, 314),
	S(336, 261), S(328, 283), S(351, 291), S(363, 296),
	S(301, 261), S(337, 254), S(322, 283), S(339, 284),
};
Score bishop_table[] = {
	S(316, 294), S(0, 0), S(0, 0), S(0, 0),
	S(318, 301), S(368, 292), S(0, 0), S(0, 0),
	S(331, 302), S(345, 306), S(358, 309), S(0, 0),
	S(325, 307), S(344, 306), S(337, 318), S(348, 316),
};
Score rook_table[] = {
	S(472, 524), S(464, 532), S(478, 529), S(477, 528),
	S(443, 544), S(465, 539), S(477, 533), S(489, 526),
	S(447, 538), S(471, 535), S(476, 529), S(470, 531),
	S(445, 541), S(465, 535), S(477, 536), S(477, 531),
};
Score queen_table[] = {
	S(925, 976), S(909, 1004), S(920, 1011), S(945, 991),
	S(926, 968), S(871, 1021), S(896, 1026), S(866, 1058),
	S(934, 978), S(925, 997), S(924, 1007), S(891, 1049),
	S(913, 1014), S(904, 1028), S(900, 1021), S(893, 1037),
	S(930, 984), S(910, 1029), S(920, 1011), S(918, 1025),
	S(923, 996), S(945, 980), S(932, 1004), S(933, 996),
	S(926, 975), S(942, 957), S(955, 960), S(949, 975),
	S(944, 952), S(934, 955), S(937, 955), S(951, 945),
};
Score king_table[] = {
	S(108, 43), S(147, 69), S(108, 96), S(115, 92),
	S(111, 78), S(149, 93), S(117, 117), S(98, 128),
	S(83, 93), S(140, 109), S(137, 123), S(126, 134),
	S(49, 97), S(118, 118), S(112, 136), S(100, 145),
};
Score passed_pawn_bonus[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(16, 136), S(1, 129), S(5, 105), S(10, 78),
	S(18, 68), S(4, 66), S(12, 42), S(8, 36),
	S(17, 34), S(0, 35), S(0, 20), S(0, 17),
	S(7, 10), S(0, 12), S(0, 3), S(0, 0),
	S(1, 9), S(3, 9), S(4, 0), S(0, 2),
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
Score rook_semi_open_file_bonus = S(10, 16);
Score tempo_bonus = S(20, 17);
Score king_full_open_penalty = S(46, 8);
Score king_semi_open_penalty = S(10, 0);
Score king_adjacent_full_open_penalty = S(10, 10);
Score king_adjacent_semi_open_penalty = S(16, 0);
Score knight_outpost_bonus = S(38, 15);
Score bishop_outpost_bonus = S(46, 0);
Score virtual_king_m = S(2, 1);
Score virtual_king_b = S(3, 0);
Score rook_closed_file_penalty = S(11, 2);

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
				if(pt == BISHOP)
					continue;
                for (Square sq = SQ_ZERO; sq < 32; ++sq)
                {
                    int r = sq / 4;
                    int f = sq & 0x3;

		            if ((sq < 16) || (pt != KING && pt != BISHOP && pt != ROOK))
		            {
						// horizontal mirror
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                        score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
                        score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];

						if (pt == KING || pt == ROOK) 
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
            
            for (PieceType pt = BISHOP; pt <= BISHOP; ++pt)
            {
                for (Square sq = SQ_ZERO; sq < 16; ++sq)
                {
                    int r = sq / 4;
                    int f = sq & 0x3;

						if (r >= f) 
						{
						// horizontal mirror
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                        score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
                        score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];
							// vertical mirror
							score_table[make_piece(pt, WHITE)][56 ^ (((7 - r) << 3) + f)]       = &piece_table[pt][sq];
							score_table[make_piece(pt, WHITE)][56 ^ (((7 - r) << 3) + (7 - f))] = &piece_table[pt][sq];

							score_table[make_piece(pt, BLACK)][56 ^ ((r << 3) + f)]             = &piece_table[pt][sq];
							score_table[make_piece(pt, BLACK)][56 ^ ((r << 3) + (7 - f))]       = &piece_table[pt][sq];
							// diagonal mirror
								cout << piece_table[pt][sq] << endl;
								score_table[make_piece(pt, WHITE)][f*8 + r] = &piece_table[pt][sq];
								score_table[make_piece(pt, WHITE)][f*8 + 7 - r] = &piece_table[pt][sq];
								score_table[make_piece(pt, WHITE)][(7-f)*8 + r] = &piece_table[pt][sq];
								score_table[make_piece(pt, WHITE)][(7-f)*8 + 7 - r] = &piece_table[pt][sq];
								
								score_table[make_piece(pt, BLACK)][f*8 + r] = &piece_table[pt][sq];
								score_table[make_piece(pt, BLACK)][f*8 + 7 - r] = &piece_table[pt][sq];
								score_table[make_piece(pt, BLACK)][(7-f)*8 + r] = &piece_table[pt][sq];
								score_table[make_piece(pt, BLACK)][(7-f)*8 + 7 - r] = &piece_table[pt][sq];
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
