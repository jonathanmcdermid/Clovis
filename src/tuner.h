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
            constexpr TTuple(int i, int wc, int bc) : index(i), coefficient{ wc, bc } {}

            int index;
            std::array<int, COLOUR_N> coefficient;
        };

        struct TGradient {
            constexpr TGradient() = default;

            double eval{ 0.0 };
            std::array<double, COLOUR_N> safety{ 0.0 };
        };

        struct TEntry {
            constexpr TEntry() = default;

            Colour stm{ WHITE };
            double result{ 0.0 }, phase{ 0.0 };
            int seval{ 0 };
            std::array<int, COLOUR_N> safety{ 0 }, n_att{ 0 };
            std::vector<TTuple> tuples;
        };

        void tune_eval();

	} // namespace Tuner

} // namespace Clovis
