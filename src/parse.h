#pragma once

#include <string>
#include <fstream>

#include "position.h"
#include "types.h"

using namespace std;

namespace Clovis {

	namespace Parse {
		
		Move parse(const Position& pos, string s);
		void generate_dataset();

	}

} // namespace Clovis
