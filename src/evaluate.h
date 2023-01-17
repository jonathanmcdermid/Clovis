#pragma once

#include "position.h"
#include "tt.h"

namespace Clovis {

	struct Position;

	namespace Eval {

#define S(mg, eg) Score(mg, eg)

		constexpr Score pawn_table[] = {
			S(0,0), S(0,0), S(0,0), S(0,0),
			S(140,334), S(176,312), S(182,281), S(203,261),
			S(94,111), S(112,109), S(141,101), S(136,97),
			S(76,99), S(94,93), S(97,95), S(112,83),
			S(66,90), S(74,89), S(94,87), S(105,83),
			S(75,82), S(84,83), S(94,87), S(96,92),
			S(68,85), S(92,83), S(91,96), S(85,97),
			S(0,0), S(0,0), S(0,0), S(0,0),
        };

        constexpr Score knight_table[] = {
            S(178,236), S(247,263), S(238,290), S(324,276),
            S(284,272), S(313,291), S(403,274), S(337,306),
            S(316,275), S(382,282), S(369,311), S(382,307),
            S(352,284), S(353,305), S(361,321), S(366,327),
            S(336,288), S(357,302), S(364,318), S(365,321),
            S(331,282), S(360,293), S(364,302), S(369,316),
            S(332,265), S(333,282), S(352,295), S(366,296),
            S(302,259), S(334,255), S(334,282), S(342,286),
        };

        constexpr Score bishop_table[] = {
            S(275,299), S(290,299), S(205,315), S(226,316),
            S(277,305), S(319,305), S(299,314), S(298,307),
            S(291,316), S(322,308), S(330,308), S(311,309),
            S(315,308), S(322,308), S(317,316), S(336,313),
            S(332,299), S(325,305), S(323,317), S(346,311),
            S(333,300), S(345,305), S(355,308), S(339,319),
            S(338,291), S(366,289), S(350,301), S(344,304),
            S(314,295), S(332,305), S(332,298), S(335,303),
        };

        constexpr Score rook_table[] = {
            S(445,544), S(463,539), S(431,548), S(473,539),
            S(468,542), S(468,543), S(507,531), S(512,525),
            S(456,536), S(487,532), S(468,535), S(469,534),
            S(446,540), S(455,536), S(479,538), S(473,531),
            S(443,535), S(467,529), S(462,533), S(472,527),
            S(446,524), S(468,524), S(474,518), S(471,520),
            S(442,523), S(471,516), S(472,519), S(482,517),
            S(474,509), S(469,519), S(490,516), S(493,512),
        };

        constexpr Score queen_table[] = {
            S(914,978), S(890,1009), S(900,1016), S(921,996),
            S(915,984), S(864,1024), S(883,1028), S(853,1059),
            S(933,980), S(920,997), S(920,1000), S(886,1044),
            S(911,1016), S(897,1034), S(898,1016), S(892,1029),
            S(927,981), S(909,1025), S(916,1009), S(913,1020),
            S(919,1000), S(943,976), S(928,1000), S(931,988),
            S(921,975), S(939,956), S(953,958), S(945,975),
            S(941,951), S(931,959), S(938,961), S(951,950),
        };

        constexpr Score king_table[] = {
            S(3,58), S(162,67), S(119,97), S(110,87),
            S(98,83), S(137,112), S(128,129), S(174,113),
            S(112,95), S(204,118), S(219,124), S(158,116),
            S(40,96), S(105,126), S(105,136), S(85,141),
            S(3,95), S(63,113), S(52,137), S(23,151),
            S(56,87), S(89,106), S(73,124), S(61,139),
            S(101,68), S(110,89), S(69,118), S(45,130),
            S(94,33), S(125,60), S(75,92), S(89,86),
        };

        constexpr Score passed_pawn_bonus[] = {
            S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
            S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
            S(17, 137),  S(4, 127),  S(11, 101),  S(5, 79),
            S(20, 67),  S(6, 65),  S(12, 40),  S(8, 37),
            S(18, 35),  S(0, 35),  S(0, 17),  S(0, 16),
            S(12, 8),  S(0, 12),  S(0, 0),  S(0, 0),
            S(4, 9),  S(6, 6),  S(4, 0),  S(0, 0),
            S(0, 0),  S(0, 0),  S(0, 0),  S(0, 0),
        };

