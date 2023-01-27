#pragma once

#include <immintrin.h>
#include <cstring>
#include <cassert>
#include <iostream>
#include <string>

#include "random.h"
#include "types.h"

namespace Clovis {

    constexpr Bitboard sqbb(Square sq) { return 1ULL << sq; }

    constexpr Bitboard operator|(Square sq1, Square sq2) { return sqbb(sq1) | sqbb(sq2); }
    constexpr Bitboard operator&(Bitboard bb, Square sq) { return bb & sqbb(sq); }
    constexpr Bitboard operator|(Bitboard bb, Square sq) { return bb | sqbb(sq); }
    constexpr Bitboard operator^(Bitboard bb, Square sq) { return bb ^ sqbb(sq); }
    constexpr Bitboard& operator|=(Bitboard& bb, Square sq) { return bb |= sqbb(sq); }
    constexpr Bitboard& operator^=(Bitboard& bb, Square sq) { return bb ^= sqbb(sq); }

#if defined(__GNUC__)

    inline int popcnt(Bitboard bb) {
        return __builtin_popcountll(bb);
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        return Square(__builtin_ctzll(bb));
    }

    inline Square msb(Bitboard bb) {
        assert(bb);
        return Square(63 ^ __builtin_clzll(bb));
    }

#elif defined(_MSC_VER)

#ifdef _WIN64

    inline int popcnt(Bitboard bb) {
        return __popcnt64(bb);
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        unsigned long pos;
        _BitScanForward64(&pos, bb);
        return Square(pos);
    }

	inline Square msb(Bitboard bb) {
        assert(bb);
        unsigned long idx;
        _BitScanReverse64(&idx, bb);
        return Square(idx);
    }

#else

    inline int popcnt(Bitboard bb) {
        return __popcnt(int32_t(bb)) + __popcnt(int32_t(bb >> 32));
    }

    inline Square lsb(Bitboard bb) {
        assert(bb);
        unsigned long pos;

        if (bb & 0xffffffff)
        {
            _BitScanForward(&pos, int32_t(bb));
            return Square(pos);
        }
        else
        {
            _BitScanForward(&pos, int32_t(bb >> 32));
            return Square(pos + 32);
        }
    }

    inline Square msb(Bitboard bb) {
        assert(bb);
        unsigned long idx;

        if (bb >> 32) 
		{
            _BitScanReverse(&idx, int32_t(bb >> 32));
            return Square(idx + 32);
        } else 
		{
            _BitScanReverse(&idx, int32_t(bb));
            return Square(idx);
        }
    }

#endif

#else  // Cant use GCC or MSVC 

#error "Invalid Compiler"

#endif

    inline Square pop_lsb(Bitboard& bb) {
        assert(bb);
        Square sq = lsb(bb);
        bb &= bb - 1;
        return sq;
    }

    namespace Bitboards {

        constexpr int bishop_attack_indices = 512;
        constexpr int rook_attack_indices = 4096;

		// 64 - number of relevant occupancy bits for every bishop square
		constexpr int bishop_rbits[SQ_N] = {
			58, 59, 59, 59, 59, 59, 59, 58,
			59, 59, 59, 59, 59, 59, 59, 59,
			59, 59, 57, 57, 57, 57, 59, 59,
			59, 59, 57, 55, 55, 57, 59, 59,
			59, 59, 57, 55, 55, 57, 59, 59,
			59, 59, 57, 57, 57, 57, 59, 59,
			59, 59, 59, 59, 59, 59, 59, 59,
			58, 59, 59, 59, 59, 59, 59, 58,
		};

		// 64 - number of relevant occupancy bits for every rook square
		constexpr int rook_rbits[SQ_N] = {
			52, 53, 53, 53, 53, 53, 53, 52,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			53, 54, 54, 54, 54, 54, 54, 53,
			52, 53, 53, 53, 53, 53, 53, 52,
		};

		// precalculated magic numbers for generating rook attacks
		constexpr Bitboard rook_magic[SQ_N] = {
			0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,  0x100081001000420ULL, 
			0x200020010080420ULL,  0x3001c0002010008ULL,  0x8480008002000100ULL, 0x2080088004402900ULL, 
			0x800098204000ULL,     0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL, 
			0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL, 0x801000060821100ULL, 
			0x80044006422000ULL,   0x100808020004000ULL,  0x12108a0010204200ULL, 0x140848010000802ULL, 
			0x481828014002800ULL,  0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL, 
			0x100400080208000ULL,  0x2040002120081000ULL, 0x21200680100081ULL,   0x20100080080080ULL, 
			0x2000a00200410ULL,    0x20080800400ULL,      0x80088400100102ULL,   0x80004600042881ULL, 
			0x4040008040800020ULL, 0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL, 
			0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,  0x200046502000484ULL, 
			0x480400080088020ULL,  0x1000422010034000ULL, 0x30200100110040ULL,   0x100021010009ULL, 
			0x2002080100110004ULL, 0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL, 
			0x101002200408200ULL,  0x40802000401080ULL,   0x4008142004410100ULL, 0x2060820c0120200ULL, 
			0x1001004080100ULL,    0x20c020080040080ULL,  0x2935610830022400ULL, 0x44440041009200ULL, 
			0x280001040802101ULL,  0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL, 
			0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,  0x1004081002402ULL, 
		};

