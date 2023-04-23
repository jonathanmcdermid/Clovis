#pragma once

#include <array>

#include "position.h"
#include "tt.h"

namespace Clovis {

	struct Position;

	namespace Eval {

		struct EvalInfo : public PTEntry {
			constexpr EvalInfo() : PTEntry(), n_att{ 0,0 } {}
			EvalInfo(const PTEntry& pte) : PTEntry(pte), n_att{0, 0} {}
			short n_att[COLOUR_N];
		};
	
#define S(mg, eg) Score(mg, eg)

        	constexpr Score pawn_source[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(184, 246), S(184, 249), S(158, 237), S(195, 204),
			S(71, 97), S(88, 97), S(108, 89), S(111, 45),
			S(64, 86), S(78, 81), S(89, 76), S(102, 74),
			S(58, 78), S(66, 81), S(81, 75), S(98, 71),
			S(62, 71), S(71, 73), S(76, 77), S(81, 81),
			S(57, 74), S(76, 76), S(72, 84), S(68, 85),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		constexpr Score knight_source[] = {
			S(249, 200), S(295, 211), S(285, 236), S(301, 233),
			S(307, 213), S(298, 236), S(315, 239), S(321, 242),
			S(301, 223), S(317, 237), S(319, 244), S(327, 253),
			S(306, 237), S(310, 246), S(325, 252), S(321, 260),
		};

		constexpr Score bishop_source[] = {
			S(303, 231), S(336, 224), S(315, 237), S(322, 237),
			S(304, 237), S(332, 221), S(333, 235), S(329, 240),
			S(314, 239), S(332, 239), S(335, 240), S(332, 244),
			S(308, 242), S(311, 242), S(318, 244), S(329, 240),
		};

		constexpr Score rook_source[] = {
			S(442, 437), S(439, 439), S(443, 440), S(444, 437),
			S(418, 442), S(441, 435), S(451, 432), S(451, 431),
			S(434, 439), S(443, 439), S(446, 436), S(451, 436),
			S(429, 447), S(447, 440), S(448, 441), S(455, 437),
		};

		constexpr Score queen_source[] = {
			S(879, 810), S(874, 816), S(875, 818), S(890, 809),
			S(876, 811), S(875, 823), S(896, 818), S(896, 823),
			S(883, 824), S(890, 828), S(884, 840), S(882, 848),
			S(880, 834), S(875, 848), S(877, 838), S(876, 850),
		};

		constexpr Score king_source[] = {
			S(77, 13), S(105, 36), S(72, 58), S(69, 59),
			S(88, 44), S(107, 55), S(82, 72), S(53, 85),
			S(44, 63), S(88, 68), S(67, 85), S(61, 94),
			S(36, 63), S(69, 79), S(70, 92), S(39, 101),
		};

		constexpr Score passed_pawn[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(40, 112), S(29, 111), S(41, 85), S(29, 108),
			S(30, 61), S(23, 63), S(11, 49), S(5, 35),
			S(13, 39), S(8, 40), S(0, 23), S(0, 20),
			S(0, 11), S(0, 20), S(0, 7), S(0, 0),
			S(2, 15), S(0, 13), S(0, 2), S(0, 2),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		constexpr Score candidate_passer[] = {
			S(0, 0), S(0, 5), S(0, 9), S(7, 24), S(21, 49), S(26, 69), S(0, 0), S(0, 0),
		};

		constexpr Score quiet_mobility[] = {
			S(0, 0), S(0, 0), S(6, 1), S(4, 4), S(3, 3), S(1, 3), S(0, 0),
		};

		constexpr Score capture_mobility[] = {
			S(0, 0), S(0, 0), S(8, 21), S(13, 20), S(10, 23), S(0, 16), S(0, 0),
		};

		constexpr Score double_pawn_penalty = S(1, 9);
		constexpr Score isolated_pawn_penalty = S(12, 6);
		constexpr Score bishop_pair_bonus = S(22, 43);
		constexpr Score rook_open_file_bonus = S(18, 2);
		constexpr Score rook_semi_open_file_bonus = S(0, 0);
		constexpr Score rook_closed_file_penalty = S(14, 4);
		constexpr Score tempo_bonus = S(23, 15);
		constexpr Score king_open_penalty = S(33, 10);
		constexpr Score king_adjacent_open_penalty = S(4, 10);
		constexpr Score knight_outpost_bonus = S(37, 11);
		constexpr Score bishop_outpost_bonus = S(35, 0);
		constexpr Score weak_queen_penalty = S(32, 4);
		constexpr Score rook_on_our_passer_file = S(4, 8);
		constexpr Score rook_on_their_passer_file = S(5, 29);
		constexpr Score tall_pawn_penalty = S(10, 24);
		constexpr Score fianchetto_bonus = S(17, 11);
		constexpr Score rook_on_seventh = S(0, 23);

		constexpr short pawn_shield[] = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			32, 35, 30, 0,
		};

		constexpr short inner_ring_attack[] = {
			0, 20, 18, 26, 23, 20, 0,
		};

		constexpr short outer_ring_attack[] = {
			0, 0, 28, 14, 10, 18, 0,
		};

		constexpr short attack_factor = 60;
		constexpr short virtual_mobility = 15;

#undef S
		
		constexpr auto isolated_masks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) {
				if (file_of(sq) != FILE_A)
					arr[sq] |= Bitboards::file_masks[sq + WEST];
				if (file_of(sq) != FILE_H)
					arr[sq] |= Bitboards::file_masks[sq + EAST];
			}

