#include "random.h"

using namespace std;

namespace Clovis {

	namespace Random {
		
		static mt19937_64 rng;
		
		U64 random_U64() { return rng(); }

		void seed_random(U64 seed) { rng.seed(seed); }

	} // namespace Random

} // namespace Clovis
