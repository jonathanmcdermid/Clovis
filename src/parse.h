#pragma once

#include <string>

#include "search.h"

namespace Clovis {

	namespace Parse {
		
		Move parse(const Position& pos, std::string move);
		void generate_data();

	}

} // namespace Clovis