		// precalculated magic numbers for generating bishop attacks
		constexpr Bitboard bishop_magic[SQ_N] = {
			0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,   0x108060845042010ULL, 
			0x581104180800210ULL,  0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL, 
			0x4050404440404ULL,    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL, 
			0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,  0x801010402020200ULL, 
			0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL, 
			0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL, 
			0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL, 
			0x4001001021004000ULL, 0x800040400a011002ULL, 0xe4004081011002ULL,   0x1c004001012080ULL, 
			0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL, 
			0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL, 
			0x209188240001000ULL,  0x400408a884001800ULL, 0x110400a6080400ULL,   0x1840060a44020800ULL, 
			0x90080104000041ULL,   0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL, 
			0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL, 0x300000261044000aULL, 
			0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL, 0x4010801011c04ULL, 
			0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL, 
			0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL, 
		};

		constexpr Bitboard bishop_masks[SQ_N] = {
			0x40201008040200ULL,   0x402010080400ULL,     0x4020100a00ULL,       0x40221400ULL, 
			0x2442800ULL,          0x204085000ULL,        0x20408102000ULL,      0x2040810204000ULL, 
			0x20100804020000ULL,   0x40201008040000ULL,   0x4020100a0000ULL,     0x4022140000ULL, 
			0x244280000ULL,        0x20408500000ULL,      0x2040810200000ULL,    0x4081020400000ULL, 
			0x10080402000200ULL,   0x20100804000400ULL,   0x4020100a000a00ULL,   0x402214001400ULL, 
			0x24428002800ULL,      0x2040850005000ULL,    0x4081020002000ULL,    0x8102040004000ULL, 
			0x8040200020400ULL,    0x10080400040800ULL,   0x20100a000a1000ULL,   0x40221400142200ULL, 
			0x2442800284400ULL,    0x4085000500800ULL,    0x8102000201000ULL,    0x10204000402000ULL, 
			0x4020002040800ULL,    0x8040004081000ULL,    0x100a000a102000ULL,   0x22140014224000ULL, 
			0x44280028440200ULL,   0x8500050080400ULL,    0x10200020100800ULL,   0x20400040201000ULL, 
			0x2000204081000ULL,    0x4000408102000ULL,    0xa000a10204000ULL,    0x14001422400000ULL, 
			0x28002844020000ULL,   0x50005008040200ULL,   0x20002010080400ULL,   0x40004020100800ULL, 
			0x20408102000ULL,      0x40810204000ULL,      0xa1020400000ULL,      0x142240000000ULL, 
			0x284402000000ULL,     0x500804020000ULL,     0x201008040200ULL,     0x402010080400ULL, 
			0x2040810204000ULL,    0x4081020400000ULL,    0xa102040000000ULL,    0x14224000000000ULL, 
			0x28440200000000ULL,   0x50080402000000ULL,   0x20100804020000ULL,   0x40201008040200ULL, 
		};

