#pragma once

#include "position.h"

namespace Clovis {

	class Position;

	enum GamePhase : int {
		MG, EG,
		PHASE_N = 2
	};

	struct Score {
	public:
		Score() : mg(0), eg(0) {}
		Score(int mg, int eg) : mg(mg), eg(eg) {}
		void operator+=(const Score& rhs) { 
			this->mg += rhs.mg; 
			this->eg += rhs.eg;
		}
		void operator-=(const Score& rhs) {
			this->mg -= rhs.mg;
			this->eg -= rhs.eg;
		}
		bool operator==(const Score& rhs) {
			return this->mg == rhs.mg && this->eg == rhs.eg;
		}
		int get_score(int game_phase, Colour side) const { 
			return (side == WHITE) 
				? (mg * game_phase + eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE
				:-(mg * game_phase + eg * (MAX_GAMEPHASE - game_phase)) / MAX_GAMEPHASE;
		}
		short mg;
		short eg;
	};

	struct KingZone {
		KingZone() : outer_ring(0ULL), inner_ring(0ULL) { ; }
		KingZone(Bitboard outer_ring, Bitboard inner_ring) : outer_ring(outer_ring), inner_ring(inner_ring) { ; }
		Bitboard outer_ring;
		Bitboard inner_ring;
	};

	inline Score operator+(Score s1, Score s2) { return Score(s1.mg + s2.mg, s1.eg + s2.eg); }
	inline Score operator+(Score s1, int i) { return Score(s1.mg + i, s1.eg + i); }
	inline Score operator-(Score s1, Score s2) { return Score(s1.mg - s2.mg, s1.eg - s2.eg); }
	inline Score operator-(Score s1, int i) { return Score(s1.mg - i, s1.eg - i); }
	inline Score operator*(Score s1, int i) { return Score(s1.mg * i, s1.eg * i); }
	inline Score operator*(Score s1, Score s2) { return Score(s1.mg * s2.mg, s1.eg * s2.eg); }
	inline Score operator/(Score s1, int i) { return Score(s1.mg / i, s1.eg / i); }
	inline Score operator/(Score s1, Score s2) { return Score(s1.mg / s2.mg, s1.eg / s2.eg); }

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
		extern Score outer_ring_attack[7];
		extern Score inner_ring_attack[7];
		extern Score king_safety_reduction_factor;

		void init_eval();
		void init_values();
		void init_masks();

		void test_eval();

		Bitboard set_file_rank_mask(File file_number, Rank rank_number);
		Score evaluate(const Position& pos);
		Score evaluate_pawns(const Position& pos);

	} // namespace Eval

} // namespace Clovis