#pragma once

#include "position.h"

namespace Clovis {

	class Position;

	namespace Eval {

		extern int mg_value[7];
		extern int eg_value[7];

		void init_eval();
		void init_masks();
		Bitboard set_file_rank_mask(File file_number, Rank rank_number);
		int evaluate(const Position& pos);
		int evaluate_pawns(const Position& pos);

	} // namespace Eval

} // namespace Clovis