		constexpr Bitboard rook_masks[SQ_N] = {
			0x101010101017eULL,    0x202020202027cULL,    0x404040404047aULL,    0x8080808080876ULL, 
			0x1010101010106eULL,   0x2020202020205eULL,   0x4040404040403eULL,   0x8080808080807eULL, 
			0x1010101017e00ULL,    0x2020202027c00ULL,    0x4040404047a00ULL,    0x8080808087600ULL, 
			0x10101010106e00ULL,   0x20202020205e00ULL,   0x40404040403e00ULL,   0x80808080807e00ULL, 
			0x10101017e0100ULL,    0x20202027c0200ULL,    0x40404047a0400ULL,    0x8080808760800ULL, 
			0x101010106e1000ULL,   0x202020205e2000ULL,   0x404040403e4000ULL,   0x808080807e8000ULL, 
			0x101017e010100ULL,    0x202027c020200ULL,    0x404047a040400ULL,    0x8080876080800ULL, 
			0x1010106e101000ULL,   0x2020205e202000ULL,   0x4040403e404000ULL,   0x8080807e808000ULL, 
			0x1017e01010100ULL,    0x2027c02020200ULL,    0x4047a04040400ULL,    0x8087608080800ULL, 
			0x10106e10101000ULL,   0x20205e20202000ULL,   0x40403e40404000ULL,   0x80807e80808000ULL, 
			0x17e0101010100ULL,    0x27c0202020200ULL,    0x47a0404040400ULL,    0x8760808080800ULL, 
			0x106e1010101000ULL,   0x205e2020202000ULL,   0x403e4040404000ULL,   0x807e8080808000ULL, 
			0x7e010101010100ULL,   0x7c020202020200ULL,   0x7a040404040400ULL,   0x76080808080800ULL, 
			0x6e101010101000ULL,   0x5e202020202000ULL,   0x3e404040404000ULL,   0x7e808080808000ULL, 
			0x7e01010101010100ULL, 0x7c02020202020200ULL, 0x7a04040404040400ULL, 0x7608080808080800ULL, 
			0x6e10101010101000ULL, 0x5e20202020202000ULL, 0x3e40404040404000ULL, 0x7e80808080808000ULL, 
		};

        constexpr Bitboard pawn_attacks[COLOUR_N][SQ_N] = {
			0x200ULL,              0x500ULL,              0xa00ULL,              0x1400ULL, 
			0x2800ULL,             0x5000ULL,             0xa000ULL,             0x4000ULL, 
			0x20000ULL,            0x50000ULL,            0xa0000ULL,            0x140000ULL, 
			0x280000ULL,           0x500000ULL,           0xa00000ULL,           0x400000ULL, 
			0x2000000ULL,          0x5000000ULL,          0xa000000ULL,          0x14000000ULL, 
			0x28000000ULL,         0x50000000ULL,         0xa0000000ULL,         0x40000000ULL, 
			0x200000000ULL,        0x500000000ULL,        0xa00000000ULL,        0x1400000000ULL, 
			0x2800000000ULL,       0x5000000000ULL,       0xa000000000ULL,       0x4000000000ULL, 
			0x20000000000ULL,      0x50000000000ULL,      0xa0000000000ULL,      0x140000000000ULL, 
			0x280000000000ULL,     0x500000000000ULL,     0xa00000000000ULL,     0x400000000000ULL, 
			0x2000000000000ULL,    0x5000000000000ULL,    0xa000000000000ULL,    0x14000000000000ULL, 
			0x28000000000000ULL,   0x50000000000000ULL,   0xa0000000000000ULL,   0x40000000000000ULL, 
			0x200000000000000ULL,  0x500000000000000ULL,  0xa00000000000000ULL,  0x1400000000000000ULL, 
			0x2800000000000000ULL, 0x5000000000000000ULL, 0xa000000000000000ULL, 0x4000000000000000ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x0ULL,                0x0ULL,                0x0ULL,                0x0ULL, 
			0x2ULL,                0x5ULL,                0xaULL,                0x14ULL, 
			0x28ULL,               0x50ULL,               0xa0ULL,               0x40ULL, 
			0x200ULL,              0x500ULL,              0xa00ULL,              0x1400ULL, 
			0x2800ULL,             0x5000ULL,             0xa000ULL,             0x4000ULL, 
			0x20000ULL,            0x50000ULL,            0xa0000ULL,            0x140000ULL, 
			0x280000ULL,           0x500000ULL,           0xa00000ULL,           0x400000ULL, 
			0x2000000ULL,          0x5000000ULL,          0xa000000ULL,          0x14000000ULL, 
			0x28000000ULL,         0x50000000ULL,         0xa0000000ULL,         0x40000000ULL, 
			0x200000000ULL,        0x500000000ULL,        0xa00000000ULL,        0x1400000000ULL, 
			0x2800000000ULL,       0x5000000000ULL,       0xa000000000ULL,       0x4000000000ULL, 
			0x20000000000ULL,      0x50000000000ULL,      0xa0000000000ULL,      0x140000000000ULL, 
			0x280000000000ULL,     0x500000000000ULL,     0xa00000000000ULL,     0x400000000000ULL, 
			0x2000000000000ULL,    0x5000000000000ULL,    0xa000000000000ULL,    0x14000000000000ULL, 
			0x28000000000000ULL,   0x50000000000000ULL,   0xa0000000000000ULL,   0x40000000000000ULL,
		};