		constexpr Score double_pawn_penalty = S(0, 8);
        constexpr Score isolated_pawn_penalty = S(18, 9);
        constexpr Score bishop_pair_bonus = S(37, 47);
        constexpr Score rook_open_file_bonus = S(40, 0);
        constexpr Score rook_semi_open_file_bonus = S(12, 19);
        constexpr Score tempo_bonus = S(20, 17);
        constexpr Score king_full_open_penalty = S(51, 7);
        constexpr Score king_semi_open_penalty = S(17, 0);
        constexpr Score king_adjacent_full_open_penalty = S(12, 9);
        constexpr Score king_adjacent_semi_open_penalty = S(17, 0);
        constexpr Score mobility[7] = { S(0, 0), S(0, 0), S(4, 0), S(8, 2), S(4, 3), S(3, 5), S(0, 0), };
        constexpr Score inner_ring_attack[7] = { S(0, 0), S(2, 0), S(4, 10), S(7, 5), S(7, 0), S(4, 1), S(0, 0), };
        constexpr Score outer_ring_attack[7] = { S(0, 0), S(0, 0), S(6, 0), S(3, 0), S(2, 3), S(4, 0), S(0, 0), };
        constexpr Score knight_outpost_bonus = S(39, 13);
        constexpr Score bishop_outpost_bonus = S(46, 0);

#undef S

		extern const Score* piece_table[7];
        extern const Score* score_table[15][SQ_N];
		extern const Score* passed_table[COLOUR_N][SQ_N];

        constexpr Bitboard file_masks[SQ_N] =  {
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL, 
			0x101010101010101ULL, 0x202020202020202ULL, 0x404040404040404ULL, 0x808080808080808ULL, 
			0x1010101010101010ULL, 0x2020202020202020ULL, 0x4040404040404040ULL, 0x8080808080808080ULL,
		};

        constexpr Bitboard rank_masks[SQ_N] = {
			0xffULL, 0xffULL, 0xffULL, 0xffULL, 
			0xffULL, 0xffULL, 0xffULL, 0xffULL, 
			0xff00ULL, 0xff00ULL, 0xff00ULL, 0xff00ULL, 
			0xff00ULL, 0xff00ULL, 0xff00ULL, 0xff00ULL, 
			0xff0000ULL, 0xff0000ULL, 0xff0000ULL, 0xff0000ULL, 
			0xff0000ULL, 0xff0000ULL, 0xff0000ULL, 0xff0000ULL, 
			0xff000000ULL, 0xff000000ULL, 0xff000000ULL, 0xff000000ULL, 
			0xff000000ULL, 0xff000000ULL, 0xff000000ULL, 0xff000000ULL, 
			0xff00000000ULL, 0xff00000000ULL, 0xff00000000ULL, 0xff00000000ULL, 
			0xff00000000ULL, 0xff00000000ULL, 0xff00000000ULL, 0xff00000000ULL, 
			0xff0000000000ULL, 0xff0000000000ULL, 0xff0000000000ULL, 0xff0000000000ULL, 
			0xff0000000000ULL, 0xff0000000000ULL, 0xff0000000000ULL, 0xff0000000000ULL, 
			0xff000000000000ULL, 0xff000000000000ULL, 0xff000000000000ULL, 0xff000000000000ULL, 
			0xff000000000000ULL, 0xff000000000000ULL, 0xff000000000000ULL, 0xff000000000000ULL, 
			0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL, 
			0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL, 0xff00000000000000ULL,
		};

        constexpr Bitboard isolated_masks[SQ_N] = {
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL, 
			0x202020202020202ULL, 0x505050505050505ULL, 0xa0a0a0a0a0a0a0aULL, 0x1414141414141414ULL, 
			0x2828282828282828ULL, 0x5050505050505050ULL, 0xa0a0a0a0a0a0a0a0ULL, 0x4040404040404040ULL,
		};

