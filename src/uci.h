#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include "search.h"
#include "position.h"
#include "movelist.h"
#include "types.h"
#include "tt.h"
#include "tuner/tuner.h"

namespace Clovis {

	namespace UCI {

		void loop(int argc, char* argv[]);
		void set_option(std::istringstream& is);
		void go(Position& pos, std::istringstream& is);
		void position(Position& pos, std::istringstream& is);
		void local(Position& pos, std::istringstream& is);
		Move to_move(const Position& pos, std::string& str);
		std::string move2str(Move m);

	} // namespace UCI
	
} // namespace Clovis