        constexpr Bitboard knight_attacks[SQ_N] = {
			0x20400ULL,            0x50800ULL,            0xa1100ULL,            0x142200ULL, 
			0x284400ULL,           0x508800ULL,           0xa01000ULL,           0x402000ULL, 
			0x2040004ULL,          0x5080008ULL,          0xa110011ULL,          0x14220022ULL, 
			0x28440044ULL,         0x50880088ULL,         0xa0100010ULL,         0x40200020ULL, 
			0x204000402ULL,        0x508000805ULL,        0xa1100110aULL,        0x1422002214ULL, 
			0x2844004428ULL,       0x5088008850ULL,       0xa0100010a0ULL,       0x4020002040ULL, 
			0x20400040200ULL,      0x50800080500ULL,      0xa1100110a00ULL,      0x142200221400ULL, 
			0x284400442800ULL,     0x508800885000ULL,     0xa0100010a000ULL,     0x402000204000ULL, 
			0x2040004020000ULL,    0x5080008050000ULL,    0xa1100110a0000ULL,    0x14220022140000ULL, 
			0x28440044280000ULL,   0x50880088500000ULL,   0xa0100010a00000ULL,   0x40200020400000ULL, 
			0x204000402000000ULL,  0x508000805000000ULL,  0xa1100110a000000ULL,  0x1422002214000000ULL, 
			0x2844004428000000ULL, 0x5088008850000000ULL, 0xa0100010a0000000ULL, 0x4020002040000000ULL, 
			0x400040200000000ULL,  0x800080500000000ULL,  0x1100110a00000000ULL, 0x2200221400000000ULL, 
			0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010a000000000ULL, 0x2000204000000000ULL, 
			0x4020000000000ULL,    0x8050000000000ULL,    0x110a0000000000ULL,   0x22140000000000ULL, 
			0x44280000000000ULL,   0x88500000000000ULL,   0x10a00000000000ULL,   0x20400000000000ULL, 
		};

        constexpr Bitboard king_attacks[SQ_N] = {
			0x302ULL,              0x705ULL,              0xe0aULL,              0x1c14ULL, 
			0x3828ULL,             0x7050ULL,             0xe0a0ULL,             0xc040ULL, 
			0x30203ULL,            0x70507ULL,            0xe0a0eULL,            0x1c141cULL, 
			0x382838ULL,           0x705070ULL,           0xe0a0e0ULL,           0xc040c0ULL, 
			0x3020300ULL,          0x7050700ULL,          0xe0a0e00ULL,          0x1c141c00ULL, 
			0x38283800ULL,         0x70507000ULL,         0xe0a0e000ULL,         0xc040c000ULL, 
			0x302030000ULL,        0x705070000ULL,        0xe0a0e0000ULL,        0x1c141c0000ULL, 
			0x3828380000ULL,       0x7050700000ULL,       0xe0a0e00000ULL,       0xc040c00000ULL, 
			0x30203000000ULL,      0x70507000000ULL,      0xe0a0e000000ULL,      0x1c141c000000ULL, 
			0x382838000000ULL,     0x705070000000ULL,     0xe0a0e0000000ULL,     0xc040c0000000ULL, 
			0x3020300000000ULL,    0x7050700000000ULL,    0xe0a0e00000000ULL,    0x1c141c00000000ULL, 
			0x38283800000000ULL,   0x70507000000000ULL,   0xe0a0e000000000ULL,   0xc040c000000000ULL, 
			0x302030000000000ULL,  0x705070000000000ULL,  0xe0a0e0000000000ULL,  0x1c141c0000000000ULL, 
			0x3828380000000000ULL, 0x7050700000000000ULL, 0xe0a0e00000000000ULL, 0xc040c00000000000ULL, 
			0x203000000000000ULL,  0x507000000000000ULL,  0xa0e000000000000ULL,  0x141c000000000000ULL, 
			0x2838000000000000ULL, 0x5070000000000000ULL, 0xa0e0000000000000ULL, 0x40c0000000000000ULL,
		};

        extern Bitboard bishop_attacks[SQ_N][bishop_attack_indices];
        extern Bitboard rook_attacks[SQ_N][rook_attack_indices];

        void print_bitboard(const Bitboard& bb);

        void init_bitboards();

        template<PieceType PT>
        inline Bitboard get_attacks(Bitboard occ, Square sq)
		{
			static_assert(PT != PAWN);
            return PT == KNIGHT ? knight_attacks[sq] 
                 : PT == BISHOP	? bishop_attacks[sq][(occ & bishop_masks[sq]) * bishop_magic[sq] >> bishop_rbits[sq]]
                 : PT == ROOK	?   rook_attacks[sq][(occ &   rook_masks[sq]) *   rook_magic[sq] >>   rook_rbits[sq]]
                 : PT == QUEEN	? get_attacks<BISHOP>(occ, sq) | get_attacks<ROOK>(occ, sq)
                 : king_attacks[sq];
        }

    } // namespace Bitboards

} // namespace Clovis
