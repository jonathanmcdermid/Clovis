#include <random>
#include "random.h"
#include <ctime>

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
			std::srand(std::time(0));
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<std::uintmax_t> dis;
			return dis(gen);

			//return	((U64)(random_U32())	& 0xFFFF)			| 
			//		(((U64)(random_U32())	& 0xFFFF) << 16)	| 
			//		(((U64)(random_U32())	& 0xFFFF) << 32)	| 
			//		(((U64)(random_U32())	& 0xFFFF) << 48);
		}

	} // namespace Random

} // namespace Clovis