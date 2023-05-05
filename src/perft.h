#pragma once

#include <string>
#include <vector>

#include "movelist.h"

namespace Clovis {

    namespace Perft {

		struct PerftPosition {
			std::string fen;
			std::vector<uint64_t> nodes;
		};

		void perft();

    } // namespace Perft

} // namespace Clovis
