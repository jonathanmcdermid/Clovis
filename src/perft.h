#pragma once

#include <string>
#include <vector>

#include "movelist.h"

namespace clovis {

    namespace perft {

		struct PerftPosition {
			std::string fen;
			std::vector<uint64_t> nodes;
		};

		void perft();

    } // namespace perft

} // namespace clovis
