#pragma once

#include <thread>
#include <vector>
#include <math.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include "position.h"
#include "evaluate.h"
#include "types.h"
#include "benchmark.h"
#include "search.h"

namespace Clovis {

	namespace Tuner {
	
		struct TTuple {
			TTuple(int i, int wc, int bc) { 
				index = i; 
				coefficient[WHITE] = wc; 
				coefficient[BLACK] = bc; 
			}
			int index;
			int coefficient[COLOUR_N];
		};
		
		struct TGradient {
			double eval;
			double safety[COLOUR_N];
		};
		
		struct TEntry {
			Colour stm;
			int seval;
			int safety[COLOUR_N];
			double result;
			double phase;
			vector<TTuple> tuples;
		};

		void tune_eval();

	} // namespace Tuner

} // namespace Clovis
