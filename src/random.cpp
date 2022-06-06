#include "random.h"

namespace Clovis {

	namespace Random {

		// pseudo-random number state
		U32 state = 1804289383;

		// XORSHIFT32 pseudo-random number generator
		U32 random_U32()
		{
			state ^= state << 13;
			state ^= state >> 17;
			state ^= state << 5;
			return state;
		}

		// XORSHIFT32 extension for pseudo-random 64 bit numbers
		U64 random_U64()
		{
			return	((U64)(random_U32())	& 0xFFFF)			| 
					(((U64)(random_U32())	& 0xFFFF) << 16)	| 
					(((U64)(random_U32())	& 0xFFFF) << 32)	| 
					(((U64)(random_U32())	& 0xFFFF) << 48);
		}

	} // namespace Random

} // namespace Clovis