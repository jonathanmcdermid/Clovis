#pragma once

#include <string>

#include "search.h"

namespace clovis::parse {

Move parse(const Position &pos, std::string move);
void generate_data();

} // namespace clovis::parse
