#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cassert>

#include "perft.h"
#include "search.h"
#include "position.h"
#include "movelist.h"
#include "types.h"
#include "tt.h"
#include "tuner/tuner.h"

using namespace std;

namespace Clovis {

	namespace UCI {

		void loop(int argc, char* argv[]);
		void set_option(istringstream& is);
		void go(Position& pos, istringstream& is);
		void position(Position& pos, istringstream& is);
		void test();
		Move to_move(const Position& pos, string& str);

	} // namespace UCI
	
} // namespace Clovis
