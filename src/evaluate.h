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
			S(184, 245), S(183, 248), S(159, 236), S(196, 203),
			S(71, 97), S(89, 96), S(108, 89), S(111, 44),
			S(65, 85), S(79, 81), S(90, 75), S(102, 74),
			S(59, 77), S(67, 80), S(82, 75), S(99, 70),
			S(63, 71), S(72, 72), S(78, 76), S(82, 80),
			S(58, 74), S(78, 75), S(73, 83), S(70, 85),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		constexpr Score knight_source[] = {
			S(249, 199), S(295, 210), S(286, 235), S(302, 233),
			S(307, 213), S(299, 235), S(315, 239), S(321, 241),
			S(301, 223), S(317, 236), S(319, 243), S(328, 253),
			S(306, 236), S(310, 245), S(325, 251), S(322, 259),
		};

		constexpr Score bishop_source[] = {
			S(304, 230), S(336, 223), S(316, 236), S(323, 236),
			S(303, 236), S(333, 221), S(333, 235), S(329, 239),
			S(314, 239), S(332, 238), S(335, 240), S(332, 244),
			S(308, 241), S(312, 241), S(318, 243), S(329, 239),
		};

		constexpr Score rook_source[] = {
			S(443, 435), S(440, 437), S(444, 438), S(445, 436),
			S(419, 440), S(442, 434), S(452, 430), S(452, 430),
			S(435, 438), S(443, 438), S(447, 435), S(452, 435),
			S(429, 446), S(448, 439), S(448, 439), S(456, 435),
		};

		constexpr Score queen_source[] = {
			S(874, 815), S(840, 847), S(836, 858), S(869, 842),
			S(879, 808), S(851, 841), S(872, 845), S(840, 867),
			S(899, 807), S(895, 822), S(888, 832), S(877, 850),
			S(883, 832), S(877, 840), S(875, 840), S(875, 843),
			S(895, 814), S(891, 833), S(891, 821), S(889, 841),
			S(891, 817), S(903, 813), S(896, 828), S(895, 835),
			S(893, 793), S(908, 785), S(911, 800), S(910, 807),
			S(897, 788), S(892, 788), S(892, 791), S(905, 787),
		};

		constexpr Score king_source[] = {
			S(76, 14), S(105, 36), S(72, 58), S(69, 59),
			S(88, 44), S(107, 55), S(82, 72), S(53, 85),
			S(45, 63), S(88, 68), S(68, 85), S(62, 94),
			S(37, 63), S(70, 79), S(70, 92), S(41, 101),
		};

		constexpr Score passed_pawn[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(40, 112), S(29, 111), S(41, 85), S(29, 108),
			S(30, 61), S(23, 63), S(11, 49), S(5, 35),
			S(12, 39), S(8, 40), S(0, 23), S(0, 20),
			S(0, 11), S(0, 20), S(0, 7), S(0, 0),
			S(2, 15), S(0, 13), S(0, 2), S(0, 3),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		constexpr Score candidate_passer[] = {
			S(0, 0), S(0, 5), S(0, 9), S(7, 24), S(20, 49), S(25, 69), S(0, 0), S(0, 0),
		};

		constexpr Score quiet_mobility[] = {
			S(0, 0), S(0, 0), S(6, 1), S(4, 4), S(3, 3), S(1, 3), S(0, 0),
		};

		constexpr Score capture_mobility[] = {
			S(0, 0), S(0, 0), S(8, 21), S(13, 20), S(10, 23), S(1, 16), S(0, 0),
		};

		constexpr Score double_pawn_penalty = S(1, 9);
		constexpr Score isolated_pawn_penalty = S(12, 6);
		constexpr Score bishop_pair_bonus = S(23, 43);
		constexpr Score rook_open_file_bonus = S(18, 2);
		constexpr Score rook_semi_open_file_bonus = S(0, 0);
		constexpr Score rook_closed_file_penalty = S(14, 4);
		constexpr Score tempo_bonus = S(23, 15);
		constexpr Score king_open_penalty = S(33, 10);
		constexpr Score king_adjacent_open_penalty = S(4, 10);
		constexpr Score knight_outpost_bonus = S(37, 11);
		constexpr Score bishop_outpost_bonus = S(35, 0);
		constexpr Score weak_queen_penalty = S(31, 5);
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

#undef S
		
		constexpr auto isolated_masks = [] {
			std::array<Bitboard, SQ_N> arr{};

			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				arr[sq] = (file_of(sq) != FILE_A ? Bitboards::file_masks[sq + WEST] : 0ULL)
				        | (file_of(sq) != FILE_H ? Bitboards::file_masks[sq + EAST] : 0ULL);

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
			std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr = passed_masks;

			for (Colour c : { WHITE, BLACK })
				for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
					arr[c][sq] &= ~Bitboards::file_masks[sq];

			return arr;
		}();

		constexpr auto rook_on_passer_masks = [] {
			std::array<std::array<Bitboard, SQ_N>, COLOUR_N> arr = passed_masks;

			for (Colour c : { WHITE, BLACK })
				for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
					arr[c][sq] ^= outpost_pawn_masks[c][sq];

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
