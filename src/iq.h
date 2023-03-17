#pragma once

#include <fstream>

#include "parse.h"
#include "search.h"

namespace Clovis {

	namespace IQ {

		struct IQPosition {
			IQPosition(string f, vector<Move> m) : fen(f), moves(m) {;}
			string fen;
			vector<Move> moves;
		};

		void iq_test();

	} // namespace IQ

} // namespace Clovis
