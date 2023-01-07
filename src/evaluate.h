#pragma once

#include "position.h"
#include "tt.h"

namespace Clovis {

	struct Position;

	namespace Eval {
		extern Score pawn_table[32];
		extern Score knight_table[32];
		extern Score bishop_table[32];
		extern Score rook_table[32];
		extern Score queen_table[32];
        extern Score king_table[32];
		extern Score double_pawn_penalty;
		extern Score isolated_pawn_penalty;
		extern Score passed_pawn_bonus[32];
		extern Score bishop_pair_bonus;
		extern Score rook_open_file_bonus;
		extern Score rook_semi_open_file_bonus;
		extern Score tempo_bonus;
		extern Score mobility[7];
		extern Score outer_ring_attack[7];
		extern Score inner_ring_attack[7];
		extern Score knight_outpost_bonus;
		extern Score bishop_outpost_bonus;
		extern Score king_full_open_penalty;
		extern Score king_semi_open_penalty;
		extern Score king_adjacent_full_open_penalty;
		extern Score king_adjacent_semi_open_penalty;

		extern Score* piece_table[7];
        extern Score* score_table[15][SQ_N];
		extern Score* passed_table[SQ_N];

        extern Bitboard file_masks[SQ_N];
        extern Bitboard rank_masks[SQ_N];
        extern Bitboard isolated_masks[SQ_N];
        extern Bitboard passed_masks[COLOUR_N][SQ_N];
        extern Bitboard outpost_masks[COLOUR_N];
        extern Bitboard outpost_pawn_masks[COLOUR_N][SQ_N];
		extern KingZone king_zones[SQ_N];

		void init_eval();
		void init_values();
		void init_masks();

		Bitboard set_file_rank_mask(File file_number, Rank rank_number);
		
		inline bool doubled_pawn(Bitboard bb, Square sq) {
            return popcnt(bb & file_masks[sq]) >= 2;
        }

        inline bool isolated_pawn(Bitboard bb, Square sq) {
            return !(bb & isolated_masks[sq]);
        }

        inline bool passed_pawn(Bitboard bb, Square sq, Colour side) {
            return !(bb & passed_masks[side][sq]);
        }

        inline bool outpost(Bitboard enemy_pawns, Bitboard friendly_pawns, Square sq, Colour side) {
            return (Bitboards::pawn_attacks[other_side(side)][sq] & friendly_pawns) 
				&& (outpost_masks[side] & sq) 
				&& !(enemy_pawns & outpost_pawn_masks[side][sq]);
        }

		template<Colour US, PieceType PT, bool SAFE>
		Score evaluate_majors(const Position& pos, PTEntry& pte)
		{
			constexpr Colour THEM = other_side(US);

			constexpr Piece OUR_PAWN	= make_piece(PAWN, US);
			//constexpr Piece OUR_BISHOP	= make_piece(BISHOP, US);
			constexpr Piece OUR_ROOK	= make_piece(ROOK, US);
			constexpr Piece PIECE		= make_piece(PT, US);

			constexpr Piece THEIR_PAWN	= make_piece(PAWN, THEM);
			constexpr Piece THEIR_ROOK	= make_piece(ROOK, THEM);
			constexpr Piece THEIR_KING	= make_piece(KING, THEM);

			assert(PT >= KNIGHT && PT < KING);

			Score score;
			Square sq;
			Bitboard bb = pos.piece_bitboard[PIECE];

			Bitboard transparent_occ =
				PT == BISHOP
				? pos.occ_bitboard[BOTH] ^ pos.piece_bitboard[THEIR_KING] ^ pos.piece_bitboard[W_QUEEN] ^ pos.piece_bitboard[B_QUEEN] ^ pos.piece_bitboard[THEIR_ROOK]
				: PT == ROOK 
				? pos.occ_bitboard[BOTH] ^ pos.piece_bitboard[THEIR_KING] ^ pos.piece_bitboard[W_QUEEN] ^ pos.piece_bitboard[B_QUEEN] ^ pos.piece_bitboard[OUR_ROOK]
				: PT == QUEEN 
				? pos.occ_bitboard[BOTH] ^ pos.piece_bitboard[THEIR_KING]
				: pos.occ_bitboard[BOTH];

			while (bb)
			{
				sq = pop_lsb(bb);
				score += *score_table[PIECE][sq];
				Bitboard attacks = Bitboards::get_attacks<PT>(transparent_occ, sq) & ~pte.attacks[THEM];

				score += mobility[PT] * (popcnt(attacks & ~pos.occ_bitboard[US]));

				if (PT == KNIGHT)
				{
					if (outpost(pos.piece_bitboard[THEIR_PAWN], pos.piece_bitboard[OUR_PAWN], sq, US))
						score += knight_outpost_bonus;
				}
				else if (PT == BISHOP)
				{
					if (outpost(pos.piece_bitboard[THEIR_PAWN], pos.piece_bitboard[OUR_PAWN], sq, US))
						score += bishop_outpost_bonus;
					if (bb)
						score += bishop_pair_bonus;
				}
				else if (PT == ROOK)
				{
					if (!(file_masks[sq] & (pos.piece_bitboard[W_PAWN] | pos.piece_bitboard[B_PAWN])))
						score += rook_open_file_bonus;
					else if (!(file_masks[sq] & pos.piece_bitboard[OUR_PAWN]))
						score += rook_semi_open_file_bonus;
				}

				if (!SAFE)
				{
					Bitboard or_att_bb = attacks & pte.zone[THEM].outer_ring;
					Bitboard ir_att_bb = attacks & pte.zone[THEM].inner_ring;

					if (or_att_bb || ir_att_bb)
					{
						pte.weight[US] += inner_ring_attack[PT] * popcnt(ir_att_bb) + outer_ring_attack[PT] * popcnt(or_att_bb);
						++pte.n_att[US];
					}
				}
			}

			return score;
		}

