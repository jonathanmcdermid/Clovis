#include "evaluate.h"

namespace Clovis {

	namespace Eval {

		const Score* piece_table[7] = { NULL, pawn_psqt, knight_psqt, bishop_psqt, rook_psqt, queen_psqt, king_psqt };
		const Score* passed_table[COLOUR_N][SQ_N];
		const short* shield_table[COLOUR_N][SQ_N];
		const Score* major_table[7][SQ_N];
		const Score* pawn_table[COLOUR_N][SQ_N];

		int T[TI_MISC][PHASE_N];

		void init_eval()
		{
			for (Square sq = SQ_ZERO; sq < 32; ++sq)
			{
				int r = sq / 4;
				int f = sq & 0x3;
				
				// horizontal mirror
				pawn_table[WHITE][((7 - r) << 3) + f]       = &pawn_psqt[sq];
				pawn_table[WHITE][((7 - r) << 3) + (7 - f)] = &pawn_psqt[sq];

				pawn_table[BLACK][(r << 3) + f]             = &pawn_psqt[sq];
				pawn_table[BLACK][(r << 3) + (7 - f)]       = &pawn_psqt[sq];
			}
			
			for (auto pt : {KNIGHT, BISHOP, ROOK, QUEEN, KING})
			{
 				for (Square sq = SQ_ZERO; sq < 16; ++sq)
				{
					int r = sq / 4;
					int f = sq & 0x3;
					
					// horizontal mirror
					major_table[pt][((7 - r) << 3) + f]       = &piece_table[pt][sq];
					major_table[pt][((7 - r) << 3) + (7 - f)] = &piece_table[pt][sq];

					// vertical mirror
					major_table[pt][56 ^ (((7 - r) << 3) + f)]       = &piece_table[pt][sq];
					major_table[pt][56 ^ (((7 - r) << 3) + (7 - f))] = &piece_table[pt][sq];				
				}
			}
			
			/*for (auto pt : {})
			{
				for (Square sq = SQ_ZERO; sq < 16; ++sq)
				{
					int r = sq / 4;
					int f = sq & 0x3;

					if (r >= f) 
					{
						// horizontal mirror
						score_table[pt][(7 - r) * 8 + f]       = &piece_table[pt][sq];
						score_table[pt][(7 - r) * 8 + (7 - f)] = &piece_table[pt][sq];

						// vertical mirror
						score_table[pt][56 ^ ((7 - r) * 8 + f)]       = &piece_table[pt][sq];
						score_table[pt][56 ^ ((7 - r) * 8 + (7 - f))] = &piece_table[pt][sq];

						// diagonal mirror
						score_table[pt][f * 8 + r]           = &piece_table[pt][sq];
						score_table[pt][f * 8 + 7 - r]       = &piece_table[pt][sq];
						score_table[pt][(7 - f) * 8 + r]     = &piece_table[pt][sq];
						score_table[pt][(7 - f) * 8 + 7 - r] = &piece_table[pt][sq];
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

			// convenience for making masks
			/*for (Square sq1 = SQ_ZERO; sq1 < SQ_N; ++sq1)
			{
				Bitboard bb = 0ULL;

				for (Square sq2 = SQ_ZERO; sq2 < SQ_N; ++sq2)
					if (rank_of(sq2) < rank_of(sq1))
						bb |= sq2;

				//Bitboards::print_bitboard(bb);

				cout << "0x" << hex << bb << "ULL, ";

				if (!((sq1 + 1) % 4))
					cout << endl;
			}*/
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
		constexpr bool is_candidate_passer(Bitboard our_pawns, Bitboard their_pawns, Square sq)
		{
			if (file_masks[file_of(sq)] & their_pawns)
				return false;
			do {
				int support = popcnt(Bitboards::pawn_attacks[~US][sq + pawn_push(US)] & our_pawns);
				int danger  = popcnt(Bitboards::pawn_attacks[ US][sq + pawn_push(US)] & their_pawns);
				if (danger > support)
					return false;
				sq += pawn_push(US);
			} while (relative_rank(US, rank_of(sq)) != RANK_7);
			
			return true;
		}

		template<Colour US>
		constexpr bool is_outpost(Square sq, EvalInfo& ei) 
		{
			return (outpost_masks[US] & sq & ~ei.potential_pawn_attacks[~US] & ei.pawn_attacks[US]);
		}
		
		template<Colour US>
		constexpr bool is_fianchetto(const Position& pos, Square sq)
		{
			return fianchetto_bishop_mask[US] & sq && multiple_bits(center_mask & Bitboards::get_attacks<BISHOP>(pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN], sq));
		}
		
