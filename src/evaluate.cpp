#include "evaluate.h"

namespace Clovis {

	namespace Eval {

#define S(mg, eg) Score(mg, eg)

		Score pawn_table[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(129, 339), S(175, 314), S(188, 281), S(199, 265),
			S(92, 113), S(113, 108), S(144, 98), S(129, 100),
			S(76, 98), S(95, 93), S(98, 93), S(114, 82),
			S(65, 88), S(74, 89), S(94, 83), S(102, 83),
			S(73, 81), S(85, 82), S(91, 86), S(93, 90),
			S(67, 83), S(90, 81), S(81, 95), S(81, 98),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		Score knight_table[] = {
			S(185, 239), S(247, 264), S(239, 290), S(315, 279),
			S(284, 271), S(314, 290), S(402, 273), S(344, 303),
			S(325, 272), S(377, 283), S(368, 309), S(378, 307),
			S(350, 283), S(354, 302), S(362, 319), S(363, 326),
			S(339, 283), S(357, 302), S(362, 315), S(362, 320),
			S(333, 281), S(360, 291), S(363, 299), S(367, 314),
			S(336, 260), S(335, 280), S(351, 290), S(362, 293),
			S(303, 258), S(336, 253), S(323, 283), S(338, 283),
		};

		Score bishop_table[] = {
			S(317, 292), S(333, 299), S(335, 296), S(333, 302),
			S(318, 296), S(364, 287), S(350, 302), S(348, 305),
			S(324, 306), S(348, 307), S(358, 309), S(346, 315),
			S(325, 307), S(330, 307), S(332, 317), S(351, 314),
		};

		Score rook_table[] = {
			S(470, 538), S(487, 533), S(451, 544), S(499, 532),
			S(477, 539), S(474, 542), S(511, 531), S(521, 524),
			S(461, 535), S(493, 532), S(480, 533), S(475, 534),
			S(452, 538), S(461, 534), S(489, 536), S(481, 530),
			S(450, 534), S(471, 531), S(468, 532), S(479, 526),
			S(452, 524), S(472, 525), S(480, 519), S(473, 521),
			S(444, 526), S(474, 517), S(477, 520), S(484, 519),
			S(476, 513), S(469, 522), S(485, 518), S(484, 516),
		};

		Score queen_table[] = {
			S(932, 980), S(913, 1008), S(923, 1018), S(949, 994),
			S(929, 973), S(874, 1020), S(896, 1028), S(862, 1061),
			S(937, 978), S(921, 1001), S(921, 1008), S(891, 1050),
			S(914, 1015), S(908, 1028), S(903, 1022), S(899, 1036),
			S(932, 987), S(917, 1027), S(925, 1010), S(921, 1027),
			S(927, 998), S(947, 979), S(935, 1005), S(935, 1001),
			S(926, 976), S(942, 960), S(957, 962), S(952, 976),
			S(946, 952), S(935, 956), S(937, 959), S(951, 948),
		};

		Score king_table[] = {
			S(69, 1), S(105, 26), S(62, 53), S(69, 48),
			S(73, 37), S(111, 49), S(77, 72), S(52, 84),
			S(44, 52), S(97, 66), S(89, 80), S(80, 89),
			S(10, 54), S(76, 75), S(68, 92), S(52, 102),
		};

		Score passed_pawn[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(16, 136), S(0, 133), S(5, 107), S(8, 79),
			S(15, 71), S(4, 68), S(9, 45), S(6, 37),
			S(11, 38), S(0, 36), S(0, 21), S(0, 17),
			S(5, 10), S(0, 13), S(0, 3), S(0, 0),
			S(0, 10), S(0, 9), S(0, 0), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		short pawn_shield[] = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 3,
			0, 0, 0, 0,
			0, 2, 6, 2,
			4, 3, 1, 0,
		};

		Score mobility[] = {
			S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(2, 5),
		};

		short inner_ring_attack[] = {
			0, 2, 3, 5, 5, 4,
		};

		short outer_ring_attack[] = {
			0, 0, 5, 1, 2, 3,
		};

		Score double_pawn_penalty = S(1, 8);
		Score isolated_pawn_penalty = S(17, 8);
		Score bishop_pair_bonus = S(40, 43);
		Score rook_open_file_bonus = S(32, 0);
		Score rook_semi_open_file_bonus = S(8, 13);
		Score tempo_bonus = S(19, 19);
		Score king_full_open_penalty = S(41, 8);
		Score king_semi_open_penalty = S(7, 0);
		Score king_adjacent_full_open_penalty = S(5, 10);
		Score king_adjacent_semi_open_penalty = S(10, 0);
		Score knight_outpost_bonus = S(38, 14);
		Score bishop_outpost_bonus = S(45, 0);
		short virtual_king_m = 3;
		short virtual_king_b = 1;
		Score rook_closed_file_penalty = S(14, 1);
		Score weak_queen_penalty = S(38, 1);
		Score rook_on_our_passer_file = S(8, 10);
		Score rook_on_their_passer_file = S(6, 35);
		Score tall_pawn_penalty = S(10, 32);
		Score fianchetto_bonus = S(11, 10);
		short safety_threshold = 8;
		short opposite_bishops_scaling = 15;

#undef S

		const Score* piece_table[7] = { NULL, pawn_table, knight_table, bishop_table, rook_table, queen_table, king_table };
		const Score* passed_table[COLOUR_N][SQ_N];
		const short* shield_table[COLOUR_N][SQ_N];
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

				passed_table[WHITE][((7 - r) << 3) + f]       = &passed_pawn[sq];
				passed_table[WHITE][((7 - r) << 3) + (7 - f)] = &passed_pawn[sq];

				passed_table[BLACK][(r << 3) + f]             = &passed_pawn[sq];
				passed_table[BLACK][(r << 3) + (7 - f)]       = &passed_pawn[sq];
				
				shield_table[WHITE][((7 - r) << 3) + f]       = &pawn_shield[sq];
				shield_table[WHITE][((7 - r) << 3) + (7 - f)] = &pawn_shield[sq];

				shield_table[BLACK][(r << 3) + f]             = &pawn_shield[sq];
				shield_table[BLACK][(r << 3) + (7 - f)]       = &pawn_shield[sq];
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
				Bitboard attacks = Bitboards::get_attacks<PT>(transparent_occ, sq);
				Bitboard safe_attacks = attacks & ~pte.attacks[THEM];

				score += mobility[PT] * popcnt(safe_attacks & ~pos.occ_bb[US]);

				if constexpr (PT == KNIGHT)
				{
					if (is_outpost<US>(pos, sq))
						score += knight_outpost_bonus;
				}
				if constexpr (PT == BISHOP)
				{
					if (is_outpost<US>(pos, sq))
						score += bishop_outpost_bonus;
					if (bb)
						score += bishop_pair_bonus;
					if (popcnt(Bitboards::pawn_attacks[US][sq] & pos.pc_bb[make_piece(PAWN, US)]) == 2)
						score -= tall_pawn_penalty;
					if (fianchetto_bishop_mask[US] & sq
						&& popcnt(center_mask & attacks) == 2)
						score += fianchetto_bonus;
				}
				if constexpr (PT == ROOK)
				{
					if (!(file_masks[sq] & (pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN])))
						score += rook_open_file_bonus;
					else if (!(file_masks[sq] & pos.pc_bb[make_piece(PAWN, US)]))
						score += rook_semi_open_file_bonus;
					else if (file_masks[sq] & pos.pc_bb[make_piece(PAWN, THEM)])
						score -= rook_closed_file_penalty;
					if (safe_attacks & rook_on_passer_masks[US][sq] & pte.passers[US])
						score += rook_on_our_passer_file;
					if (safe_attacks & rook_on_passer_masks[~US][sq] & pte.passers[THEM])
						score += rook_on_their_passer_file;
				}
				if constexpr (PT == QUEEN)
				{
					if (pos.discovery_threat<US>(sq))
						score -= weak_queen_penalty;
				}
				if constexpr (!SAFE)
					king_danger<US, PT>(safe_attacks, pte);
			}

