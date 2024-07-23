#pragma once

#include <string>
#include <vector>

#include "movelist.hpp"

namespace clovis::perft {

struct PerftPosition
{
    std::string fen;
    std::vector<uint64_t> nodes;
};

void perft(const std::vector<std::string>& args);

} // namespace clovis::perft