		template<Colour US, PieceType PT, bool TRACE>
		inline void king_danger(Bitboard attacks, EvalInfo& ei)
		{
			Bitboard or_att_bb = attacks & king_zones[ei.ksq[~US]].outer_ring;
			Bitboard ir_att_bb = attacks & king_zones[ei.ksq[~US]].inner_ring;
		
			if (or_att_bb || ir_att_bb)
			{
				ei.weight[US] += inner_ring_attack[PT] * popcnt(ir_att_bb) + outer_ring_attack[PT] * popcnt(or_att_bb);
				if constexpr (PT != PAWN) ++ei.n_att[US];
				if constexpr (TRACE) T[SAFETY_INNER_RING + PT][US] += popcnt(ir_att_bb);
				if constexpr (TRACE) T[SAFETY_OUTER_RING + PT][US] += popcnt(or_att_bb);
			}
		}
		
		template<Colour US, PieceType PT>
		void psqt_trace(Square sq)
		{
			if constexpr (PT == PAWN)   ++T[PAWN_PSQT   + source32[relative_square(US, sq)]][US];
			if constexpr (PT == KNIGHT) ++T[KNIGHT_PSQT + source16[sq]][US];
			if constexpr (PT == BISHOP) ++T[BISHOP_PSQT + source16[sq]][US];
			if constexpr (PT == ROOK)   ++T[ROOK_PSQT   + source16[sq]][US];
			if constexpr (PT == QUEEN)  ++T[QUEEN_PSQT  + source16[sq]][US];
			if constexpr (PT == KING)   ++T[KING_PSQT   + source16[sq]][US];
		}

		template<Colour US, PieceType PT>
		Bitboard worthy_trades(const Position& pos)
		{
			static_assert(PT >= KNIGHT && PT <= QUEEN);

			return (PT == QUEEN)  ? pos.pc_bb[make_piece(KING, ~US)] | pos.pc_bb[make_piece(QUEEN, ~US) ]
			     : (PT == ROOK)   ? pos.pc_bb[make_piece(KING, ~US)] | pos.pc_bb[make_piece(QUEEN, ~US)] | pos.pc_bb[make_piece(ROOK, ~US)]
			     : pos.occ_bb[~US] ^ pos.pc_bb[make_piece(PAWN, ~US)];
		}
		
