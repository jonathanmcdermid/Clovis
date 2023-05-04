#pragma once

#include <string>
#include <fstream>

#include "search.h"

namespace Clovis {

	namespace Parse {
		
		Move parse(const Position& pos, std::string move);
		void generate_dataset();

	}

} // namespace Clovis
