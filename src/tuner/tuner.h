#pragma once

#include <thread>
#include <vector>
#include <math.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../position.h"
#include "../evaluate.h"
#include "../types.h"

namespace Clovis {

	namespace Tuner {

		struct Weight {
			Weight(short* value, bool skip, bool natural) : value(value), skip(skip), natural(natural){};
			short* value;
			bool skip;
			bool natural;
		};

		void tune();
		long double find_k();
		long double mean_squared_error(long double K);
		void processor(int start, int end, long double K);
		void map_weights_to_params();
		void print_params();
	
	} // Tuner

} // Clovis