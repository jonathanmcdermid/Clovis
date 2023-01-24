#pragma once

#include <fstream>

#include "search.h"

namespace Clovis {

	namespace IQ {

		struct IQMove {
			IQMove(PieceType p, Square t) : pt(p), to(t) {;}
			PieceType pt;
			Square to;
		};

		struct IQPosition {
			IQPosition(string f, vector<IQMove> m) : fen(f), moves(m) {;}
			string fen;
			vector<IQMove> moves;
		};

		void iq_test();

	} // namespace IQ

} // namespace Clovis
