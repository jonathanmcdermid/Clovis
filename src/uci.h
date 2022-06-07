#pragma once

#include <sstream>

#include "search.h"
#include "position.h"
#include "movelist.h"
#include "types.h"

namespace Clovis {

	namespace UCI {

		void loop(int argc, char* argv[]);
		void go(Position& pos, std::istringstream& is);
		void position(Position& pos, std::istringstream& is);
		Move to_move(const Position& pos, std::string& str);
		std::string move2str(Move m);

	} // namespace UCI
	
} // namespace Clovis