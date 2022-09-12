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
		short mg;
		short eg;
	};

	inline Score operator+(Score s1, Score s2) {
		s1 += s2;
		return s1;
	}

	inline Score operator-(Score s1, Score s2) {
		s1 -= s2;
		return s1;
	}

	inline Score operator*(Score s1, int i) {
		return Score(s1.mg * i, s1.eg * i);
	}

	// evaluation function utilizes two primary types of parameters
	// type 1: signed
	// type 2: unsigned

	class EvalParam {
		EvalParam(short m, short d);
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
		extern Score mobility[7];

		void init_eval();
		void init_values();
		void init_masks();
		Bitboard set_file_rank_mask(File file_number, Rank rank_number);
		Score evaluate(const Position& pos);
		Score evaluate_pawns(const Position& pos);

	} // namespace Eval

} // namespace Clovis