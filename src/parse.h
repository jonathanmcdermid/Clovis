#pragma once

#include <string>
#include <fstream>

#include "search.h"
#include "position.h"
#include "types.h"
#include "random.h"

namespace Clovis {

	namespace Parse {
		
		Move parse(const Position& pos, string s);
		void generate_dataset();

	}

} // namespace Clovis
