#include "random.h"

using namespace std;

namespace Clovis {

	namespace Random {

		U64 random_U64()
		{
			srand(0);//time(0));
			random_device rd;
			mt19937_64 gen(rd());
			uniform_int_distribution<uintmax_t> dis;
			return dis(gen);
		}

	} // namespace Random

} // namespace Clovis