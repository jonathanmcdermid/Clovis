#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cassert>

#include "search.h"
#include "position.h"
#include "movelist.h"
#include "types.h"
#include "tt.h"

namespace Clovis {

	namespace UCI {

		void loop(int argc, char* argv[]);

	} // namespace UCI
	
} // namespace Clovis
