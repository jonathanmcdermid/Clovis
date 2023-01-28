#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(136, 337), S(178, 310), S(187, 279), S(201, 262),
	S(91, 112), S(114, 107), S(144, 98), S(132, 99),
	S(76, 98), S(95, 93), S(97, 93), S(113, 82),
	S(65, 89), S(74, 89), S(93, 84), S(105, 81),
	S(74, 81), S(84, 83), S(91, 85), S(93, 91),
	S(69, 84), S(91, 82), S(87, 94), S(79, 97),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score knight_table[] = {
	S(182, 240), S(258, 261), S(234, 292), S(313, 279),
	S(280, 272), S(315, 291), S(403, 274), S(346, 302),
	S(324, 271), S(382, 282), S(366, 310), S(378, 307),
	S(350, 284), S(352, 305), S(358, 321), S(363, 325),
	S(340, 283), S(357, 302), S(363, 317), S(362, 321),
	S(333, 281), S(359, 291), S(363, 299), S(368, 314),
	S(337, 260), S(328, 283), S(351, 291), S(363, 295),
	S(303, 261), S(336, 252), S(323, 284), S(339, 283),
};
Score bishop_table[] = {
	S(316, 294), S(0, 0), S(0, 0), S(0, 0),
	S(320, 302), S(368, 293), S(0, 0), S(0, 0),
	S(331, 302), S(346, 307), S(358, 309), S(0, 0),
	S(326, 307), S(344, 306), S(338, 318), S(349, 315),
};
Score rook_table[] = {
	S(462, 530), S(0, 0), S(0, 0), S(0, 0),
	S(448, 542), S(459, 541), S(0, 0), S(0, 0),
	S(464, 533), S(461, 540), S(465, 534), S(0, 0),
	S(460, 536), S(469, 535), S(459, 539), S(464, 536),
};
Score queen_table[] = {
	S(926, 977), S(911, 1003), S(921, 1011), S(948, 991),
	S(925, 967), S(872, 1021), S(897, 1026), S(865, 1058),
	S(934, 977), S(925, 997), S(923, 1006), S(893, 1048),
	S(914, 1013), S(905, 1029), S(901, 1020), S(894, 1038),
	S(931, 982), S(910, 1027), S(920, 1012), S(918, 1025),
	S(924, 997), S(947, 978), S(932, 1004), S(933, 995),
	S(924, 976), S(944, 957), S(956, 960), S(948, 976),
	S(943, 949), S(934, 955), S(936, 955), S(951, 946),
};
Score king_table[] = {
	S(108, 44), S(0, 0), S(0, 0), S(0, 0),
	S(144, 71), S(149, 96), S(0, 0), S(0, 0),
	S(102, 96), S(122, 117), S(132, 125), S(0, 0),
	S(109, 91), S(93, 129), S(114, 137), S(95, 147),
};
Score passed_pawn_bonus[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(17, 137), S(1, 129), S(5, 105), S(9, 78),
	S(17, 68), S(4, 66), S(13, 42), S(8, 37),
	S(17, 35), S(0, 36), S(0, 20), S(0, 17),
	S(7, 10), S(0, 13), S(0, 3), S(0, 0),
	S(1, 9), S(3, 9), S(4, 0), S(0, 2),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score mobility[] = {
	S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 3),
};
Score inner_ring_attack[] = {
	S(0, 0), S(2, 0), S(1, 6), S(3, 4), S(4, 0), S(3, 0),
};
Score outer_ring_attack[] = {
	S(0, 0), S(0, 0), S(4, 0), S(1, 0), S(1, 1), S(2, 0),
};
Score double_pawn_penalty = S(0, 8);
Score isolated_pawn_penalty = S(17, 8);
Score bishop_pair_bonus = S(40, 42);
Score rook_open_file_bonus = S(40, 0);
Score rook_semi_open_file_bonus = S(9, 15);
Score tempo_bonus = S(20, 17);
Score king_full_open_penalty = S(45, 7);
Score king_semi_open_penalty = S(10, 0);
Score king_adjacent_full_open_penalty = S(8, 8);
Score king_adjacent_semi_open_penalty = S(16, 0);
Score knight_outpost_bonus = S(37, 15);
Score bishop_outpost_bonus = S(44, 0);
Score virtual_king_m = S(2, 1);
Score virtual_king_b = S(3, 0);
Score rook_closed_file_penalty = S(13, 4);

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
				if(pt == BISHOP || pt == ROOK || pt == KING)
					continue;
                for (Square sq = SQ_ZERO; sq < 32; ++sq)
                {
                    int r = sq / 4;
                    int f = sq & 0x3;

		            if (1)
		            {
						// horizontal mirror
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
                        score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

                        score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
                        score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];

						if (0) 
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
            
            for (PieceType pt = BISHOP; pt <= KING; ++pt)
            {
				if (pt == QUEEN)
					continue;
                for (Square sq = SQ_ZERO; sq < 16; ++sq)
                {
                    int r = sq / 4;
                    int f = sq & 0x3;

					if (r >= f) 
					{
						// horizontal mirror
						score_table[make_piece(pt, WHITE)][(7 - r) * 8 + f]       = &piece_table[pt][sq];
						score_table[make_piece(pt, WHITE)][(7 - r) * 8 + (7 - f)] = &piece_table[pt][sq];
						score_table[make_piece(pt, BLACK)][r * 8 + f]             = &piece_table[pt][sq];
						score_table[make_piece(pt, BLACK)][r * 8 + (7 - f)]       = &piece_table[pt][sq];
						
						// vertical mirror
						score_table[make_piece(pt, WHITE)][56 ^ ((7 - r) * 8 + f)]       = &piece_table[pt][sq];
						score_table[make_piece(pt, WHITE)][56 ^ ((7 - r) * 8 + (7 - f))] = &piece_table[pt][sq];
						score_table[make_piece(pt, BLACK)][56 ^ (r * 8 + f)]             = &piece_table[pt][sq];
						score_table[make_piece(pt, BLACK)][56 ^ (r * 8 + (7 - f))]       = &piece_table[pt][sq];

						// diagonal mirror
						score_table[make_piece(pt, WHITE)][f * 8 + r]           = &piece_table[pt][sq];
						score_table[make_piece(pt, WHITE)][f * 8 + 7 - r]       = &piece_table[pt][sq];
						score_table[make_piece(pt, WHITE)][(7 - f) * 8 + r]     = &piece_table[pt][sq];
						score_table[make_piece(pt, WHITE)][(7 - f) * 8 + 7 - r] = &piece_table[pt][sq];
								
						score_table[make_piece(pt, BLACK)][f * 8 + r]           = &piece_table[pt][sq];
						score_table[make_piece(pt, BLACK)][f * 8 + 7 - r]       = &piece_table[pt][sq];
						score_table[make_piece(pt, BLACK)][(7 - f) * 8 + r]     = &piece_table[pt][sq];
						score_table[make_piece(pt, BLACK)][(7 - f) * 8 + 7 - r] = &piece_table[pt][sq];
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