        constexpr Bitboard passed_masks[COLOUR_N][SQ_N] = {
			0x303030303030300ULL, 0x707070707070700ULL, 0xe0e0e0e0e0e0e00ULL, 0x1c1c1c1c1c1c1c00ULL, 
			0x3838383838383800ULL, 0x7070707070707000ULL, 0xe0e0e0e0e0e0e000ULL, 0xc0c0c0c0c0c0c000ULL, 
			0x303030303030000ULL, 0x707070707070000ULL, 0xe0e0e0e0e0e0000ULL, 0x1c1c1c1c1c1c0000ULL, 
			0x3838383838380000ULL, 0x7070707070700000ULL, 0xe0e0e0e0e0e00000ULL, 0xc0c0c0c0c0c00000ULL, 
			0x303030303000000ULL, 0x707070707000000ULL, 0xe0e0e0e0e000000ULL, 0x1c1c1c1c1c000000ULL, 
			0x3838383838000000ULL, 0x7070707070000000ULL, 0xe0e0e0e0e0000000ULL, 0xc0c0c0c0c0000000ULL, 
			0x303030300000000ULL, 0x707070700000000ULL, 0xe0e0e0e00000000ULL, 0x1c1c1c1c00000000ULL, 
			0x3838383800000000ULL, 0x7070707000000000ULL, 0xe0e0e0e000000000ULL, 0xc0c0c0c000000000ULL, 
			0x303030000000000ULL, 0x707070000000000ULL, 0xe0e0e0000000000ULL, 0x1c1c1c0000000000ULL, 
			0x3838380000000000ULL, 0x7070700000000000ULL, 0xe0e0e00000000000ULL, 0xc0c0c00000000000ULL, 
			0x303000000000000ULL, 0x707000000000000ULL, 0xe0e000000000000ULL, 0x1c1c000000000000ULL, 
			0x3838000000000000ULL, 0x7070000000000000ULL, 0xe0e0000000000000ULL, 0xc0c0000000000000ULL, 
			0x300000000000000ULL, 0x700000000000000ULL, 0xe00000000000000ULL, 0x1c00000000000000ULL, 
			0x3800000000000000ULL, 0x7000000000000000ULL, 0xe000000000000000ULL, 0xc000000000000000ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x3ULL, 0x7ULL, 0xeULL, 0x1cULL, 
			0x38ULL, 0x70ULL, 0xe0ULL, 0xc0ULL, 
			0x303ULL, 0x707ULL, 0xe0eULL, 0x1c1cULL, 
			0x3838ULL, 0x7070ULL, 0xe0e0ULL, 0xc0c0ULL, 
			0x30303ULL, 0x70707ULL, 0xe0e0eULL, 0x1c1c1cULL, 
			0x383838ULL, 0x707070ULL, 0xe0e0e0ULL, 0xc0c0c0ULL, 
			0x3030303ULL, 0x7070707ULL, 0xe0e0e0eULL, 0x1c1c1c1cULL, 
			0x38383838ULL, 0x70707070ULL, 0xe0e0e0e0ULL, 0xc0c0c0c0ULL, 
			0x303030303ULL, 0x707070707ULL, 0xe0e0e0e0eULL, 0x1c1c1c1c1cULL, 
			0x3838383838ULL, 0x7070707070ULL, 0xe0e0e0e0e0ULL, 0xc0c0c0c0c0ULL, 
			0x30303030303ULL, 0x70707070707ULL, 0xe0e0e0e0e0eULL, 0x1c1c1c1c1c1cULL, 
			0x383838383838ULL, 0x707070707070ULL, 0xe0e0e0e0e0e0ULL, 0xc0c0c0c0c0c0ULL, 
			0x3030303030303ULL, 0x7070707070707ULL, 0xe0e0e0e0e0e0eULL, 0x1c1c1c1c1c1c1cULL, 
			0x38383838383838ULL, 0x70707070707070ULL, 0xe0e0e0e0e0e0e0ULL, 0xc0c0c0c0c0c0c0ULL,
		};

        constexpr Bitboard outpost_masks[COLOUR_N] = { 0xffffff000000ULL, 0xffffff0000ULL };

