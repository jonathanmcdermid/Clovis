#pragma once

#include "position.h"
#include "tt.h"

namespace Clovis {

	class Position;

	struct KingZone {
		KingZone() : outer_ring(0ULL), inner_ring(0ULL) { ; }
		KingZone(Bitboard outer_ring, Bitboard inner_ring) : outer_ring(outer_ring), inner_ring(inner_ring) { ; }
		Bitboard outer_ring;
		Bitboard inner_ring;
	};

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
		extern Score outpost_bonus[2];
		extern Score king_safety_reduction_factor;
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
            return (Bitboards::pawn_attacks[other_side(side)][sq] & friendly_pawns) && (outpost_masks[side] & sq) && !(enemy_pawns & outpost_pawn_masks[side][sq]);
        }

		template<Colour US, PieceType P>
		Score evaluate_majors(const Position& pos, KingZone enemy_king_zone, int& n_king_attackers, Score& king_attack_weight)
		{
			constexpr Colour THEM = other_side(US);

			constexpr Piece OUR_PAWN	= make_piece(PAWN, US);
			constexpr Piece OUR_BISHOP	= make_piece(BISHOP, US);
			constexpr Piece OUR_ROOK	= make_piece(ROOK, US);
			constexpr Piece PIECE		= make_piece(P, US);

			constexpr Piece THEIR_PAWN	= make_piece(PAWN, THEM);
			constexpr Piece THEIR_ROOK	= make_piece(ROOK, THEM);

			Score score;
			Square sq;
			Bitboard bb = pos.piece_bitboard[PIECE];

			Bitboard transparent_occ =
				(P == KNIGHT || P == QUEEN)
				? pos.occ_bitboard[BOTH]
				: P == BISHOP
				? pos.occ_bitboard[BOTH] & ~(pos.piece_bitboard[OUR_BISHOP] | pos.piece_bitboard[THEIR_ROOK] | pos.piece_bitboard[W_QUEEN] | pos.piece_bitboard[B_QUEEN])
				: pos.occ_bitboard[BOTH] & ~(pos.piece_bitboard[W_QUEEN] | pos.piece_bitboard[B_QUEEN] | pos.piece_bitboard[OUR_ROOK]);

			while (bb)
			{
				sq = pop_lsb(bb);
				score += *score_table[PIECE][sq];
				Bitboard attacks = Bitboards::get_attacks<P>(transparent_occ, sq);

				score += mobility[P] * (popcnt(attacks & ~pos.occ_bitboard[US]));

				if (P == ROOK)
				{
					if (!(file_masks[sq] & (pos.piece_bitboard[W_PAWN] | pos.piece_bitboard[B_PAWN])))
						score += rook_open_file_bonus;
					else if (!(file_masks[sq] & pos.piece_bitboard[OUR_PAWN]))
						score += rook_semi_open_file_bonus;
				}
				else if (P == KNIGHT || P == BISHOP)
				{
					if (outpost(pos.piece_bitboard[THEIR_PAWN], pos.piece_bitboard[OUR_PAWN], sq, US))
						score += outpost_bonus[P - KNIGHT];
					if (P == BISHOP && bb)
						score += bishop_pair_bonus;
				}

				Bitboard or_att_bb = attacks & enemy_king_zone.outer_ring;
				Bitboard ir_att_bb = attacks & enemy_king_zone.inner_ring;

				if (or_att_bb || ir_att_bb) {
					++n_king_attackers;
					king_attack_weight += inner_ring_attack[P] * popcnt(ir_att_bb) + outer_ring_attack[P] * popcnt(or_att_bb);
				}
			}

			return score;
		}

		template<Colour US>
		Score evaluate_all(const Position& pos)
        {
        	constexpr Colour THEM = other_side(US);

			constexpr Piece THEIR_KING	= make_piece(KING, THEM);
			constexpr Piece OUR_QUEEN	= make_piece(QUEEN, US);
        	
			Score score;

			int n_king_attackers = 0;
			Score king_attack_weight;
			KingZone enemy_king_zone = king_zones[lsb(pos.piece_bitboard[THEIR_KING])];

			score += evaluate_majors<US, KNIGHT>(pos, enemy_king_zone, n_king_attackers, king_attack_weight);
			score += evaluate_majors<US, BISHOP>(pos, enemy_king_zone, n_king_attackers, king_attack_weight);
			score += evaluate_majors<US, ROOK>	(pos, enemy_king_zone, n_king_attackers, king_attack_weight);
			score += evaluate_majors<US, QUEEN>	(pos, enemy_king_zone, n_king_attackers, king_attack_weight);

			if (n_king_attackers >= 2 && pos.piece_bitboard[OUR_QUEEN])
				score += (king_attack_weight * king_attack_weight) / (king_safety_reduction_factor + 1);

			return score;
        }

		template<Colour US>
		Score evaluate_pawns(const Position& pos)
		{
			constexpr Colour THEM = other_side(US);

			constexpr Piece OUR_KING	= make_piece(KING, US);
			constexpr Piece OUR_PAWN	= make_piece(PAWN, US);
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

			score += evaluate_all<WHITE>(pos) - evaluate_all<BLACK>(pos);

			if (USE_TT)
			{
				PTEntry* pte = tt.probe_pawn(pos.get_pawn_key());

				if (!pte)
				{
					tt.new_pawn_entry(pos.get_pawn_key(), evaluate_pawns<WHITE>(pos) - evaluate_pawns<BLACK>(pos));
					pte = tt.probe_pawn(pos.get_pawn_key());
					assert(pte);
				}

				score += pte->s;
			}
			else score += evaluate_pawns<WHITE>(pos) - evaluate_pawns<BLACK>(pos);

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
