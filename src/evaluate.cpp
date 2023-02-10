#include "evaluate.h"

namespace Clovis {

	namespace Eval {

#define S(mg, eg) Score(mg, eg)

		Score pawn_table[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(128, 340), S(175, 315), S(186, 284), S(198, 266),
			S(92, 112), S(112, 108), S(143, 98), S(129, 100),
			S(76, 98), S(95, 93), S(98, 93), S(113, 81),
			S(65, 88), S(74, 89), S(94, 83), S(102, 83),
			S(73, 81), S(85, 82), S(91, 86), S(93, 90),
			S(67, 83), S(90, 81), S(81, 95), S(81, 98),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		Score knight_table[] = {
			S(184, 239), S(248, 264), S(239, 289), S(317, 278),
			S(285, 271), S(315, 291), S(403, 273), S(344, 304),
			S(326, 271), S(377, 283), S(369, 309), S(378, 307),
			S(351, 283), S(354, 302), S(362, 319), S(364, 325),
			S(339, 283), S(357, 301), S(363, 316), S(362, 320),
			S(334, 282), S(360, 290), S(363, 299), S(366, 314),
			S(335, 259), S(335, 279), S(351, 291), S(362, 293),
			S(305, 258), S(335, 252), S(322, 283), S(339, 284),
		};

		Score bishop_table[] = {
			S(317, 292), S(334, 299), S(335, 296), S(334, 302),
			S(318, 296), S(357, 282), S(350, 302), S(348, 305),
			S(325, 305), S(348, 306), S(359, 309), S(346, 315),
			S(326, 306), S(330, 307), S(334, 316), S(351, 315),
		};

		Score rook_table[] = {
			S(471, 538), S(488, 533), S(450, 544), S(501, 531),
			S(478, 538), S(474, 542), S(512, 531), S(521, 524),
			S(463, 535), S(494, 532), S(480, 533), S(476, 534),
			S(453, 539), S(462, 534), S(489, 536), S(482, 530),
			S(450, 535), S(472, 531), S(468, 533), S(479, 526),
			S(452, 524), S(473, 526), S(480, 519), S(474, 521),
			S(445, 527), S(474, 517), S(477, 520), S(484, 519),
			S(477, 514), S(469, 522), S(485, 518), S(484, 516),
		};

		Score queen_table[] = {
			S(931, 980), S(914, 1009), S(922, 1018), S(950, 996),
			S(929, 973), S(874, 1020), S(896, 1030), S(864, 1061),
			S(937, 979), S(922, 1001), S(922, 1009), S(893, 1051),
			S(915, 1015), S(909, 1027), S(903, 1022), S(899, 1036),
			S(933, 989), S(918, 1027), S(925, 1010), S(922, 1027),
			S(927, 997), S(947, 980), S(935, 1004), S(936, 1000),
			S(926, 977), S(943, 962), S(957, 962), S(952, 976),
			S(945, 952), S(936, 956), S(937, 959), S(951, 948),
		};

		Score king_table[] = {
			S(69, 1), S(105, 26), S(62, 53), S(69, 48),
			S(73, 37), S(111, 49), S(78, 73), S(52, 84),
			S(45, 52), S(97, 66), S(87, 80), S(80, 89),
			S(10, 54), S(76, 75), S(67, 92), S(51, 102),
		};

		Score passed_pawn[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(15, 137), S(0, 133), S(5, 107), S(8, 79),
			S(15, 71), S(4, 68), S(7, 45), S(6, 37),
			S(11, 38), S(0, 37), S(0, 22), S(0, 17),
			S(4, 10), S(0, 13), S(0, 2), S(0, 0),
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
			3, 3, 1, 0,
		};

		Score mobility[] = {
			S(0, 0), S(0, 0), S(5, 0), S(7, 2), S(4, 3), S(2, 5), S(0, 0),
		};

		short inner_ring_attack[] = {
			0, 6, 3, 5, 5, 4, 0,
		};

		short outer_ring_attack[] = {
			0, 1, 5, 1, 1, 3, 0,
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
		Score weak_queen_penalty = S(39, 0);
		Score rook_on_our_passer_file = S(6, 13);
		Score rook_on_their_passer_file = S(0, 44);
		Score knight_behind_pawn_bonus = S(4, 4);
		Score bishop_behind_pawn_bonus = S(6, 6);
		Score rook_on_seventh = S(6, 12);
		Score tall_pawn_penalty = S(10, 32);
		Score fianchetto_bonus = S(17, 17);
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
		
		inline bool is_doubled_pawn(Bitboard bb, Square sq) 
		{
			return multiple_bits(bb & file_masks[sq]);
		}
		
		constexpr bool is_isolated_pawn(Bitboard bb, Square sq) 
		{
			return !(bb & isolated_masks[sq]);
		}

		template<Colour US>
		constexpr bool is_passed_pawn(Bitboard bb, Square sq) 
		{
			return !(bb & passed_masks[US][sq]);
		}

		template<Colour US>
		constexpr bool is_outpost(Square sq, PTEntry& pte) 
		{
			return (outpost_masks[US] & sq & ~pte.potential_attacks[~US] & pte.attacks[US]);
		}
		
		template<Colour US>
		constexpr bool is_fianchetto(const Position& pos, Square sq)
		{
			return fianchetto_bishop_mask[US] & sq && multiple_bits(center_mask & Bitboards::get_attacks<BISHOP>(pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN], sq));
		}
		
		template<Colour US, PieceType PT>
		inline void king_danger(Bitboard attacks, PTEntry& pte)
		{
			Bitboard or_att_bb = attacks & king_zones[pte.ksq[~US]].outer_ring;
			Bitboard ir_att_bb = attacks & king_zones[pte.ksq[~US]].inner_ring;
		
			if (or_att_bb || ir_att_bb)
			{
				pte.weight[US] += inner_ring_attack[PT] * popcnt(ir_att_bb) + outer_ring_attack[PT] * popcnt(or_att_bb);
				if constexpr (PT != PAWN)
					++pte.n_att[US];
			}
		}
        
		template<Colour US, PieceType PT, bool SAFE>
		Score evaluate_majors(const Position& pos, PTEntry& pte)
		{
			static_assert(PT >= KNIGHT && PT < KING);

			Score score;
			Square sq;
			Bitboard bb = pos.pc_bb[make_piece(PT, US)];

			Bitboard transparent_occ =
				  PT == BISHOP ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK, ~US)]
				: PT == ROOK   ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK, US)]
				: pos.occ_bb[BOTH];

			while (bb)
			{
				sq = pop_lsb(bb);
				score += *score_table[make_piece(PT, US)][sq];
				Bitboard attacks = Bitboards::get_attacks<PT>(transparent_occ, sq);
				Bitboard safe_attacks = attacks & ~pte.attacks[~US];

				score += mobility[PT] * popcnt(safe_attacks & ~pos.occ_bb[US]);

				if constexpr (PT == KNIGHT)
				{
					if (is_outpost<US>(sq, pte))
						score += knight_outpost_bonus;
					if (pos.pc_bb[make_piece(PAWN, US)] & (sq + pawn_push(US)))
					    score += knight_behind_pawn_bonus;
				}
				if constexpr (PT == BISHOP)
				{
					if (is_outpost<US>(sq, pte))
						score += bishop_outpost_bonus;
					if (is_fianchetto<US>(pos, sq))
						score += fianchetto_bonus;
					if (bb)
						score += bishop_pair_bonus;
					if (multiple_bits(Bitboards::pawn_attacks[US][sq] & pos.pc_bb[make_piece(PAWN, US)]))
						score -= tall_pawn_penalty;
					if (pos.pc_bb[make_piece(PAWN, US)] & (sq + pawn_push(US)))
						score += bishop_behind_pawn_bonus;
				}
				if constexpr (PT == ROOK)
				{
					if (!(file_masks[sq] & (pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN])))
						score += rook_open_file_bonus;
					else if (!(file_masks[sq] & pos.pc_bb[make_piece(PAWN, US)]))
						score += rook_semi_open_file_bonus;
					else if (file_masks[sq] & pos.pc_bb[make_piece(PAWN, ~US)])
						score -= rook_closed_file_penalty;
					if (safe_attacks & rook_on_passer_masks[US][sq] & pte.passers[US])
						score += rook_on_our_passer_file;
					if (safe_attacks & rook_on_passer_masks[~US][sq] & pte.passers[~US])
						score += rook_on_their_passer_file;
					//if (relative_rank(US, rank_of(sq)) == RANK_7 && relative_rank(US, rank_of(pte.ksq[~US])) == RANK_8)
					//    score += rook_on_seventh;
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
			Score score;

			if (pos.pc_bb[make_piece(QUEEN, US)] && pos.get_game_phase() > safety_threshold)
			{
				score += evaluate_majors<US, KNIGHT, false>(pos, pte);
				score += evaluate_majors<US, BISHOP, false>(pos, pte);
				score += evaluate_majors<US, ROOK,   false>(pos, pte);
				score += evaluate_majors<US, QUEEN,  false>(pos, pte);

				// we dont count kings or pawns in n_att so the max should be 7, barring promotion trolling
				assert(pte.n_att[US] < 10);

				int virtual_mobility = popcnt(Bitboards::get_attacks<QUEEN>(pos.occ_bb[~US] ^ pos.pc_bb[make_piece(PAWN, US)], pte.ksq[~US]) & ~pte.attacks[~US]);

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
			constexpr Piece OUR_PAWN   = make_piece(PAWN, US);
			constexpr Piece THEIR_PAWN = make_piece(PAWN, ~US);

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

				king_danger<US, PAWN>(sqbb(sq + pawn_push(US)), pte);

				pte.attacks[US] |= Bitboards::pawn_attacks[US][sq];

				pte.potential_attacks[US] |= outpost_pawn_masks[US][sq];
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
					pte.weight[~US] -= *shield_table[US][US == WHITE ? lsb(fp) : msb(fp)];
				else
					pte.weight[~US] += *shield_table[0][f];
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
