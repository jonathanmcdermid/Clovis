#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "position.h"
#include "movelist.h"
#include "time.h"
#include "movepicker.h"

namespace Clovis {

    namespace Perft {

		struct PerftPosition {
			PerftPosition(string s, vector<U64> nodes) : s(s), nodes(nodes) { ; }
			string s;
			vector<U64> nodes;
		};

		void perft(Position& pos, int depth, U64& nodes);
		void perft_control();

    } // namespace Perft

} // namespace Clovis
