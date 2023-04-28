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
		void set_option(std::istringstream& is);
		void go(Position& pos, std::istringstream& is);
		void position(Position& pos, std::istringstream& is);
		Move to_move(const Position& pos, std::string& str);

	} // namespace UCI
	
} // namespace Clovis
