#pragma once

#include "position.h"

namespace Clovis {

	class Position;

	namespace Eval {

		void init_eval();

		int evaluate(const Position& pos);

	} // namespace Eval

} // namespace Clovis