        constexpr Bitboard outpost_pawn_masks[COLOUR_N][SQ_N] = {
			0x202020202020200ULL, 0x505050505050500ULL, 0xa0a0a0a0a0a0a00ULL, 0x1414141414141400ULL, 
			0x2828282828282800ULL, 0x5050505050505000ULL, 0xa0a0a0a0a0a0a000ULL, 0x4040404040404000ULL, 
			0x202020202020000ULL, 0x505050505050000ULL, 0xa0a0a0a0a0a0000ULL, 0x1414141414140000ULL, 
			0x2828282828280000ULL, 0x5050505050500000ULL, 0xa0a0a0a0a0a00000ULL, 0x4040404040400000ULL, 
			0x202020202000000ULL, 0x505050505000000ULL, 0xa0a0a0a0a000000ULL, 0x1414141414000000ULL, 
			0x2828282828000000ULL, 0x5050505050000000ULL, 0xa0a0a0a0a0000000ULL, 0x4040404040000000ULL, 
			0x202020200000000ULL, 0x505050500000000ULL, 0xa0a0a0a00000000ULL, 0x1414141400000000ULL, 
			0x2828282800000000ULL, 0x5050505000000000ULL, 0xa0a0a0a000000000ULL, 0x4040404000000000ULL, 
			0x202020000000000ULL, 0x505050000000000ULL, 0xa0a0a0000000000ULL, 0x1414140000000000ULL, 
			0x2828280000000000ULL, 0x5050500000000000ULL, 0xa0a0a00000000000ULL, 0x4040400000000000ULL, 
			0x202000000000000ULL, 0x505000000000000ULL, 0xa0a000000000000ULL, 0x1414000000000000ULL, 
			0x2828000000000000ULL, 0x5050000000000000ULL, 0xa0a0000000000000ULL, 0x4040000000000000ULL, 
			0x200000000000000ULL, 0x500000000000000ULL, 0xa00000000000000ULL, 0x1400000000000000ULL, 
			0x2800000000000000ULL, 0x5000000000000000ULL, 0xa000000000000000ULL, 0x4000000000000000ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x0ULL, 0x0ULL, 0x0ULL, 0x0ULL, 
			0x2ULL, 0x5ULL, 0xaULL, 0x14ULL, 
			0x28ULL, 0x50ULL, 0xa0ULL, 0x40ULL, 
			0x202ULL, 0x505ULL, 0xa0aULL, 0x1414ULL, 
			0x2828ULL, 0x5050ULL, 0xa0a0ULL, 0x4040ULL, 
			0x20202ULL, 0x50505ULL, 0xa0a0aULL, 0x141414ULL, 
			0x282828ULL, 0x505050ULL, 0xa0a0a0ULL, 0x404040ULL, 
			0x2020202ULL, 0x5050505ULL, 0xa0a0a0aULL, 0x14141414ULL, 
			0x28282828ULL, 0x50505050ULL, 0xa0a0a0a0ULL, 0x40404040ULL, 
			0x202020202ULL, 0x505050505ULL, 0xa0a0a0a0aULL, 0x1414141414ULL, 
			0x2828282828ULL, 0x5050505050ULL, 0xa0a0a0a0a0ULL, 0x4040404040ULL, 
			0x20202020202ULL, 0x50505050505ULL, 0xa0a0a0a0a0aULL, 0x141414141414ULL, 
			0x282828282828ULL, 0x505050505050ULL, 0xa0a0a0a0a0a0ULL, 0x404040404040ULL, 
			0x2020202020202ULL, 0x5050505050505ULL, 0xa0a0a0a0a0a0aULL, 0x14141414141414ULL, 
			0x28282828282828ULL, 0x50505050505050ULL, 0xa0a0a0a0a0a0a0ULL, 0x40404040404040ULL,
		};

#define KZ(out, in) KingZone(out, in)

