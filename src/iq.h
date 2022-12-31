#pragma once

#include <fstream>

#include "search.h"

namespace Clovis {

	namespace IQ {

		struct IQMove {
			IQMove(PieceType pt, Square to) : pt(pt), to(to) {;}
			PieceType pt;
			Square to;
		};

		struct IQPosition {
			IQPosition(string fen, vector<IQMove> moves) : fen(fen), moves(moves) {;}
			string fen;
			vector<IQMove> moves;
		};

		void iq_test();

	} // namespace IQ

} // namespace Clovis
