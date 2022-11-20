#pragma once

#include <iostream>
#include <string>

#include "position.h"
#include "movelist.h"
#include "time.h"
#include "movepicker.h"

namespace Clovis {

    namespace Perft {

		struct perft_position {
			perft_position(string s, vector<U64> nodes) : s(s), nodes(nodes) { ; }
			string s;
			vector<U64> nodes;
		};

		void perft(Position& pos, int depth, U64& nodes);
		void test_perft();

    } // namespace Perft

} // namespace Clovis