			return arr;
		}();
		
		constexpr auto passed_masks = [] {
			std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr{};

			for (Colour c : { WHITE, BLACK })
				for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
					for (Square s2 = s1; is_valid(s2); s2 += pawn_push(c))
						arr[c][s1] |= Bitboards::pawn_attacks[c][s2] | s2;

			return arr;
		}();
				
		constexpr auto outpost_pawn_masks = [] {
			std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr{};

			for (Colour c : { WHITE, BLACK })
				for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
					for (Square s2 = s1; is_valid(s2); s2 += pawn_push(c))
						arr[c][s1] |= Bitboards::pawn_attacks[c][s2];

			return arr;
		}();

		constexpr auto rook_on_passer_masks = [] {
			std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr{};

			for (Colour c : { WHITE, BLACK })
				for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1)
					for (Square s2 = s1; is_valid(s2); s2 += pawn_push(c))
						arr[c][s1] |= s2;

			return arr;
		}();

		constexpr auto inner_ring = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[sq] = Bitboards::get_attacks<KING>(sq) | sq;

			return arr;
		}();
		
		constexpr auto outer_ring = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square s1 = SQ_ZERO; s1 < SQ_N; ++s1) {
				Bitboard bb = Bitboards::get_attacks<KING>(s1);
				while (bb) {
					Square s2 = pop_lsb(bb);
					arr[s1] |= Bitboards::get_attacks<KING>(s2);
				}
				arr[s1] &= ~(Bitboards::get_attacks<KING>(s1) | s1);
			}

			return arr;
		}();

		constexpr Bitboard outpost_masks[COLOUR_N] = { 
			Bitboards::rank_masks[A4] | Bitboards::rank_masks[A5] | Bitboards::rank_masks[A6],
			Bitboards::rank_masks[A3] | Bitboards::rank_masks[A4] | Bitboards::rank_masks[A5] 
		};

		constexpr Bitboard light_mask = 0x55aa55aa55aa55aaULL;

		constexpr Bitboard dark_mask = 0xaa55aa55aa55aa55ULL;

		constexpr Bitboard fianchetto_bishop_mask[COLOUR_N] = { B2 | G2, B7 | G7 };

		constexpr Bitboard center_mask[COLOUR_N] = { D5 | E5, D4 | E4 };
		
		constexpr auto source32 = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < 32; ++sq) {
				int r = sq / 4;
				int f = sq & 0x3;
				arr[make_square(f, r ^ 7)] = arr[make_square(f ^ 7, r ^ 7)] = sq;
			}

			return arr;
		}();
		
		constexpr auto source16 = [] {
			std::array<Bitboard, SQ_N> arr = source32;

			for (Square sq = SQ_ZERO; sq < 16; ++sq) {
				int r = sq / 4;
				int f = sq & 0x3;
				arr[make_square(f, r ^ 7) ^ 56] = arr[make_square(f ^ 7, r ^ 7) ^ 56] = sq;
			}

			return arr;
		}();
		
		constexpr auto source10 = [] {
			std::array<Bitboard, SQ_N> arr{};

			int index = 0;

			for (Square sq = SQ_ZERO; sq < 16; ++sq) {
				int r = sq / 4;
				int f = sq & 0x3;

				if (r >= f) {
					arr[make_square(f, r)] = arr[make_square(f, r ^ 7)] = arr[make_square(f ^ 7, r)] = arr[make_square(f ^ 7, r ^ 7)] = index;
					arr[make_square(r, f)] = arr[make_square(r, f ^ 7)] = arr[make_square(r ^ 7, f)] = arr[make_square(r ^ 7, f ^ 7)] = index;
					++index;
				}
			}

			return arr;
		}();
						

		void init_eval();
		template<bool TRACE> int evaluate(const Position& pos);
		
		extern int T[TI_MISC][PHASE_N];

	} // namespace Eval

} // namespace Clovis
