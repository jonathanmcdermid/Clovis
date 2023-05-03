#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <chrono>

#include "position.h"
#include "movelist.h"
#include "time.h"
#include "movepicker.h"

namespace Clovis {

    namespace Perft {

		struct PerftPosition {
			std::string fen;
			std::vector<U64> nodes;
		};

		void perft();

    } // namespace Perft

} // namespace Clovis
