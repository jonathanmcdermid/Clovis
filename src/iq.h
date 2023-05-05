#pragma once

#include <vector>

#include "parse.h"

namespace clovis {

	namespace iq {

		struct IQPosition {
			std::string fen;
			std::vector<Move> moves;
		};

		void iq_test();

	} // namespace iq

} // namespace clovis
