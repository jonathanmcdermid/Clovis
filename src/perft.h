#pragma once

#include <string>
#include <vector>

#include "movelist.h"

namespace clovis::perft {

    struct PerftPosition {
        std::string           fen;
        std::vector<uint64_t> nodes;
    };

    void perft();

} // namespace clovis::perft
