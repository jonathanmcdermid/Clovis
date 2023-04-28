#pragma once

#include <string>
#include <fstream>

#include "search.h"
#include "position.h"
#include "types.h"
#include "random.h"

namespace Clovis {

	namespace Parse {
		
		Move parse(const Position& pos, std::string s);
		void generate_dataset();

	}

} // namespace Clovis
