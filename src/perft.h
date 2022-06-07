#pragma once

#include <iostream>
#include <string>

#include "position.h"
#include "movelist.h"
#include "time.h"

namespace Clovis {

    namespace Perft {

		void perft(int depth);
		void perft_control();

    } // namespace Perft

} // namespace Clovis