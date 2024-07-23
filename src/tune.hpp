#pragma once

#include <string>
#include <vector>

#include "evaluate.hpp"

namespace clovis::tune {

struct TTuple
{
    constexpr TTuple(const int i, const int wc, const int bc) : index(i), coefficient{wc, bc} {}

    int index;
    std::array<int, 2> coefficient;
};

struct TGradient
{
    constexpr TGradient() = default;

    double eval{0.0};
    std::array<double, 2> safety{0.0};
};

struct TEntry
{
    TEntry() = default;

    Colour stm{WHITE};
    int static_eval{0};
    double result{0.0};
    double phase{0.0};
    std::array<int, 2> safety{0};
    std::array<int, 2> n_att{0};
    std::vector<TTuple> tuples;
};

void tune_eval(const std::vector<std::string>& args);

} // namespace clovis::tune
