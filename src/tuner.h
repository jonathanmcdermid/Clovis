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
			TTuple(int i, int wc, int bc) : index(i), coefficient{ wc, bc } {}
			int index, coefficient[COLOUR_N];
		};
		
		struct TGradient {
			TGradient() : eval(0), safety{ 0.0 } {}
			double eval, safety[COLOUR_N];
		};
		
		struct TEntry {
			TEntry() : stm(WHITE), result(0.0), phase(0.0), seval(0), safety{ 0 }, n_att{ 0 } {}
			Colour stm;
			double result, phase;
			int seval, safety[COLOUR_N], n_att[COLOUR_N];
			std::vector<TTuple> tuples;
		};

		void tune_eval();

	} // namespace Tuner

} // namespace Clovis