		template<Colour US, PieceType PT, bool SAFETY, bool TRACE>
		Score evaluate_majors(const Position& pos, EvalInfo& ei)
		{
			static_assert(PT >= KNIGHT && PT <= QUEEN);

			Score score;
			Square sq;
			Bitboard bb = pos.pc_bb[make_piece(PT, US)];

			Bitboard transparent_occ =
				  PT == BISHOP ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK, ~US)] ^ ei.ksq[~US]
				: PT == ROOK   ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK,  US)] ^ ei.ksq[~US]
				: pos.occ_bb[BOTH];

			while (bb)
			{
				sq = pop_lsb(bb);
				score += *major_table[PT][sq];
				Bitboard attacks = Bitboards::get_attacks<PT>(transparent_occ, sq);

				Square pinner = pos.get_pinner<US>(sq);

				if (pinner != SQ_NONE)
					attacks &= between_squares(ei.ksq[US], pinner) | pinner;

				Bitboard safe_attacks = attacks & (~ei.pawn_attacks[~US] | worthy_trades<US, PT>(pos));

				score += mobility[PT] * (popcnt(safe_attacks & ~pos.occ_bb[BOTH]) + 5 * popcnt(safe_attacks & pos.occ_bb[~US]));

				if constexpr (SAFETY) king_danger<US, PT, TRACE>(safe_attacks, ei);
				if constexpr (TRACE) psqt_trace<US, PT>(sq);
				if constexpr (TRACE) T[MOBILITY + PT][US] += popcnt(safe_attacks & ~pos.occ_bb[BOTH]) + 5 * popcnt(safe_attacks & pos.occ_bb[~US]);
				if constexpr (PT == KNIGHT)
				{
					if (is_outpost<US>(sq, ei))
					{
						score += knight_outpost_bonus;
						if constexpr (TRACE) ++T[KNIGHT_OUTPOST][US];
					}
				}
				if constexpr (PT == BISHOP)
				{
					if (bb)
					{
						score += bishop_pair_bonus;
						if constexpr (TRACE) ++T[BISHOP_PAIR][US];
					}
					if (is_fianchetto<US>(pos, sq))
					{
						score += fianchetto_bonus;
						if constexpr (TRACE) ++T[FIANCHETTO][US];
					}
					else 
					{
						if (is_outpost<US>(sq, ei))
						{
							score += bishop_outpost_bonus;
							if constexpr (TRACE) ++T[BISHOP_OUTPOST][US];
						}
						if (multiple_bits(Bitboards::pawn_attacks[US][sq] & pos.pc_bb[make_piece(PAWN, US)]))
						{
							score -= tall_pawn_penalty;
							if constexpr (TRACE) --T[TALL_PAWN][US];
						}
					}
				}
				if constexpr (PT == ROOK)
				{
					if (!(file_masks[sq] & (pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN])))
					{
						score += rook_open_file_bonus;
						if constexpr (TRACE) ++T[ROOK_FULL][US];
					}
					else 
					{
						if (!(file_masks[sq] & pos.pc_bb[make_piece(PAWN, US)]))
						{
							score += rook_semi_open_file_bonus;
							if constexpr (TRACE) ++T[ROOK_SEMI][US];
						}
						else if (file_masks[sq] & pos.pc_bb[make_piece(PAWN, ~US)])
						{
							score -= rook_closed_file_penalty;
							if constexpr (TRACE) --T[ROOK_CLOSED][US];
						}
						if (safe_attacks & rook_on_passer_masks[US][sq] & ei.passers[US])
						{
							score += rook_on_our_passer_file;
							if constexpr (TRACE) ++T[ROOK_OUR_PASSER][US];
						}
						if (safe_attacks & rook_on_passer_masks[~US][sq] & ei.passers[~US])
						{
							score += rook_on_their_passer_file;
							if constexpr (TRACE) ++T[ROOK_THEIR_PASSER][US];
						}
					}
					if (relative_rank(US, rank_of(sq)) == RANK_7 && relative_rank(US, rank_of(ei.ksq[~US])) == RANK_8)
					{
						score += rook_on_seventh;
						if constexpr (TRACE) ++T[ROOK_ON_SEVENTH][US];
					}
				}
				if constexpr (PT == QUEEN)
				{
					score += queen_forward_bonus * popcnt(safe_attacks & forward_masks[US][sq]);
					if constexpr (TRACE) T[QUEEN_FORWARD][US] += popcnt(safe_attacks & forward_masks[US][sq]);
					if (pos.discovery_threat<US>(sq))
					{
						score -= weak_queen_penalty;
						if constexpr (TRACE) --T[WEAK_QUEEN][US];
					}
				}
			}

			return score;
		}

		template<Colour US, bool TRACE>
		Score evaluate_all(const Position& pos, EvalInfo& ei)
		{
			Score score;

			if (pos.pc_bb[make_piece(QUEEN, US)] && pos.get_game_phase() > 8)
			{
				score += evaluate_majors<US, KNIGHT, true, TRACE>(pos, ei);
				score += evaluate_majors<US, BISHOP, true, TRACE>(pos, ei);
				score += evaluate_majors<US, ROOK,   true, TRACE>(pos, ei);
				score += evaluate_majors<US, QUEEN,  true, TRACE>(pos, ei);

				// we dont count kings or pawns in n_att so the max should be 7, barring promotion trolling
				assert(ei.n_att[US] < 10);

				int mob = popcnt(Bitboards::get_attacks<QUEEN>(pos.occ_bb[~US] ^ pos.pc_bb[make_piece(PAWN, US)], ei.ksq[~US]) & ~ei.pawn_attacks[~US]);

				if (mob > 4)
				{
					ei.weight[US] += virtual_mobility * min(13, mob);
					if constexpr (TRACE) T[SAFETY_VIRTUAL_MOBILITY][US] = min(13, mob);
				}
				
				if (ei.weight[US] > 0) 
				{
					score.mg += ei.weight[US] * ei.weight[US] / (720 - attack_factor * ei.n_att[US]);
					if constexpr (TRACE) T[SAFETY_N_ATT][US] = ei.n_att[US];
				}
				else if constexpr (TRACE) for (int i = TI_SAFETY; i < TI_N; ++i) T[i][US] = 0;
			}
			else
			{
				score += evaluate_majors<US, KNIGHT, false, TRACE>(pos, ei);
				score += evaluate_majors<US, BISHOP, false, TRACE>(pos, ei);
				score += evaluate_majors<US, ROOK,   false, TRACE>(pos, ei);
				score += evaluate_majors<US, QUEEN,  false, TRACE>(pos, ei);
				
				if constexpr (TRACE) for (int i = TI_SAFETY; i < TI_N; ++i) T[i][US] = 0;
			}

			return score;
		}

		template<Colour US, bool TRACE>
		Score evaluate_pawns(const Position& pos, EvalInfo& ei)
		{
			constexpr Piece OUR_PAWN   = make_piece(PAWN, US);
			constexpr Piece THEIR_PAWN = make_piece(PAWN, ~US);

			Score score;

			Bitboard bb = pos.pc_bb[OUR_PAWN];

			while (bb)
			{
				Square sq = pop_lsb(bb);

				if constexpr (TRACE) psqt_trace<US, PAWN>(sq);

				score += *pawn_table[US][sq];

				if (is_doubled_pawn(pos.pc_bb[OUR_PAWN], sq))
				{
					score -= double_pawn_penalty;
					if constexpr (TRACE) --T[DOUBLE_PAWN][US];
				}

				if (is_isolated_pawn(pos.pc_bb[OUR_PAWN], sq))
				{
					score -= isolated_pawn_penalty;
					if constexpr (TRACE) --T[ISOLATED_PAWN][US];
				}

				if (is_passed_pawn<US>(pos.pc_bb[THEIR_PAWN], sq))
				{
					ei.passers[US] |= sq;
					score += *passed_table[US][sq];
					if (TRACE && relative_rank(US, rank_of(sq)) != RANK_7) 
						++T[PASSED_PAWN + source32[relative_square(US, sq)]][US];
				}
				else if (is_candidate_passer<US>(pos.pc_bb[OUR_PAWN], pos.pc_bb[THEIR_PAWN], sq))
				{
					score += candidate_passer[relative_rank(US, rank_of(sq))];
					if constexpr (TRACE) ++T[CANDIDATE_PASSER + relative_rank(US, rank_of(sq))][US];
				}

				king_danger<US, PAWN, TRACE>(sqbb(sq + pawn_push(US)), ei);

				ei.pawn_attacks[US] |= Bitboards::pawn_attacks[US][sq];
				ei.potential_pawn_attacks[US] |= outpost_pawn_masks[US][sq];
			}
			
			File kf = file_of(ei.ksq[US]);

			if (!(file_masks[ei.ksq[US]] & pos.pc_bb[OUR_PAWN]))
			{
				if (file_masks[ei.ksq[US]] & pos.pc_bb[THEIR_PAWN])
				{
					score -= king_semi_open_penalty;
					if constexpr (TRACE) --T[KING_SEMI][US];
				}
				else
				{
					score -= king_full_open_penalty;
					if constexpr (TRACE) --T[KING_FULL][US];
				}
			}
			if (kf != FILE_A && !(file_masks[ei.ksq[US] + WEST] & pos.pc_bb[OUR_PAWN]))
			{
				if (file_masks[ei.ksq[US] + WEST] & pos.pc_bb[THEIR_PAWN])
				{
					score -= king_adjacent_semi_open_penalty;
					if constexpr (TRACE) --T[KING_ADJ_SEMI][US];
				}
				else
				{
					score -= king_adjacent_full_open_penalty;
					if constexpr (TRACE) --T[KING_ADJ_FULL][US];
				}
			}
			if (kf != FILE_H && !(file_masks[ei.ksq[US] + EAST] & pos.pc_bb[OUR_PAWN]))
			{
				if (file_masks[ei.ksq[US] + EAST] & pos.pc_bb[THEIR_PAWN])
				{
					score -= king_adjacent_semi_open_penalty;
					if constexpr (TRACE) --T[KING_ADJ_SEMI][US];
				}
				else
				{
					score -= king_adjacent_full_open_penalty;
					if constexpr (TRACE) --T[KING_ADJ_FULL][US];
				}
			}
			
			File cf = kf == FILE_H ? FILE_G : kf == FILE_A ? FILE_B : kf;
			
			for (File f = cf - 1; f <= cf + 1; ++f)
			{
				Bitboard fp = pos.pc_bb[OUR_PAWN] & file_masks[f];

				if (fp)
				{
					//ei.weight[~US] -= *shield_table[US][US == WHITE ? lsb(fp) : msb(fp)];
					//if constexpr (TRACE) --T[SAFETY_PAWN_SHIELD + source32[relative_square(US, US == WHITE ? lsb(fp) : msb(fp))]][~US];
				}
				else
				{
					ei.weight[~US] += *shield_table[0][f];
					if constexpr (TRACE) ++T[SAFETY_PAWN_SHIELD + source32[f]][~US];
				}
			}
			
			score += *major_table[KING][ei.ksq[US]];
			
			if constexpr (TRACE) psqt_trace<US, KING>(ei.ksq[US]);

			return score;
		}

		template<bool TRACE>
		int evaluate(const Position& pos)
		{
			/*bool insufficient[COLOUR_N] = { 
				pos.is_insufficient<WHITE>(), 
				pos.is_insufficient<BLACK>() 
			};*/

			Colour us = pos.side;

			int game_phase = pos.get_game_phase();

			Score score = (us == WHITE) ? tempo_bonus : -tempo_bonus;
			
			if constexpr (TRACE) memset(T, 0, sizeof(T));
			if constexpr (TRACE) ++T[TEMPO][us];

			EvalInfo ei = EvalInfo(tt.probe_pawn(pos.bs->pkey));

			if (TRACE || ei.key != pos.bs->pkey)
			{
				ei.clear();
				ei.key = pos.bs->pkey;
				ei.ksq[WHITE] = lsb(pos.pc_bb[W_KING]);
				ei.ksq[BLACK] = lsb(pos.pc_bb[B_KING]);
				ei.score = evaluate_pawns<WHITE, TRACE>(pos, ei) - evaluate_pawns<BLACK, TRACE>(pos, ei);
				tt.new_pawn_entry(ei);
			}

			score += ei.score + evaluate_all<WHITE, TRACE>(pos, ei) - evaluate_all<BLACK, TRACE>(pos, ei);

			int eval = (score.mg * game_phase + score.eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;
			
			if (us == BLACK)
				eval = -eval;

			/*
			int scaling;

			if (game_phase == 2 && popcnt(pos.pc_bb[W_BISHOP]) == 1 && popcnt(pos.pc_bb[B_BISHOP]) == 1 
				&& bool(pos.pc_bb[W_BISHOP] & light_mask) == bool(pos.pc_bb[B_BISHOP] & dark_mask))
				scaling = opposite_bishops_scaling;
			else
				scaling = MAX_SCALING;

			if (scaling < MAX_SCALING)
				eval = eval * scaling / MAX_SCALING;
			*/

			return eval;//insufficient[us] ? min(DRAW_SCORE, eval) : insufficient[them] ? max(DRAW_SCORE, eval) : eval;
		}
		
		// explicit template instantiations
		template int evaluate<true>(const Position& pos);
		template int evaluate<false>(const Position& pos);

	} // namespace Eval

} // namespace Clovis
