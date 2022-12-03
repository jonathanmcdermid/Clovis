#pragma once

#include <random>

#include "types.h"

namespace Clovis {

	namespace Random {

		void seed_random(U64 seed);
		U64 random_U64();

	} // namespace Random

} // namespace Clovis
