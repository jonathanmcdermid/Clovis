#pragma once

#include "position.h"

namespace Clovis {

	class Position;

	struct Score {
	public:
		Score() : mg(0), eg(0) {}
		Score(int m, int e) : mg(m), eg(e) {}
		void operator+=(const Score& rhs) { 
			this->mg += rhs.mg; 
			this->eg += rhs.eg;
		}
		void operator-=(const Score& rhs) {
			this->mg -= rhs.mg;
			this->eg -= rhs.eg;
		}
		int get_score(int game_phase, Colour side) const { 
			return (side == WHITE) 
				? (mg * game_phase + eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE
				:-(mg * game_phase + eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;
		}
		int mg;
		int eg;
	};

	namespace Eval {

		extern Score piece_value[7];

		void init_eval();
		void init_masks();
		Bitboard set_file_rank_mask(File file_number, Rank rank_number);
		int evaluate(const Position& pos);
		Score evaluate_pawns(const Position& pos);

	} // namespace Eval

} // namespace Clovis