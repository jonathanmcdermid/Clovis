#include "evaluate.h"

namespace Clovis {

    namespace Eval {

#define S(mg, eg) Score(mg, eg)

Score pawn_table[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(134, 337), S(178, 310), S(185, 280), S(201, 262),
	S(91, 112), S(114, 107), S(144, 98), S(132, 99),
	S(76, 98), S(95, 92), S(97, 93), S(113, 82),
	S(65, 89), S(74, 89), S(93, 84), S(105, 81),
	S(74, 81), S(84, 83), S(91, 85), S(93, 91),
	S(69, 84), S(91, 82), S(87, 94), S(78, 98),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score knight_table[] = {
	S(244, 244), S(0, 0), S(0, 0), S(0, 0),
	S(330, 259), S(322, 287), S(0, 0), S(0, 0),
	S(327, 279), S(354, 293), S(358, 306), S(0, 0),
	S(335, 286), S(355, 302), S(360, 318), S(359, 324),
};
Score bishop_table[] = {
	S(316, 295), S(0, 0), S(0, 0), S(0, 0),
	S(321, 300), S(368, 293), S(0, 0), S(0, 0),
	S(330, 301), S(346, 307), S(358, 310), S(0, 0),
	S(326, 307), S(344, 306), S(338, 318), S(350, 316),
};
Score rook_table[] = {
	S(462, 530), S(0, 0), S(0, 0), S(0, 0),
	S(448, 542), S(459, 541), S(0, 0), S(0, 0),
	S(464, 533), S(461, 540), S(465, 534), S(0, 0),
	S(460, 536), S(469, 535), S(459, 539), S(464, 536),
};
Score queen_table[] = {
	S(927, 976), S(913, 1003), S(922, 1011), S(949, 991),
	S(927, 967), S(871, 1018), S(897, 1026), S(865, 1058),
	S(934, 977), S(925, 996), S(923, 1006), S(894, 1048),
	S(913, 1013), S(905, 1029), S(901, 1019), S(894, 1038),
	S(930, 982), S(911, 1027), S(920, 1012), S(917, 1025),
	S(923, 997), S(947, 979), S(931, 1004), S(933, 995),
	S(925, 976), S(943, 956), S(956, 960), S(949, 975),
	S(943, 949), S(933, 955), S(936, 955), S(951, 944),
};
Score king_table[] = {
	S(107, 44), S(0, 0), S(0, 0), S(0, 0),
	S(144, 71), S(149, 96), S(0, 0), S(0, 0),
	S(102, 96), S(121, 117), S(132, 125), S(0, 0),
	S(108, 91), S(93, 129), S(114, 137), S(94, 147),
};
Score passed_pawn_bonus[] = {
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
	S(17, 137), S(1, 129), S(6, 105), S(9, 78),
	S(18, 68), S(4, 66), S(13, 42), S(8, 37),
	S(18, 35), S(0, 36), S(0, 20), S(0, 18),
	S(5, 10), S(0, 13), S(0, 3), S(0, 0),
	S(1, 9), S(2, 9), S(4, 0), S(0, 1),
	S(0, 0), S(0, 0), S(0, 0), S(0, 0),
};
Score mobility[] = {
	S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 3),
};
Score inner_ring_attack[] = {
	S(0, 0), S(2, 0), S(1, 7), S(3, 4), S(4, 0), S(3, 0),
};
Score outer_ring_attack[] = {
	S(0, 0), S(0, 0), S(4, 0), S(1, 0), S(1, 1), S(2, 0),
};
Score double_pawn_penalty = S(0, 8);
Score isolated_pawn_penalty = S(17, 8);
Score bishop_pair_bonus = S(39, 41);
Score rook_open_file_bonus = S(40, 0);
Score rook_semi_open_file_bonus = S(9, 15);
Score tempo_bonus = S(20, 17);
Score king_full_open_penalty = S(45, 7);
Score king_semi_open_penalty = S(10, 0);
Score king_adjacent_full_open_penalty = S(8, 8);
Score king_adjacent_semi_open_penalty = S(16, 0);
Score knight_outpost_bonus = S(38, 15);
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
                if(pt == BISHOP || pt == ROOK || pt == KING || pt == KNIGHT)
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
            
            for (PieceType pt = KNIGHT; pt <= KING; ++pt)
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
