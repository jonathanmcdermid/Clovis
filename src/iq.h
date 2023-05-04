#pragma once

#include "parse.h"

namespace Clovis {

	namespace IQ {

		struct IQPosition {
			std::string fen;
			std::vector<Move> moves;
		};

		void iq_test();

	} // namespace IQ

} // namespace Clovis
