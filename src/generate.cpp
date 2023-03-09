#include "generate.h"

namespace Clovis {

	namespace Generator {

		constexpr int N_RANDOM_MOVES = 12;
		constexpr int EVAL_THRESHOLD = 1000;

		void generate() 
		{
			for (int i = 0; i < 64000; ++i)
			{
				// 1. set position to startpos
				Position pos = Position(START_POS); 

				bool mate = false;

				// 2. make 12 random moves
				for (int clk = 0; clk < N_RANDOM_MOVES; ++clk)
				{
					MoveGen::MoveList ml = MoveGen::MoveList(pos);
					ml.remove_illegal(pos);

					if (!ml.size())
					{
						mate = true;
						break;
					}

					if (!pos.do_move(ml.begin()[Random::random_U64() % ml.size()]))
						exit(EXIT_FAILURE);
				}

				// 3. if pos is mate or eval is outside of threshold, return to 1
				if (mate || abs(Eval::evaluate<false>(pos)) > EVAL_THRESHOLD)
					continue;

				vector<string> fens;

				// 4. begin self play
				while (true) 
				{
					Search::SearchLimits limits;
					Move best_move, ponder_move;
					int score;
					U64 nodes;
					Search::start_search(pos, limits, best_move, ponder_move, score, nodes);

					pos.do_move(best_move);

					if (abs(score) > EVAL_THRESHOLD || pos.is_draw())
						break;

					if (pos.is_king_in_check())
						continue;

					MovePick::MovePicker mp = MovePick::MovePicker(pos, 0, MOVE_NONE, MOVE_NONE);

					if (mp.get_next(false) == MOVE_NONE)
						fens.push_back(pos.get_fen());
				}
			}
		}

	} // namespace Generator

} // namespace Clovis
