#pragma once

#include "position.h"
#include "tt.h"

namespace clovis::eval {

	struct EvalInfo : transposition::PTEntry {
		constexpr EvalInfo() = default;
		explicit EvalInfo(const transposition::PTEntry& pte) : transposition::PTEntry(pte) {}

		std::array<int, COLOUR_N> n_att{ 0,0 };
	};

	constexpr Score pawn_source[] = {
		{0, 0}, {0, 0}, {0, 0}, {0, 0},
		{184, 245}, {183, 248}, {159, 236}, {196, 203},
		{71, 97}, {89, 96}, {108, 89}, {111, 44},
		{65, 85}, {79, 81}, {90, 75}, {102, 74},
		{59, 77}, {67, 80}, {82, 75}, {99, 70},
		{63, 71}, {72, 72}, {78, 76}, {82, 80},
		{58, 74}, {78, 75}, {73, 83}, {70, 85},
		{0, 0}, {0, 0}, {0, 0}, {0, 0},
	};

	constexpr Score knight_source[] = {
		{249, 199}, {295, 210}, {286, 235}, {302, 233},
		{307, 213}, {299, 235}, {315, 239}, {321, 241},
		{301, 223}, {317, 236}, {319, 243}, {328, 253},
		{306, 236}, {310, 245}, {325, 251}, {322, 259},
	};

	constexpr Score bishop_source[] = {
		{304, 230}, {336, 223}, {316, 236}, {323, 236},
		{303, 236}, {333, 221}, {333, 235}, {329, 239},
		{314, 239}, {332, 238}, {335, 240}, {332, 244},
		{308, 241}, {312, 241}, {318, 243}, {329, 239},
	};

	constexpr Score rook_source[] = {
		{443, 435}, {440, 437}, {444, 438}, {445, 436},
		{419, 440}, {442, 434}, {452, 430}, {452, 430},
		{435, 438}, {443, 438}, {447, 435}, {452, 435},
		{429, 446}, {448, 439}, {448, 439}, {456, 435},
	};

	constexpr Score queen_source[] = {
		{874, 815}, {840, 847}, {836, 858}, {869, 842},
		{879, 808}, {851, 841}, {872, 845}, {840, 867},
		{899, 807}, {895, 822}, {888, 832}, {877, 850},
		{883, 832}, {877, 840}, {875, 840}, {875, 843},
		{895, 814}, {891, 833}, {891, 821}, {889, 841},
		{891, 817}, {903, 813}, {896, 828}, {895, 835},
		{893, 793}, {908, 785}, {911, 800}, {910, 807},
		{897, 788}, {892, 788}, {892, 791}, {905, 787},
	};

	constexpr Score king_source[] = {
		{76, 14}, {105, 36}, {72, 58}, {69, 59},
		{88, 44}, {107, 55}, {82, 72}, {53, 85},
		{45, 63}, {88, 68}, {68, 85}, {62, 94},
		{37, 63}, {70, 79}, {70, 92}, {41, 101},
	};

	constexpr Score passed_pawn[] = {
		{0, 0}, {0, 0}, {0, 0}, {0, 0},
		{0, 0}, {0, 0}, {0, 0}, {0, 0},
		{40, 112}, {29, 111}, {41, 85}, {29, 108},
		{30, 61}, {23, 63}, {11, 49}, {5, 35},
		{12, 39}, {8, 40}, {0, 23}, {0, 20},
		{0, 11}, {0, 20}, {0, 7}, {0, 0},
		{2, 15}, {0, 13}, {0, 2}, {0, 3},
		{0, 0}, {0, 0}, {0, 0}, {0, 0}
	};

	constexpr Score candidate_passer[] = {
		{0, 0}, {0, 5}, {0, 9}, {7, 24}, {20, 49}, {25, 69}, {0, 0}, {0, 0}
	};

	constexpr Score quiet_mobility[] = {
		{0, 0}, {0, 0}, {6, 1}, {4, 4}, {3, 3}, {1, 3}, {0, 0}
	};

	constexpr Score capture_mobility[] = {
		{0, 0}, {0, 0}, {8, 21}, {13, 20}, {10, 23}, {1, 16}, {0, 0}
	};

	constexpr Score double_pawn_penalty{ 1, 9 };
	constexpr Score isolated_pawn_penalty{ 12, 6 };
	constexpr Score bishop_pair_bonus{ 23, 43 };
	constexpr Score rook_open_file_bonus{ 18, 2 };
	constexpr Score rook_semi_open_file_bonus{ 0, 0 };
	constexpr Score rook_closed_file_penalty{ 14, 4 };
	constexpr Score tempo_bonus{ 23, 15 };
	constexpr Score king_open_penalty{ 33, 10 };
	constexpr Score king_adjacent_open_penalty{ 4, 10 };
	constexpr Score knight_outpost_bonus{ 37, 11 };
	constexpr Score bishop_outpost_bonus{ 35, 0 };
	constexpr Score weak_queen_penalty{ 31, 5 };
	constexpr Score rook_on_our_passer_file{ 4, 8 };
	constexpr Score rook_on_their_passer_file{ 5, 29 };
	constexpr Score tall_pawn_penalty{ 10, 24 };
	constexpr Score fianchetto_bonus{ 17, 11 };
	constexpr Score rook_on_seventh{ 0, 23 };

	constexpr short pawn_shield[] = {
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		32, 34, 30, 0,
	};

	constexpr short inner_ring_attack[] = {
		0, 19, 18, 26, 23, 21, 0,
	};

	constexpr short outer_ring_attack[] = {
		0, 0, 28, 14, 10, 19, 0,
	};

	constexpr short attack_factor = 59;
	constexpr short virtual_mobility = 14;

