#include "random.h"

namespace Clovis {

	namespace Random {

		U64 random_U64()
		{
			std::srand(std::time(0));
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<std::uintmax_t> dis;
			return dis(gen);
		}

	} // namespace Random

} // namespace Clovis