		constexpr KingZone king_zones[SQ_N] = {
			KZ(0x70404ULL, 0x302ULL), KZ(0xf0808ULL, 0x705ULL), 
			KZ(0x1f1111ULL, 0xe0aULL), KZ(0x3e2222ULL, 0x1c14ULL), 
			KZ(0x7c4444ULL, 0x3828ULL), KZ(0xf88888ULL, 0x7050ULL), 
			KZ(0xf01010ULL, 0xe0a0ULL), KZ(0xe02020ULL, 0xc040ULL), 
			KZ(0x7040404ULL, 0x30203ULL), KZ(0xf080808ULL, 0x70507ULL), 
			KZ(0x1f111111ULL, 0xe0a0eULL), KZ(0x3e222222ULL, 0x1c141cULL), 
			KZ(0x7c444444ULL, 0x382838ULL), KZ(0xf8888888ULL, 0x705070ULL), 
			KZ(0xf0101010ULL, 0xe0a0e0ULL), KZ(0xe0202020ULL, 0xc040c0ULL), 
			KZ(0x704040407ULL, 0x3020300ULL), KZ(0xf0808080fULL, 0x7050700ULL), 
			KZ(0x1f1111111fULL, 0xe0a0e00ULL), KZ(0x3e2222223eULL, 0x1c141c00ULL), 
			KZ(0x7c4444447cULL, 0x38283800ULL), KZ(0xf8888888f8ULL, 0x70507000ULL), 
			KZ(0xf0101010f0ULL, 0xe0a0e000ULL), KZ(0xe0202020e0ULL, 0xc040c000ULL), 
			KZ(0x70404040700ULL, 0x302030000ULL), KZ(0xf0808080f00ULL, 0x705070000ULL), 
			KZ(0x1f1111111f00ULL, 0xe0a0e0000ULL), KZ(0x3e2222223e00ULL, 0x1c141c0000ULL), 
			KZ(0x7c4444447c00ULL, 0x3828380000ULL), KZ(0xf8888888f800ULL, 0x7050700000ULL), 
			KZ(0xf0101010f000ULL, 0xe0a0e00000ULL), KZ(0xe0202020e000ULL, 0xc040c00000ULL), 
			KZ(0x7040404070000ULL, 0x30203000000ULL), KZ(0xf0808080f0000ULL, 0x70507000000ULL), 
			KZ(0x1f1111111f0000ULL, 0xe0a0e000000ULL), KZ(0x3e2222223e0000ULL, 0x1c141c000000ULL), 
			KZ(0x7c4444447c0000ULL, 0x382838000000ULL), KZ(0xf8888888f80000ULL, 0x705070000000ULL), 
			KZ(0xf0101010f00000ULL, 0xe0a0e0000000ULL), KZ(0xe0202020e00000ULL, 0xc040c0000000ULL), 
			KZ(0x704040407000000ULL, 0x3020300000000ULL), KZ(0xf0808080f000000ULL, 0x7050700000000ULL), 
			KZ(0x1f1111111f000000ULL, 0xe0a0e00000000ULL), KZ(0x3e2222223e000000ULL, 0x1c141c00000000ULL), 
			KZ(0x7c4444447c000000ULL, 0x38283800000000ULL), KZ(0xf8888888f8000000ULL, 0x70507000000000ULL), 
			KZ(0xf0101010f0000000ULL, 0xe0a0e000000000ULL), KZ(0xe0202020e0000000ULL, 0xc040c000000000ULL), 
			KZ(0x404040700000000ULL, 0x302030000000000ULL), KZ(0x808080f00000000ULL, 0x705070000000000ULL), 
			KZ(0x1111111f00000000ULL, 0xe0a0e0000000000ULL), KZ(0x2222223e00000000ULL, 0x1c141c0000000000ULL), 
			KZ(0x4444447c00000000ULL, 0x3828380000000000ULL), KZ(0x888888f800000000ULL, 0x7050700000000000ULL), 
			KZ(0x101010f000000000ULL, 0xe0a0e00000000000ULL), KZ(0x202020e000000000ULL, 0xc040c00000000000ULL), 
			KZ(0x404070000000000ULL, 0x203000000000000ULL), KZ(0x8080f0000000000ULL, 0x507000000000000ULL), 
			KZ(0x11111f0000000000ULL, 0xa0e000000000000ULL), KZ(0x22223e0000000000ULL, 0x141c000000000000ULL), 
			KZ(0x44447c0000000000ULL, 0x2838000000000000ULL), KZ(0x8888f80000000000ULL, 0x5070000000000000ULL), 
			KZ(0x1010f00000000000ULL, 0xa0e0000000000000ULL), KZ(0x2020e00000000000ULL, 0x40c0000000000000ULL),
		};

#undef KZ

		void init_eval();
		void init_values();
		
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
			constexpr Piece OUR_ROOK	= make_piece(ROOK, US);
			constexpr Piece PIECE		= make_piece(PT, US);

			constexpr Piece THEIR_PAWN	= make_piece(PAWN, THEM);
			constexpr Piece THEIR_ROOK	= make_piece(ROOK, THEM);

			assert(PT >= KNIGHT && PT < KING);

			Score score;
			Square sq;
			Bitboard bb = pos.piece_bitboard[PIECE];

			Bitboard transparent_occ =
				PT == BISHOP
				? pos.occ_bitboard[BOTH] ^ pos.piece_bitboard[W_QUEEN] ^ pos.piece_bitboard[B_QUEEN] ^ pos.piece_bitboard[THEIR_ROOK]
				: PT == ROOK 
				? pos.occ_bitboard[BOTH] ^ pos.piece_bitboard[W_QUEEN] ^ pos.piece_bitboard[B_QUEEN] ^ pos.piece_bitboard[OUR_ROOK]
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
					score += *passed_table[US][sq];

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

#undef S
#undef KZ