	constexpr auto source32 = [] {
		std::array<Square, SQ_N> arr{};

		for (Square sq = SQ_ZERO; sq < 32; ++sq) {
			const int r = sq / 4, f = sq & 0x3;
			arr[make_square(f, r ^ 7)] = arr[make_square(f ^ 7, r ^ 7)] = sq;
		}

		return arr;
	}();

	constexpr auto source16 = [] {
		std::array<Square, SQ_N> arr = source32;

		for (Square sq = SQ_ZERO; sq < 16; ++sq) {
			const int r = sq / 4, f = sq & 0x3;
			arr[make_square(f, r ^ 7) ^ 56] = arr[make_square(f ^ 7, r ^ 7) ^ 56] = sq;
		}

		return arr;
	}();

	constexpr auto source10 = [] {
		std::array<Square, SQ_N> arr{};

		Square index = SQ_ZERO;

		for (Square sq = SQ_ZERO; sq < 16; ++sq) {
			if (const int r = sq / 4, f = sq & 0x3; r >= f) {
				arr[make_square(f, r)] = arr[make_square(f, r ^ 7)] = arr[make_square(f ^ 7, r)] = arr[make_square(f ^ 7, r ^ 7)] = index;
				arr[make_square(r, f)] = arr[make_square(r, f ^ 7)] = arr[make_square(r ^ 7, f)] = arr[make_square(r ^ 7, f ^ 7)] = index;
				++index;
			}
		}

		return arr;
	}();

	constexpr std::array<const Score*, 7> piece_type_source =
	{ nullptr, pawn_source, knight_source, bishop_source, rook_source, queen_source, king_source };

	constexpr auto piece_table = [] {
		std::array<std::array<Score, SQ_N>, 15> arr{};

		for (const auto col : { WHITE, BLACK }) {
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) {
				for (const auto pt : { PAWN, QUEEN })
					arr[make_piece(pt, col)][sq] = piece_type_source[pt][source32[relative_square(col, sq)]];
				for (const auto pt : { KNIGHT, BISHOP, ROOK, KING })
					arr[make_piece(pt, col)][sq] = piece_type_source[pt][source16[sq]];
				//for (const auto pt : {})
				//    arr[make_piece(pt, col)][sq] = piece_type_source[pt][source10[sq]];
			}
		}

		return arr;
	}();

	constexpr auto passed_table = [] {
		std::array<std::array<Score, SQ_N>, COLOUR_N> arr{};

		for (const auto col : { WHITE, BLACK })
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[col][sq] = passed_pawn[source32[relative_square(col, sq)]];

		return arr;
	}();

	constexpr auto shield_table = [] {
		std::array<std::array<short, SQ_N>, COLOUR_N> arr{};

		for (const auto col : { WHITE, BLACK })
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[col][sq] = pawn_shield[source32[relative_square(col, sq)]];

		return arr;
	}();
	
	constexpr auto isolated_masks = [] {
		std::array<Bitboard, SQ_N> arr{};

		for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
			arr[sq] = (file_of(sq) != FILE_A ? bitboards::file_masks[sq + WEST] : 0ULL)
			        | (file_of(sq) != FILE_H ? bitboards::file_masks[sq + EAST] : 0ULL);

		return arr;
	}();
	
	constexpr auto passed_masks = [] {
		std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr{};

		for (const auto c : { WHITE, BLACK })
			for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
				for (Square s2 = s1; is_valid(s2); s2 += pawn_push(c))
					arr[c][s1] |= bitboards::pawn_attacks[c][s2] | s2;

		return arr;
	}();
			
	constexpr auto outpost_pawn_masks = [] {
		std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr = passed_masks;

		for (const auto c : { WHITE, BLACK })
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[c][sq] &= ~bitboards::file_masks[sq];

		return arr;
	}();

	constexpr auto rook_on_passer_masks = [] {
		std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr = passed_masks;

		for (const auto c : { WHITE, BLACK })
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[c][sq] ^= outpost_pawn_masks[c][sq];

		return arr;
	}();

	constexpr auto inner_ring = [] {
		std::array<Bitboard, SQ_N> arr{};

		for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
			arr[sq] = bitboards::get_attacks<KING>(sq) | sq;

		return arr;
	}();
	
	constexpr auto outer_ring = [] {
		std::array<Bitboard, SQ_N> arr{};

		for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1) {
			Bitboard bb = bitboards::get_attacks<KING>(s1);
			while (bb) 
				arr[s1] |= bitboards::get_attacks<KING>(bitboards::pop_lsb(bb));
			arr[s1] &= ~(bitboards::get_attacks<KING>(s1) | s1);
		}

		return arr;
	}();

	constexpr Bitboard outpost_masks[COLOUR_N] = { 
		bitboards::rank_masks[A4] | bitboards::rank_masks[A5] | bitboards::rank_masks[A6],
		bitboards::rank_masks[A3] | bitboards::rank_masks[A4] | bitboards::rank_masks[A5] 
	};

	constexpr Bitboard light_mask = 0x55aa55aa55aa55aaULL;

	constexpr Bitboard dark_mask = 0xaa55aa55aa55aa55ULL;

	constexpr Bitboard fianchetto_bishop_mask[COLOUR_N] = { B2 | G2, B7 | G7 };

	constexpr Bitboard center_mask[COLOUR_N] = { D5 | E5, D4 | E4 };
	
	extern std::array<std::array<int, PHASE_N>, TI_MISC> T;

	template<bool TRACE> int evaluate(const Position& pos);

	// explicit template instantiations
	template int evaluate<true>(const Position& pos);
	template int evaluate<false>(const Position& pos);

} // namespace clovis::eval
