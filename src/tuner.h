#pragma once

#include <thread>
#include <vector>
#include <math.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

#include "position.h"
#include "evaluate.h"
#include "types.h"

namespace Clovis {

	namespace Tuner {

		void tune_eval();
		void tune_search();
		long double tune_loop();
		long double find_k();
		long double mean_squared_error(long double K);
		void processor(int start, int end, long double K);
		void map_weights_to_params();
		void print_params();
	
	} // Tuner

} // Clovis
