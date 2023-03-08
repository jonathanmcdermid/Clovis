#pragma once

#include "uci.h"
#include "perft.h"
#include "tuner.h"
#include "benchmark.h"
#include "iq.h"
#include "generate.h"

using namespace std;

namespace Clovis {

	namespace CMDLine {

		void handle_cmd(int argc, char* argv[]);

	} // namespace Bench

} // namespace Clovis
