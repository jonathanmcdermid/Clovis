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
			S(285, 271), S(316, 290), S(401, 275), S(345, 303),
			S(325, 271), S(383, 283), S(367, 310), S(379, 307),
			S(350, 284), S(351, 305), S(359, 320), S(363, 326),
			S(340, 283), S(356, 303), S(364, 316), S(362, 322),
			S(333, 282), S(359, 292), S(363, 300), S(368, 314),
			S(336, 261), S(328, 282), S(351, 292), S(363, 296),
			S(301, 262), S(337, 253), S(323, 283), S(339, 284),
		};

		Score bishop_table[] = {
			S(316, 293), S(334, 301), S(331, 298), S(331, 303),
			S(316, 299), S(367, 294), S(346, 304), S(346, 305),
			S(323, 307), S(346, 306), S(357, 309), S(343, 317),
			S(325, 307), S(329, 307), S(328, 318), S(349, 315),
		};

		Score rook_table[] = {
			S(468, 540), S(487, 534), S(449, 545), S(495, 533),
			S(476, 541), S(471, 545), S(514, 532), S(522, 524),
			S(461, 537), S(493, 533), S(475, 535), S(476, 535),
			S(453, 539), S(465, 533), S(487, 536), S(482, 530),
			S(447, 536), S(468, 531), S(468, 532), S(477, 527),
			S(450, 524), S(471, 526), S(477, 520), S(473, 521),
			S(444, 526), S(474, 516), S(474, 520), S(483, 519),
			S(477, 511), S(470, 520), S(484, 518), S(485, 515),
		};

		Score queen_table[] = {
			S(925, 976), S(908, 1005), S(921, 1011), S(945, 991),
			S(928, 969), S(871, 1021), S(897, 1025), S(866, 1060),
			S(935, 979), S(925, 997), S(923, 1007), S(892, 1050),
			S(913, 1014), S(904, 1028), S(900, 1021), S(894, 1038),
			S(930, 984), S(911, 1030), S(920, 1011), S(918, 1026),
			S(924, 997), S(945, 980), S(931, 1004), S(932, 997),
			S(925, 974), S(941, 957), S(955, 960), S(949, 974),
			S(943, 953), S(935, 955), S(937, 956), S(950, 944),
		};

		Score king_table[] = {
			S(65, 0), S(104, 26), S(66, 53), S(73, 50),
			S(68, 35), S(106, 50), S(74, 74), S(55, 85),
			S(40, 50), S(97, 66), S(94, 80), S(82, 91),
			S(6, 54), S(75, 75), S(70, 93), S(58, 103),
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
		Score weak_queen_penalty = S(35, 7);

#undef S

		const Score* piece_table[7] = { NULL, pawn_table, knight_table, bishop_table, rook_table, queen_table, king_table };
		const Score* passed_table[COLOUR_N][SQ_N];
		const Score* score_table[15][SQ_N];

		void init_eval()
		{
			init_values();
		}

		void init_values()
		{
			for (auto pt : {PAWN, KNIGHT, ROOK, QUEEN})
			{
 				for (Square sq = SQ_ZERO; sq < 32; ++sq)
				{
					int r = sq / 4;
					int f = sq & 0x3;
					
					// horizontal mirror
					score_table[make_piece(pt, WHITE)][((7 - r) << 3) + f]       = &piece_table[pt][sq];
					score_table[make_piece(pt, WHITE)][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

					score_table[make_piece(pt, BLACK)][(r << 3) + f]             = &piece_table[pt][sq];
					score_table[make_piece(pt, BLACK)][(r << 3) + (7 - f)]       = &piece_table[pt][sq];
				}
			}
			
			for (auto pt : {BISHOP, KING})
			{
 				for (Square sq = SQ_ZERO; sq < 16; ++sq)
				{
					int r = sq / 4;
					int f = sq & 0x3;
					
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
				}
			}
			
			/*
			for (auto pt : {})
			{
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
			*/
			
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
        
		template<Colour US, PieceType PT, bool SAFE>
		Score evaluate_majors(const Position& pos, PTEntry& pte)
		{
			constexpr Colour THEM = ~US;

			static_assert(PT >= KNIGHT && PT < KING);

			Score score;
			Square sq;
			Bitboard bb = pos.pc_bb[make_piece(PT, US)];

			Bitboard transparent_occ =
				  PT == BISHOP ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK, THEM)]
				: PT == ROOK   ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK, US)]
				: pos.occ_bb[BOTH];

			while (bb)
			{
				sq = pop_lsb(bb);
				score += *score_table[make_piece(PT, US)][sq];
				Bitboard attacks = Bitboards::get_attacks<PT>(transparent_occ, sq) & ~pte.attacks[THEM];

				score += mobility[PT] * popcnt(attacks & ~pos.occ_bb[US]);

				if constexpr (PT == KNIGHT)
				{
					if (outpost<US>(pos, sq))
						score += knight_outpost_bonus;
				}
				if constexpr (PT == BISHOP)
				{
					if (outpost<US>(pos, sq))
						score += bishop_outpost_bonus;
					if (bb)
						score += bishop_pair_bonus;
				}
				if constexpr (PT == ROOK)
				{
					if (!(file_masks[sq] & (pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN])))
						score += rook_open_file_bonus;
					else if (!(file_masks[sq] & pos.pc_bb[make_piece(PAWN, US)]))
						score += rook_semi_open_file_bonus;
					else if (file_masks[sq] & pos.pc_bb[make_piece(PAWN, THEM)])
						score -= rook_closed_file_penalty;
				}
				if constexpr (PT == QUEEN)
				{
					if (pos.discovery_threat<US>(sq))
						score -= weak_queen_penalty;
				}
				if constexpr (!SAFE)
					king_danger<US, PT>(attacks, pte);
			}

			return score;
		}

		template<Colour US>
		Score evaluate_all(const Position& pos, PTEntry& pte)
		{
			constexpr Colour THEM = ~US;
			
			Score score;

			if (pos.pc_bb[make_piece(QUEEN, US)])
			{
				score += evaluate_majors<US, KNIGHT, false>(pos, pte);
				score += evaluate_majors<US, BISHOP, false>(pos, pte);
				score += evaluate_majors<US, ROOK,   false>(pos, pte);
				score += evaluate_majors<US, QUEEN,  false>(pos, pte);

				// we dont count kings or pawns in n_att so the max should be 7, barring promotion trolling
				assert(pte.n_att[US] < 10);

				int virtual_mobility = popcnt(Bitboards::get_attacks<QUEEN>(pos.occ_bb[THEM] ^ pos.pc_bb[make_piece(PAWN, US)], pte.ksq[THEM]) & ~pte.attacks[THEM]);

				if (virtual_mobility > 4)
					pte.weight[US] += virtual_king_m * min(13, virtual_mobility) - virtual_king_b;
				
				score += pte.weight[US] * pte.weight[US] / (10 - pte.n_att[US]);
			}
			else
			{
				score += evaluate_majors<US, KNIGHT, true>(pos, pte);
				score += evaluate_majors<US, BISHOP, true>(pos, pte);
				score += evaluate_majors<US, ROOK,   true>(pos, pte);
				score += evaluate_majors<US, QUEEN,  true>(pos, pte);
			}

			return score;
		}

		template<Colour US>
		Score evaluate_pawns(const Position& pos, PTEntry& pte)
		{
			constexpr Colour THEM = ~US;

			constexpr Piece OUR_PAWN   = make_piece(PAWN, US);
			constexpr Piece THEIR_PAWN = make_piece(PAWN, THEM);

			Score score;

			Bitboard bb = pos.pc_bb[OUR_PAWN];

			while (bb)
			{
				Square sq = pop_lsb(bb);

				score += *score_table[OUR_PAWN][sq];

				if (doubled_pawn(pos.pc_bb[OUR_PAWN], sq))
					score -= double_pawn_penalty;

				if (isolated_pawn(pos.pc_bb[OUR_PAWN], sq))
					score -= isolated_pawn_penalty;

				if (passed_pawn<US>(pos.pc_bb[THEIR_PAWN], sq))
					score += *passed_table[US][sq];

				Bitboard attacks = Bitboards::pawn_attacks[US][sq];

				king_danger<US, PAWN>(attacks, pte);

				pte.attacks[US] |= attacks;
			}

			if (!(file_masks[pte.ksq[US]] & pos.pc_bb[OUR_PAWN]))
			{
				score -= (file_masks[pte.ksq[US]] & pos.pc_bb[THEIR_PAWN])
					? king_semi_open_penalty
					: king_full_open_penalty;
			}
			if (file_of(pte.ksq[US]) != FILE_A && !(file_masks[pte.ksq[US] + WEST] & pos.pc_bb[OUR_PAWN]))
			{
				score -= (file_masks[pte.ksq[US] + WEST] & pos.pc_bb[THEIR_PAWN])
					? king_adjacent_semi_open_penalty
					: king_adjacent_full_open_penalty;
			}
			if (file_of(pte.ksq[US]) != FILE_H && !(file_masks[pte.ksq[US] + EAST] & pos.pc_bb[OUR_PAWN]))
			{
				score -= (file_masks[pte.ksq[US] + EAST] & pos.pc_bb[THEIR_PAWN])
					? king_adjacent_semi_open_penalty
					: king_adjacent_full_open_penalty;
			}

			score += *score_table[make_piece(KING, US)][pte.ksq[US]];

			return score;
		}

		template<bool USE_TT>
		int evaluate(const Position& pos)
		{
			bool insufficient[COLOUR_N] = { 
				pos.is_insufficient<WHITE>(), 
				pos.is_insufficient<BLACK>() 
			};
			
			if (insufficient[WHITE] && insufficient[BLACK])
				return DRAW_SCORE;

			Colour us = pos.side;
			Colour them = ~us;

			int game_phase = pos.get_game_phase();

			Score score = (us == WHITE) ? tempo_bonus : -tempo_bonus;

			PTEntry pte = tt.probe_pawn(pos.bs->pkey);

			if (!USE_TT || pte.key != pos.bs->pkey)
			{
				pte.clear();
				pte.key = pos.bs->pkey;
				pte.ksq[WHITE] = lsb(pos.pc_bb[W_KING]);
				pte.ksq[BLACK] = lsb(pos.pc_bb[B_KING]);
				pte.score = evaluate_pawns<WHITE>(pos, pte) - evaluate_pawns<BLACK>(pos, pte);
				tt.new_pawn_entry(pte);
			}

			score += pte.score + evaluate_all<WHITE>(pos, pte) - evaluate_all<BLACK>(pos, pte);

			int eval = (score.mg * game_phase + score.eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;
			
			if (us == BLACK)
				eval = -eval;

			return insufficient[us] ? min(DRAW_SCORE, eval) : insufficient[them] ? max(DRAW_SCORE, eval) : eval;
		}
		
		// explicit template instantiations
		template int evaluate<true>(const Position& pos);
		template int evaluate<false>(const Position& pos);

	} // namespace Eval

} // namespace Clovis
