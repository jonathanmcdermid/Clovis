#pragma once

#include <vector>

#include "evaluate.h"

namespace Clovis {

	namespace Tuner {
	
        struct TTuple {
            constexpr TTuple(const int i, const int wc, const int bc) : index(i), coefficient{ wc, bc } {}

            int index;
            std::array<int, COLOUR_N> coefficient;
        };

        struct TGradient {
            constexpr TGradient() = default;

            double eval{ 0.0 };
            std::array<double, COLOUR_N> safety{ 0.0 };
        };

        struct TEntry {
            TEntry() = default;

            Colour stm{ WHITE };
            double result{ 0.0 }, phase{ 0.0 };
            int seval{ 0 };
            std::array<int, COLOUR_N> safety{ 0 }, n_att{ 0 };
            std::vector<TTuple> tuples;
        };

        void tune_eval();

	} // namespace Tuner

} // namespace Clovis
