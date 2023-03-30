#pragma once

#include "position.h"
#include "tt.h"

namespace Clovis {

	struct Position;

	namespace Eval {

		struct EvalInfo : public PTEntry {
			EvalInfo(PTEntry pte) : PTEntry(pte) {
				n_att[WHITE] = 0;
				n_att[BLACK] = 0;
			}
			void clear() { 
				PTEntry::clear();
				n_att[WHITE] = 0;
				n_att[BLACK] = 0;
			}
			short n_att[COLOUR_N] = {0};
		};
	
#define S(mg, eg) Score(mg, eg)

        	constexpr Score pawn_table[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(172, 253), S(194, 253), S(162, 239), S(201, 209),
			S(71, 99), S(92, 97), S(114, 87), S(113, 55),
			S(65, 86), S(79, 82), S(89, 78), S(102, 75),
			S(59, 77), S(67, 81), S(82, 75), S(100, 72),
			S(63, 71), S(73, 74), S(78, 77), S(82, 81),
			S(57, 74), S(80, 75), S(73, 85), S(70, 85),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		constexpr Score knight_table[] = {
			S(238, 206), S(301, 213), S(290, 238), S(309, 235),
			S(310, 217), S(305, 238), S(322, 242), S(327, 245),
			S(307, 227), S(324, 239), S(326, 246), S(332, 256),
			S(310, 239), S(316, 247), S(331, 255), S(327, 263),
		};

		constexpr Score bishop_table[] = {
			S(314, 231), S(341, 227), S(322, 240), S(325, 241),
			S(309, 240), S(340, 225), S(340, 238), S(336, 243),
			S(320, 244), S(340, 242), S(341, 243), S(340, 247),
			S(312, 245), S(319, 246), S(326, 246), S(337, 242),
		};

		constexpr Score rook_table[] = {
			S(458, 442), S(453, 444), S(453, 446), S(454, 445),
			S(435, 447), S(449, 443), S(463, 438), S(462, 437),
			S(448, 444), S(456, 445), S(458, 441), S(461, 442),
			S(442, 451), S(458, 444), S(457, 446), S(465, 442),
		};

		constexpr Score queen_table[] = {
			S(885, 826), S(850, 864), S(847, 869), S(883, 851),
			S(893, 820), S(858, 855), S(876, 861), S(840, 888),
			S(910, 819), S(899, 836), S(901, 841), S(877, 871),
			S(892, 847), S(887, 857), S(889, 851), S(885, 855),
			S(910, 820), S(905, 844), S(906, 833), S(903, 855),
			S(904, 830), S(921, 821), S(909, 841), S(911, 844),
			S(910, 804), S(924, 797), S(927, 810), S(927, 819),
			S(915, 792), S(902, 808), S(908, 798), S(921, 794),
		};

		constexpr Score king_table[] = {
			S(75, 14), S(104, 35), S(78, 55), S(60, 62),
			S(86, 44), S(107, 55), S(81, 72), S(53, 84),
			S(44, 62), S(88, 68), S(71, 84), S(65, 93),
			S(30, 64), S(67, 80), S(69, 92), S(41, 101),
		};

		constexpr Score passed_pawn[] = {
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
			S(37, 113), S(27, 112), S(37, 89), S(27, 99),
			S(35, 60), S(28, 62), S(15, 46), S(7, 34),
			S(10, 39), S(7, 41), S(0, 24), S(0, 20),
			S(0, 11), S(0, 20), S(0, 7), S(0, 0),
			S(2, 15), S(0, 13), S(0, 2), S(0, 0),
			S(0, 0), S(0, 0), S(0, 0), S(0, 0),
		};

		constexpr Score candidate_passer[] = {
			S(0, 0), S(0, 5), S(0, 9), S(7, 24), S(20, 49), S(41, 57), S(0, 0), S(0, 0),
		};

		constexpr Score quiet_mobility[] = {
			S(0, 0), S(0, 0), S(6, 1), S(4, 4), S(2, 3), S(1, 3), S(0, 0),
		};

		constexpr Score capture_mobility[] = {
			S(0, 0), S(0, 0), S(9, 21), S(12, 21), S(10, 23), S(1, 16), S(0, 0),
		};

		constexpr Score double_pawn_penalty = S(1, 10);
		constexpr Score isolated_pawn_penalty = S(12, 6);
		constexpr Score bishop_pair_bonus = S(25, 41);
		constexpr Score rook_open_file_bonus = S(20, 2);
		constexpr Score rook_semi_open_file_bonus = S(0, 0);
		constexpr Score rook_closed_file_penalty = S(14, 4);
		constexpr Score tempo_bonus = S(23, 16);
		constexpr Score king_full_open_penalty = S(34, 9);
		constexpr Score king_adjacent_full_open_penalty = S(4, 10);
		constexpr Score knight_outpost_bonus = S(36, 13);
		constexpr Score bishop_outpost_bonus = S(35, 0);
		constexpr Score weak_queen_penalty = S(33, 8);
		constexpr Score rook_on_our_passer_file = S(4, 8);
		constexpr Score rook_on_their_passer_file = S(1, 31);
		constexpr Score tall_pawn_penalty = S(11, 25);
		constexpr Score fianchetto_bonus = S(18, 11);
		constexpr Score rook_on_seventh = S(0, 22);
		constexpr Score trapped_rook_penalty = S(49, 2);

		constexpr short pawn_shield[] = {
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			33, 39, 32, 0,
		};

		constexpr short inner_ring_attack[] = {
			0, 19, 19, 27, 24, 22, 0,
		};

		constexpr short outer_ring_attack[] = {
			0, 0, 28, 14, 10, 20, 0,
		};

		constexpr short virtual_mobility = 14;
		constexpr short attack_factor = 58;

#undef S

		constexpr Bitboard file_masks[SQ_N] =  {
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL,  0x202020202020202ULL,  0x404040404040404ULL,  0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL,
		};
		
		constexpr Bitboard rank_masks[SQ_N] = {
			0xffULL,               0xffULL,               0xffULL,               0xffULL, 
			0xffULL,               0xffULL,               0xffULL,               0xffULL, 
			0xff00ULL,             0xff00ULL,             0xff00ULL,             0xff00ULL, 
			0xff00ULL,             0xff00ULL,             0xff00ULL,             0xff00ULL, 
			0xff0000ULL,           0xff0000ULL,           0xff0000ULL,           0xff0000ULL, 
			0xff0000ULL,           0xff0000ULL,           0xff0000ULL,           0xff0000ULL, 
			0xff000000ULL,         0xff000000ULL,         0xff000000ULL,         0xff000000ULL, 
			0xff000000ULL,         0xff000000ULL,         0xff000000ULL,         0xff000000ULL, 
			0xff00000000ULL,       0xff00000000ULL,       0xff00000000ULL,       0xff00000000ULL, 
			0xff00000000ULL,       0xff00000000ULL,       0xff00000000ULL,       0xff00000000ULL, 
			0xff0000000000ULL,     0xff0000000000ULL,     0xff0000000000ULL,     0xff0000000000ULL, 
			0xff0000000000ULL,     0xff0000000000ULL,     0xff0000000000ULL,     0xff0000000000ULL, 
			0xff000000000000ULL,   0xff000000000000ULL,   0xff000000000000ULL,   0xff000000000000ULL, 
			0xff000000000000ULL,   0xff000000000000ULL,   0xff000000000000ULL,   0xff000000000000ULL, 
			0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL, 
			0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL,
		};
		
		constexpr Bitboard isolated_masks[SQ_N] = {
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL,  0x505050505050505ULL,  0xa0a0a0a0a0a0a0aULL,  0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL,
		};
		
		constexpr Bitboard passed_masks[COLOUR_N][SQ_N] = {
			0x303030303030300ULL,  0x707070707070700ULL,  0xe0e0e0e0e0e0e00ULL,  0x1c1c1c1c1c1c1c00ULL, 
			0x3838383838383800ULL, 0x7070707070707000ULL, 0xe0e0e0e0e0e0e000ULL, 0xc0c0c0c0c0c0c000ULL, 
			0x303030303030000ULL,  0x707070707070000ULL,  0xe0e0e0e0e0e0000ULL,  0x1c1c1c1c1c1c0000ULL, 
			0x3838383838380000ULL, 0x7070707070700000ULL, 0xe0e0e0e0e0e00000ULL, 0xc0c0c0c0c0c00000ULL, 
			0x303030303000000ULL,  0x707070707000000ULL,  0xe0e0e0e0e000000ULL,  0x1c1c1c1c1c000000ULL, 
			0x3838383838000000ULL, 0x7070707070000000ULL, 0xe0e0e0e0e0000000ULL, 0xc0c0c0c0c0000000ULL, 
			0x303030300000000ULL,  0x707070700000000ULL,  0xe0e0e0e00000000ULL,  0x1c1c1c1c00000000ULL, 
			0x3838383800000000ULL, 0x7070707000000000ULL, 0xe0e0e0e000000000ULL, 0xc0c0c0c000000000ULL, 
			0x303030000000000ULL,  0x707070000000000ULL,  0xe0e0e0000000000ULL,  0x1c1c1c0000000000ULL, 
			0x3838380000000000ULL, 0x7070700000000000ULL, 0xe0e0e00000000000ULL, 0xc0c0c00000000000ULL, 
			0x303000000000000ULL,  0x707000000000000ULL,  0xe0e000000000000ULL,  0x1c1c000000000000ULL, 
			0x3838000000000000ULL, 0x7070000000000000ULL, 0xe0e0000000000000ULL, 0xc0c0000000000000ULL, 
			0x300000000000000ULL,  0x700000000000000ULL,  0xe00000000000000ULL,  0x1c00000000000000ULL, 
			0x3800000000000000ULL, 0x7000000000000000ULL, 0xe000000000000000ULL, 0xc000000000000000ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x3ULL,                0x7ULL,                0xeULL,                0x1cULL, 
			0x38ULL,               0x70ULL,               0xe0ULL,               0xc0ULL, 
			0x303ULL,              0x707ULL,              0xe0eULL,              0x1c1cULL, 
			0x3838ULL,             0x7070ULL,             0xe0e0ULL,             0xc0c0ULL, 
			0x30303ULL,            0x70707ULL,            0xe0e0eULL,            0x1c1c1cULL, 
			0x383838ULL,           0x707070ULL,           0xe0e0e0ULL,           0xc0c0c0ULL, 
			0x3030303ULL,          0x7070707ULL,          0xe0e0e0eULL,          0x1c1c1c1cULL, 
			0x38383838ULL,         0x70707070ULL,         0xe0e0e0e0ULL,         0xc0c0c0c0ULL, 
			0x303030303ULL,        0x707070707ULL,        0xe0e0e0e0eULL,        0x1c1c1c1c1cULL, 
			0x3838383838ULL,       0x7070707070ULL,       0xe0e0e0e0e0ULL,       0xc0c0c0c0c0ULL, 
			0x30303030303ULL,      0x70707070707ULL,      0xe0e0e0e0e0eULL,      0x1c1c1c1c1c1cULL, 
			0x383838383838ULL,     0x707070707070ULL,     0xe0e0e0e0e0e0ULL,     0xc0c0c0c0c0c0ULL, 
			0x3030303030303ULL,    0x7070707070707ULL,    0xe0e0e0e0e0e0eULL,    0x1c1c1c1c1c1c1cULL, 
			0x38383838383838ULL,   0x70707070707070ULL,   0xe0e0e0e0e0e0e0ULL,   0xc0c0c0c0c0c0c0ULL,
		};
		
		constexpr Bitboard outpost_masks[COLOUR_N] = { 0xffffff000000ULL, 0xffffff0000ULL };
		
		constexpr Bitboard outpost_pawn_masks[COLOUR_N][SQ_N] = {
			0x202020202020200ULL,  0x505050505050500ULL,  0xa0a0a0a0a0a0a00ULL,  0x1414141414141400ULL, 
			0x2828282828282800ULL, 0x5050505050505000ULL, 0xa0a0a0a0a0a0a000ULL, 0x4040404040404000ULL, 
			0x202020202020000ULL,  0x505050505050000ULL,  0xa0a0a0a0a0a0000ULL,  0x1414141414140000ULL, 
			0x2828282828280000ULL, 0x5050505050500000ULL, 0xa0a0a0a0a0a00000ULL, 0x4040404040400000ULL, 
			0x202020202000000ULL,  0x505050505000000ULL,  0xa0a0a0a0a000000ULL,  0x1414141414000000ULL, 
			0x2828282828000000ULL, 0x5050505050000000ULL, 0xa0a0a0a0a0000000ULL, 0x4040404040000000ULL, 
			0x202020200000000ULL,  0x505050500000000ULL,  0xa0a0a0a00000000ULL,  0x1414141400000000ULL, 
			0x2828282800000000ULL, 0x5050505000000000ULL, 0xa0a0a0a000000000ULL, 0x4040404000000000ULL, 
			0x202020000000000ULL,  0x505050000000000ULL,  0xa0a0a0000000000ULL,  0x1414140000000000ULL, 
			0x2828280000000000ULL, 0x5050500000000000ULL, 0xa0a0a00000000000ULL, 0x4040400000000000ULL, 
			0x202000000000000ULL,  0x505000000000000ULL,  0xa0a000000000000ULL,  0x1414000000000000ULL, 
			0x2828000000000000ULL, 0x5050000000000000ULL, 0xa0a0000000000000ULL, 0x4040000000000000ULL, 
			0x200000000000000ULL,  0x500000000000000ULL,  0xa00000000000000ULL,  0x1400000000000000ULL, 
			0x2800000000000000ULL, 0x5000000000000000ULL, 0xa000000000000000ULL, 0x4000000000000000ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x2ULL,                0x5ULL,                0xaULL,                0x14ULL, 
			0x28ULL,               0x50ULL,               0xa0ULL,               0x40ULL, 
			0x202ULL,              0x505ULL,              0xa0aULL,              0x1414ULL, 
			0x2828ULL,             0x5050ULL,             0xa0a0ULL,             0x4040ULL, 
			0x20202ULL,            0x50505ULL,            0xa0a0aULL,            0x141414ULL, 
			0x282828ULL,           0x505050ULL,           0xa0a0a0ULL,           0x404040ULL, 
			0x2020202ULL,          0x5050505ULL,          0xa0a0a0aULL,          0x14141414ULL, 
			0x28282828ULL,         0x50505050ULL,         0xa0a0a0a0ULL,         0x40404040ULL, 
			0x202020202ULL,        0x505050505ULL,        0xa0a0a0a0aULL,        0x1414141414ULL, 
			0x2828282828ULL,       0x5050505050ULL,       0xa0a0a0a0a0ULL,       0x4040404040ULL, 
			0x20202020202ULL,      0x50505050505ULL,      0xa0a0a0a0a0aULL,      0x141414141414ULL, 
			0x282828282828ULL,     0x505050505050ULL,     0xa0a0a0a0a0a0ULL,     0x404040404040ULL, 
			0x2020202020202ULL,    0x5050505050505ULL,    0xa0a0a0a0a0a0aULL,    0x14141414141414ULL, 
			0x28282828282828ULL,   0x50505050505050ULL,   0xa0a0a0a0a0a0a0ULL,   0x40404040404040ULL,
		};

		constexpr Bitboard rook_on_passer_masks[COLOUR_N][SQ_N] = {
			0x101010101010100ULL,  0x202020202020200ULL,  0x404040404040400ULL,  0x808080808080800ULL, 
			0x1010101010101000ULL, 0x2020202020202000ULL, 0x4040404040404000ULL, 0x8080808080808000ULL, 
			0x101010101010000ULL,  0x202020202020000ULL,  0x404040404040000ULL,  0x808080808080000ULL, 
			0x1010101010100000ULL, 0x2020202020200000ULL, 0x4040404040400000ULL, 0x8080808080800000ULL, 
			0x101010101000000ULL,  0x202020202000000ULL,  0x404040404000000ULL,  0x808080808000000ULL, 
			0x1010101010000000ULL, 0x2020202020000000ULL, 0x4040404040000000ULL, 0x8080808080000000ULL, 
			0x101010100000000ULL,  0x202020200000000ULL,  0x404040400000000ULL,  0x808080800000000ULL, 
			0x1010101000000000ULL, 0x2020202000000000ULL, 0x4040404000000000ULL, 0x8080808000000000ULL, 
			0x101010000000000ULL,  0x202020000000000ULL,  0x404040000000000ULL,  0x808080000000000ULL, 
			0x1010100000000000ULL, 0x2020200000000000ULL, 0x4040400000000000ULL, 0x8080800000000000ULL, 
			0x101000000000000ULL,  0x202000000000000ULL,  0x404000000000000ULL,  0x808000000000000ULL, 
			0x1010000000000000ULL, 0x2020000000000000ULL, 0x4040000000000000ULL, 0x8080000000000000ULL, 
			0x100000000000000ULL,  0x200000000000000ULL,  0x400000000000000ULL,  0x800000000000000ULL, 
			0x1000000000000000ULL, 0x2000000000000000ULL, 0x4000000000000000ULL, 0x8000000000000000ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x1ULL,                0x2ULL,                0x4ULL,                0x8ULL, 
			0x10ULL,               0x20ULL,               0x40ULL,               0x80ULL, 
			0x101ULL,              0x202ULL,              0x404ULL,              0x808ULL, 
			0x1010ULL,             0x2020ULL,             0x4040ULL,             0x8080ULL, 
			0x10101ULL,            0x20202ULL,            0x40404ULL,            0x80808ULL, 
			0x101010ULL,           0x202020ULL,           0x404040ULL,           0x808080ULL, 
			0x1010101ULL,          0x2020202ULL,          0x4040404ULL,          0x8080808ULL, 
			0x10101010ULL,         0x20202020ULL,         0x40404040ULL,         0x80808080ULL, 
			0x101010101ULL,        0x202020202ULL,        0x404040404ULL,        0x808080808ULL, 
			0x1010101010ULL,       0x2020202020ULL,       0x4040404040ULL,       0x8080808080ULL, 
			0x10101010101ULL,      0x20202020202ULL,      0x40404040404ULL,      0x80808080808ULL, 
			0x101010101010ULL,     0x202020202020ULL,     0x404040404040ULL,     0x808080808080ULL, 
			0x1010101010101ULL,    0x2020202020202ULL,    0x4040404040404ULL,    0x8080808080808ULL, 
			0x10101010101010ULL,   0x20202020202020ULL,   0x40404040404040ULL,   0x80808080808080ULL,
		};
		
		constexpr Bitboard light_mask = 0x55aa55aa55aa55aaULL;

		constexpr Bitboard dark_mask = 0xaa55aa55aa55aa55ULL;

		constexpr Bitboard fianchetto_bishop_mask[COLOUR_N] = { 0x4200ULL, 0x42000000000000ULL };

		constexpr Bitboard fianchetto_pawn_mask[COLOUR_N] = { 0x42a500ULL, 0xa5420000000000ULL };

		constexpr Bitboard center_mask = 0x1818000000ULL;

		constexpr Bitboard inner_ring[SQ_N] = {
			0x303ULL,              0x707ULL,              0xe0eULL,              0x1c1cULL, 
			0x3838ULL,             0x7070ULL,             0xe0e0ULL,             0xc0c0ULL, 
			0x30303ULL,            0x70707ULL,            0xe0e0eULL,            0x1c1c1cULL, 
			0x383838ULL,           0x707070ULL,           0xe0e0e0ULL,           0xc0c0c0ULL, 
			0x3030300ULL,          0x7070700ULL,          0xe0e0e00ULL,          0x1c1c1c00ULL, 
			0x38383800ULL,         0x70707000ULL,         0xe0e0e000ULL,         0xc0c0c000ULL, 
			0x303030000ULL,        0x707070000ULL,        0xe0e0e0000ULL,        0x1c1c1c0000ULL, 
			0x3838380000ULL,       0x7070700000ULL,       0xe0e0e00000ULL,       0xc0c0c00000ULL, 
			0x30303000000ULL,      0x70707000000ULL,      0xe0e0e000000ULL,      0x1c1c1c000000ULL, 
			0x383838000000ULL,     0x707070000000ULL,     0xe0e0e0000000ULL,     0xc0c0c0000000ULL, 
			0x3030300000000ULL,    0x7070700000000ULL,    0xe0e0e00000000ULL,    0x1c1c1c00000000ULL, 
			0x38383800000000ULL,   0x70707000000000ULL,   0xe0e0e000000000ULL,   0xc0c0c000000000ULL, 
			0x303030000000000ULL,  0x707070000000000ULL,  0xe0e0e0000000000ULL,  0x1c1c1c0000000000ULL, 
			0x3838380000000000ULL, 0x7070700000000000ULL, 0xe0e0e00000000000ULL, 0xc0c0c00000000000ULL, 
			0x303000000000000ULL,  0x707000000000000ULL,  0xe0e000000000000ULL,  0x1c1c000000000000ULL, 
			0x3838000000000000ULL, 0x7070000000000000ULL, 0xe0e0000000000000ULL, 0xc0c0000000000000ULL, 
		};
		
		constexpr Bitboard outer_ring[SQ_N] = {
			0x70404ULL,            0xf0808ULL,            0x1f1111ULL,           0x3e2222ULL,
			0x7c4444ULL,           0xf88888ULL,           0xf01010ULL,           0xe02020ULL,
			0x7040404ULL,          0xf080808ULL,          0x1f111111ULL,         0x3e222222ULL,
			0x7c444444ULL,         0xf8888888ULL,         0xf0101010ULL,         0xe0202020ULL,
			0x704040407ULL,        0xf0808080fULL,        0x1f1111111fULL,       0x3e2222223eULL,
			0x7c4444447cULL,       0xf8888888f8ULL,       0xf0101010f0ULL,       0xe0202020e0ULL,
			0x70404040700ULL,      0xf0808080f00ULL,      0x1f1111111f00ULL,     0x3e2222223e00ULL,
			0x7c4444447c00ULL,     0xf8888888f800ULL,     0xf0101010f000ULL,     0xe0202020e000ULL,
			0x7040404070000ULL,    0xf0808080f0000ULL,    0x1f1111111f0000ULL,   0x3e2222223e0000ULL,
			0x7c4444447c0000ULL,   0xf8888888f80000ULL,   0xf0101010f00000ULL,   0xe0202020e00000ULL,
			0x704040407000000ULL,  0xf0808080f000000ULL,  0x1f1111111f000000ULL, 0x3e2222223e000000ULL,
			0x7c4444447c000000ULL, 0xf8888888f8000000ULL, 0xf0101010f0000000ULL, 0xe0202020e0000000ULL,
			0x404040700000000ULL,  0x808080f00000000ULL,  0x1111111f00000000ULL, 0x2222223e00000000ULL,
			0x4444447c00000000ULL, 0x888888f800000000ULL, 0x101010f000000000ULL, 0x202020e000000000ULL,
			0x404070000000000ULL,  0x8080f0000000000ULL,  0x11111f0000000000ULL, 0x22223e0000000000ULL,
			0x44447c0000000000ULL, 0x8888f80000000000ULL, 0x1010f00000000000ULL, 0x2020e00000000000ULL, 
		};

#define KZ(out, in) KingZone(out, in)

		constexpr KingZone king_zones[SQ_N] = {
			KZ(outer_ring[0],  inner_ring[0]),  KZ(outer_ring[1],  inner_ring[1]), 
			KZ(outer_ring[2],  inner_ring[2]),  KZ(outer_ring[3],  inner_ring[3]), 
			KZ(outer_ring[4],  inner_ring[4]),  KZ(outer_ring[5],  inner_ring[5]), 
			KZ(outer_ring[6],  inner_ring[6]),  KZ(outer_ring[7],  inner_ring[7]), 
			KZ(outer_ring[8],  inner_ring[8]),  KZ(outer_ring[9],  inner_ring[9]), 
			KZ(outer_ring[10], inner_ring[10]), KZ(outer_ring[11], inner_ring[11]), 
			KZ(outer_ring[12], inner_ring[12]), KZ(outer_ring[13], inner_ring[13]), 
			KZ(outer_ring[14], inner_ring[14]), KZ(outer_ring[15], inner_ring[15]), 
			KZ(outer_ring[16], inner_ring[16]), KZ(outer_ring[17], inner_ring[17]), 
			KZ(outer_ring[18], inner_ring[18]), KZ(outer_ring[19], inner_ring[19]), 
			KZ(outer_ring[20], inner_ring[20]), KZ(outer_ring[21], inner_ring[21]), 
			KZ(outer_ring[22], inner_ring[22]), KZ(outer_ring[23], inner_ring[23]), 
			KZ(outer_ring[24], inner_ring[24]), KZ(outer_ring[25], inner_ring[25]), 
			KZ(outer_ring[26], inner_ring[26]), KZ(outer_ring[27], inner_ring[27]), 
			KZ(outer_ring[28], inner_ring[28]), KZ(outer_ring[29], inner_ring[29]), 
			KZ(outer_ring[30], inner_ring[30]), KZ(outer_ring[31], inner_ring[31]), 
			KZ(outer_ring[32], inner_ring[32]), KZ(outer_ring[33], inner_ring[33]), 
			KZ(outer_ring[34], inner_ring[34]), KZ(outer_ring[35], inner_ring[35]), 
			KZ(outer_ring[36], inner_ring[36]), KZ(outer_ring[37], inner_ring[37]), 
			KZ(outer_ring[38], inner_ring[38]), KZ(outer_ring[39], inner_ring[39]), 
			KZ(outer_ring[40], inner_ring[40]), KZ(outer_ring[41], inner_ring[41]), 
			KZ(outer_ring[42], inner_ring[42]), KZ(outer_ring[43], inner_ring[43]), 
			KZ(outer_ring[44], inner_ring[44]), KZ(outer_ring[45], inner_ring[45]), 
			KZ(outer_ring[46], inner_ring[46]), KZ(outer_ring[47], inner_ring[47]), 
			KZ(outer_ring[48], inner_ring[48]), KZ(outer_ring[49], inner_ring[49]), 
			KZ(outer_ring[50], inner_ring[50]), KZ(outer_ring[51], inner_ring[51]), 
			KZ(outer_ring[52], inner_ring[52]), KZ(outer_ring[53], inner_ring[53]), 
			KZ(outer_ring[54], inner_ring[54]), KZ(outer_ring[55], inner_ring[55]), 
			KZ(outer_ring[56], inner_ring[56]), KZ(outer_ring[57], inner_ring[57]), 
			KZ(outer_ring[58], inner_ring[58]), KZ(outer_ring[59], inner_ring[59]), 
			KZ(outer_ring[60], inner_ring[60]), KZ(outer_ring[61], inner_ring[61]), 
			KZ(outer_ring[62], inner_ring[62]), KZ(outer_ring[63], inner_ring[63]),
		};
		
#undef KZ
		
		constexpr int source32[] = {
			28, 29, 30, 31, 31, 30, 29, 28,
			24, 25, 26, 27, 27, 26, 25, 24,
			20, 21, 22, 23, 23, 22, 21, 20,
			16, 17, 18, 19, 19, 18, 17, 16,
			12, 13, 14, 15, 15, 14, 13, 12,
			 8,  9, 10, 11, 11, 10,  9,  8,
			 4,  5,  6,  7,  7,  6,  5,  4,
			 0,  1,  2,  3,  3,  2,  1,  0,
		};
		
		constexpr int source16[] = {
			 0,  1,  2,  3,  3,  2,  1,  0,
			 4,  5,  6,  7,  7,  6,  5,  4,
			 8,  9, 10, 11, 11, 10,  9,  8,
			12, 13, 14, 15, 15, 14, 13, 12,
			12, 13, 14, 15, 15, 14, 13, 12,
			 8,  9, 10, 11, 11, 10,  9,  8,
			 4,  5,  6,  7,  7,  6,  5,  4,
			 0,  1,  2,  3,  3,  2,  1,  0,
		};
		
		constexpr int source10[] = {
			 0,  1,  2,  3,  3,  2,  1,  0,
			 1,  4,  5,  6,  6,  5,  4,  1,
			 2,  5,  7,  8,  8,  7,  5,  2,
			 3,  6,  8,  9,  9,  8,  6,  3,
			 3,  6,  8,  9,  9,  8,  6,  3,
			 2,  5,  7,  8,  8,  7,  5,  2,
			 1,  4,  5,  6,  6,  5,  4,  1,
			 0,  1,  2,  3,  3,  2,  1,  0,
		};

		void init_eval();
		template<bool TRACE> int evaluate(const Position& pos);
		
		extern int T[TI_MISC][PHASE_N];

	} // namespace Eval

} // namespace Clovis
