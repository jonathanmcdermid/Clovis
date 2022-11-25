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
		extern Score king_safety_reduction_factor;

		void init_eval();
		void init_values();
		void init_masks();

		void test_eval();

		Bitboard set_file_rank_mask(File file_number, Rank rank_number);
		Score evaluate_all(const Position& pos);
		Score evaluate_pawns(const Position& pos);

		template<bool use_tt>
		int evaluate(const Position& pos)
		{
			Colour us = pos.stm();
			Colour them = other_side(us);

			bool insufficient[COLOUR_N] = { pos.is_insufficient<WHITE>(), pos.is_insufficient<BLACK>() };

			assert(!(insufficient[WHITE] && insufficient[BLACK]));

			int game_phase = pos.get_game_phase();

			Score score = evaluate_all(pos);

			if (use_tt)
			{
				PTEntry* pte = tt.probe_pawn(pos.get_pawn_key());

				if (!pte)
				{
					tt.new_pawn_entry(pos.get_pawn_key(), evaluate_pawns(pos));
					pte = tt.probe_pawn(pos.get_pawn_key());
					assert(pte);
				}

				score += pte->s;
			}
			else score += evaluate_pawns(pos);

			int eval = (score.mg * game_phase + score.eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;
			if (us == BLACK)
				eval = -eval;

			// we dont have to worry about both sides being out of material
			if (insufficient[us])
				return min(DRAW_SCORE, eval);
			if (insufficient[them])
				return max(DRAW_SCORE, eval);
			return eval;

		}

	} // namespace Eval

} // namespace Clovis