		template<Colour US>
		Score evaluate_all(const Position& pos, PTEntry& pte)
        {
			constexpr Piece OUR_QUEEN = make_piece(QUEEN, US);
        	
			Score score;

			if (pos.piece_bitboard[OUR_QUEEN])
			{
				score += evaluate_majors<US, KNIGHT,false>(pos, pte);
				score += evaluate_majors<US, BISHOP,false>(pos, pte);
				score += evaluate_majors<US, ROOK,	false>(pos, pte);
				score += evaluate_majors<US, QUEEN, false>(pos, pte);

				// we dont count kings or pawns in n_att so the max should be 7
				assert(pte.n_att[US] < 10);
				score += pte.weight[US] * pte.weight[US] / (10 - pte.n_att[US]);
			}
			else
			{
				score += evaluate_majors<US, KNIGHT,true>(pos, pte);
				score += evaluate_majors<US, BISHOP,true>(pos, pte);
				score += evaluate_majors<US, ROOK,	true>(pos, pte);
				score += evaluate_majors<US, QUEEN, true>(pos, pte);
			}

			return score;
        }

		template<Colour US>
		Score evaluate_pawns(const Position& pos, PTEntry& pte)
		{
			constexpr Colour THEM = other_side(US);

			constexpr Piece OUR_PAWN	= make_piece(PAWN, US);
			constexpr Piece OUR_KING	= make_piece(KING, US);
			constexpr Piece THEIR_PAWN	= make_piece(PAWN, THEM);

			Square king_sq = lsb(pos.piece_bitboard[OUR_KING]);

			Score score;

			Bitboard bb = pos.piece_bitboard[OUR_PAWN];

			while (bb)
			{
				Square sq = pop_lsb(bb);

				score += *score_table[OUR_PAWN][sq];

				if (doubled_pawn(pos.piece_bitboard[OUR_PAWN], sq))
					score -= double_pawn_penalty;

				if (isolated_pawn(pos.piece_bitboard[OUR_PAWN], sq))
					score -= isolated_pawn_penalty;

				if (passed_pawn(pos.piece_bitboard[THEIR_PAWN], sq, US))
					score += *passed_table[relative_square(US, sq)];

				Bitboard attacks = Bitboards::pawn_attacks[US][sq];

				Bitboard or_att_bb = attacks & pte.zone[THEM].outer_ring;
				Bitboard ir_att_bb = attacks & pte.zone[THEM].inner_ring;

				if (or_att_bb || ir_att_bb)
				{
					pte.weight[US] += inner_ring_attack[PAWN] * popcnt(ir_att_bb) + outer_ring_attack[PAWN] * popcnt(or_att_bb);
				}

				pte.attacks[US] |= attacks;
			}

			if (!(file_masks[king_sq] & pos.piece_bitboard[OUR_PAWN]))
			{
				score -= (file_masks[king_sq] & pos.piece_bitboard[THEIR_PAWN])
					? king_semi_open_penalty
					: king_full_open_penalty;
			}
			if (file_of(king_sq) != FILE_A && !(file_masks[king_sq + WEST] & pos.piece_bitboard[OUR_PAWN]))
			{
				score -= (file_masks[king_sq + WEST] & pos.piece_bitboard[THEIR_PAWN])
					? king_adjacent_semi_open_penalty
					: king_adjacent_full_open_penalty;
			}
			if (file_of(king_sq) != FILE_H && !(file_masks[king_sq + EAST] & pos.piece_bitboard[OUR_PAWN]))
			{
				score -= (file_masks[king_sq + EAST] & pos.piece_bitboard[THEIR_PAWN])
					? king_adjacent_semi_open_penalty
					: king_adjacent_full_open_penalty;
			}

			score += *score_table[OUR_KING][king_sq];

			return score;
		}

		template<bool USE_TT>
		int evaluate(const Position& pos)
		{
			bool insufficient[COLOUR_N] = { pos.is_insufficient<WHITE>(), pos.is_insufficient<BLACK>() };
			
			if (insufficient[WHITE] && insufficient[BLACK])
				return DRAW_SCORE;

			Colour us = pos.side;
			Colour them = other_side(us);

			int game_phase = pos.get_game_phase();

			Score score = (us == WHITE) ? tempo_bonus : -tempo_bonus;

			PTEntry pte = tt.probe_pawn(pos.bs->pkey);

			if (!USE_TT || pte.key != pos.bs->pkey)
			{
				pte.clear();
				pte.key = pos.bs->pkey;
				pte.zone[WHITE] = king_zones[lsb(pos.piece_bitboard[W_KING])];
				pte.zone[BLACK] = king_zones[lsb(pos.piece_bitboard[B_KING])];
				pte.score = evaluate_pawns<WHITE>(pos, pte) - evaluate_pawns<BLACK>(pos, pte);
				tt.new_pawn_entry(pte);
			}

			score += pte.score + evaluate_all<WHITE>(pos, pte) - evaluate_all<BLACK>(pos, pte);

			int eval = (score.mg * game_phase + score.eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;
			if (us == BLACK)
				eval = -eval;

			if (insufficient[us])
				return min(DRAW_SCORE, eval);
			if (insufficient[them])
				return max(DRAW_SCORE, eval);

			return eval;

		}

	} // namespace Eval

} // namespace Clovis
