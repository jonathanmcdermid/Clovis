#pragma once

#include <string>

#include "movelist.h"

namespace Clovis {

    namespace Perft {

		struct PerftPosition {
			std::string fen;
			std::vector<U64> nodes;
		};

		void perft();

    } // namespace Perft

} // namespace Clovis
