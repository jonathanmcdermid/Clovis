#pragma once

#include <vector>

#include "parse.h"

namespace clovis::iq {

struct IQPosition
{
    std::string fen;
    std::vector<Move> moves;
};

void iq_test();

} // namespace clovis::iq
