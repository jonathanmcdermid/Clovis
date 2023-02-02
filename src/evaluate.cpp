#include "evaluate.h"

namespace Clovis {

	namespace Eval {

#define S(mg, eg) Score(mg, eg)

		Score pawn_table[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(132, 337), S(177, 312), S(190, 279), S(203, 262),
			S(93, 111), S(115, 108), S(146, 97), S(131, 99),
			S(76, 98), S(95, 93), S(97, 93), S(114, 82),
			S(66, 88), S(74, 89), S(94, 83), S(102, 83),
			S(73, 81), S(85, 82), S(91, 86), S(93, 90),
			S(67, 83), S(90, 81), S(82, 93), S(79, 96),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		Score knight_table[] = {
			S(182, 240), S(245, 265), S(238, 290), S(314, 279),
			S(285, 270), S(312, 290), S(401, 273), S(344, 303),
			S(323, 272), S(378, 283), S(368, 309), S(377, 305),
			S(348, 284), S(353, 303), S(361, 319), S(364, 325),
			S(339, 284), S(356, 302), S(363, 315), S(362, 320),
			S(332, 281), S(361, 290), S(363, 299), S(368, 313),
			S(335, 261), S(333, 281), S(352, 290), S(363, 294),
			S(304, 259), S(336, 252), S(324, 283), S(340, 283),
		};

		Score bishop_table[] = {
			S(317, 293), S(333, 300), S(332, 298), S(331, 302),
			S(315, 298), S(367, 291), S(348, 303), S(347, 304),
			S(324, 306), S(346, 306), S(357, 308), S(343, 316),
			S(324, 307), S(328, 308), S(330, 316), S(350, 313),
		};

		Score rook_table[] = {
			S(468, 538), S(487, 533), S(451, 545), S(499, 533),
			S(475, 540), S(473, 543), S(511, 531), S(519, 524),
			S(460, 534), S(494, 531), S(477, 533), S(475, 533),
			S(450, 538), S(463, 534), S(489, 536), S(480, 528),
			S(449, 534), S(472, 531), S(469, 532), S(479, 526),
			S(452, 523), S(471, 526), S(479, 519), S(472, 521),
			S(444, 527), S(473, 518), S(476, 520), S(484, 519),
			S(476, 513), S(470, 522), S(485, 517), S(484, 516),
		};

		Score queen_table[] = {
			S(932, 979), S(912, 1008), S(923, 1013), S(947, 993),
			S(927, 971), S(871, 1021), S(895, 1028), S(862, 1059),
			S(935, 977), S(921, 1000), S(921, 1006), S(890, 1049),
			S(913, 1015), S(905, 1027), S(901, 1021), S(894, 1035),
			S(931, 985), S(915, 1030), S(921, 1011), S(918, 1025),
			S(927, 997), S(945, 980), S(933, 1004), S(933, 999),
			S(926, 975), S(942, 959), S(956, 961), S(952, 976),
			S(945, 954), S(936, 956), S(938, 959), S(953, 948),
		};

		Score king_table[] = {
			S(67, 1), S(104, 24), S(62, 53), S(70, 49),
			S(72, 36), S(111, 50), S(77, 74), S(55, 85),
			S(42, 52), S(98, 67), S(93, 81), S(81, 90),
			S(11, 55), S(78, 75), S(69, 92), S(55, 101),
		};

		Score passed_pawn[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(18, 135), S(0, 129), S(6, 105), S(10, 78),
			S(18, 67), S(4, 66), S(11, 42), S(7, 37),
			S(14, 36), S(0, 36), S(0, 19), S(0, 17),
			S(7, 9), S(0, 13), S(0, 1), S(0, 0),
			S(0, 10), S(0, 8), S(0, 0), S(0, 1),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		short pawn_shield[] = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 2,
			0, 0, 0, 0,
			0, 2, 5, 1,
			2, 2, 1, 0,
		};

		Score mobility[] = {
			S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(3, 5),
		};

		short inner_ring_attack[] = {
			0, 2, 2, 4, 4, 3,
		};

		short outer_ring_attack[] = {
			0, 0, 3, 1, 1, 2,
		};

		Score double_pawn_penalty = S(1, 8);
		Score isolated_pawn_penalty = S(17, 8);
		Score bishop_pair_bonus = S(41, 43);
		Score rook_open_file_bonus = S(33, 0);
		Score rook_semi_open_file_bonus = S(6, 9);
		Score tempo_bonus = S(19, 19);
		Score king_full_open_penalty = S(39, 7);
		Score king_semi_open_penalty = S(6, 0);
		Score king_adjacent_full_open_penalty = S(6, 9);
		Score king_adjacent_semi_open_penalty = S(10, 0);
		Score knight_outpost_bonus = S(38, 14);
		Score bishop_outpost_bonus = S(45, 0);
		short virtual_king_m = 2;
		short virtual_king_b = 0;
		Score rook_closed_file_penalty = S(15, 1);
		Score weak_queen_penalty = S(37, 2);
		Score rook_on_our_passer_file = S(7, 7);
		Score rook_on_their_passer_file = S(0, 26);
		short safety_threshold = 8;

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
				Bitboard attacks = Bitboards::get_attacks<PT>(transparent_occ, sq) & ~pte.attacks[THEM];

				score += mobility[PT] * popcnt(attacks & ~pos.occ_bb[US]);

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
				}
				if constexpr (PT == ROOK)
				{
					if (!(file_masks[sq] & (pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN])))
						score += rook_open_file_bonus;
					else if (!(file_masks[sq] & pos.pc_bb[make_piece(PAWN, US)]))
						score += rook_semi_open_file_bonus;
					else if (file_masks[sq] & pos.pc_bb[make_piece(PAWN, THEM)])
						score -= rook_closed_file_penalty;
					if (attacks & file_masks[sq] & pte.passers[US])
						score += rook_on_our_passer_file;
					if (attacks & file_masks[sq] & pte.passers[THEM])
						score += rook_on_their_passer_file;
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
				    score.mg += pte.weight[US] * pte.weight[US] / (10 - pte.n_att[US]);
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

			return insufficient[us] ? min(DRAW_SCORE, eval) : insufficient[them] ? max(DRAW_SCORE, eval) : eval;
		}
		
		// explicit template instantiations
		template int evaluate<true>(const Position& pos);
		template int evaluate<false>(const Position& pos);

	} // namespace Eval

} // namespace Clovis