			return score;
		}

		template<Colour US>
		Score evaluate_all(const Position& pos, PTEntry& pte)
		{
			constexpr Colour THEM = ~US;
			
			Score score;

			if (pos.pc_bb[make_piece(QUEEN, US)] && pos.get_game_phase() > safety_threshold)
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

				if (pte.weight[US] > 0)
				    score.mg += pte.weight[US] * pte.weight[US] / (20 - 2 * pte.n_att[US]);
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

				if (is_doubled_pawn(pos.pc_bb[OUR_PAWN], sq))
					score -= double_pawn_penalty;

				if (is_isolated_pawn(pos.pc_bb[OUR_PAWN], sq))
					score -= isolated_pawn_penalty;

				if (is_passed_pawn<US>(pos.pc_bb[THEIR_PAWN], sq))
				{
					score += *passed_table[US][sq];
					pte.passers[US] |= sq;
				}

				Bitboard attacks = Bitboards::pawn_attacks[US][sq];

				king_danger<US, PAWN>(attacks, pte);

				pte.attacks[US] |= attacks;
			}
			
			File kf = file_of(pte.ksq[US]);

			if (!(file_masks[pte.ksq[US]] & pos.pc_bb[OUR_PAWN]))
			{
				score -= (file_masks[pte.ksq[US]] & pos.pc_bb[THEIR_PAWN])
					? king_semi_open_penalty
					: king_full_open_penalty;
			}
			if (kf != FILE_A && !(file_masks[pte.ksq[US] + WEST] & pos.pc_bb[OUR_PAWN]))
			{
				score -= (file_masks[pte.ksq[US] + WEST] & pos.pc_bb[THEIR_PAWN])
					? king_adjacent_semi_open_penalty
					: king_adjacent_full_open_penalty;
			}
			if (kf != FILE_H && !(file_masks[pte.ksq[US] + EAST] & pos.pc_bb[OUR_PAWN]))
			{
				score -= (file_masks[pte.ksq[US] + EAST] & pos.pc_bb[THEIR_PAWN])
					? king_adjacent_semi_open_penalty
					: king_adjacent_full_open_penalty;
			}
			
			File cf = kf == FILE_H ? FILE_G : kf == FILE_A ? FILE_B : kf;
			
			for (File f = cf - 1; f <= cf + 1; ++f)
			{
				Bitboard fp = pos.pc_bb[OUR_PAWN] & file_masks[f];

				if (fp)
					pte.weight[THEM] -= *shield_table[US][US == WHITE ? lsb(fp) : msb(fp)];
				else
					pte.weight[THEM] += *shield_table[0][f];
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

			int scaling;

			if (game_phase == 2 && popcnt(pos.pc_bb[W_BISHOP]) == 1 && popcnt(pos.pc_bb[B_BISHOP]) == 1 
				&& bool(pos.pc_bb[W_BISHOP] & light_mask) == bool(pos.pc_bb[B_BISHOP] & dark_mask))
				scaling = opposite_bishops_scaling;
			else
				scaling = MAX_SCALING;

			if (scaling < MAX_SCALING)
				eval = eval * scaling / MAX_SCALING;

			return insufficient[us] ? min(DRAW_SCORE, eval) : insufficient[them] ? max(DRAW_SCORE, eval) : eval;
		}
		
		// explicit template instantiations
		template int evaluate<true>(const Position& pos);
		template int evaluate<false>(const Position& pos);

	} // namespace Eval

} // namespace Clovis
