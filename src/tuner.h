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
#include "benchmark.h"
#include "search.h"

namespace Clovis {

	namespace Tuner {
	
		struct TTuple {
			int index;
			int coefficient[COLOUR_N];
		};
		
		struct TEntry {
			Colour stm;
			int seval;
			int eval;
			int safety[COLOUR_N];
			double result;
			double rho[PHASE_N];
			vector<TTuple> tuples;
		};

		void tune_eval();
		long double find_k();
		long double mean_squared_error(long double K);

	} // namespace Tuner

} // namespace Clovis
