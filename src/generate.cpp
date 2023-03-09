#include "generate.h"

namespace Clovis {

	namespace Generator {

		constexpr int N_MATCHES = 1000;
		constexpr int N_RANDOM_MOVES = 12;
		constexpr int EVAL_THRESHOLD = 1000;

		void generate() 
		{
			ofstream out("src/tuner.epd");

			for (int i = 0; i < N_MATCHES; ++i)
			{
				// 1. set position to startpos
				Position pos = Position(START_POS); 

				bool mate = false;

				// 2. make random moves
				for (int ply = 0; ply < N_RANDOM_MOVES; ++ply)
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

				int score;

				// 4. begin self play
				while (true) 
				{
					Search::SearchLimits limits;
					Move best_move, ponder_move;
					U64 nodes;
					Search::start_search(pos, limits, best_move, ponder_move, score, nodes);

					if (best_move == MOVE_NONE)
						break;

					pos.do_move(best_move);

					if (abs(score) > EVAL_THRESHOLD || pos.is_draw())
						break;

					if (pos.is_king_in_check())
						continue;

					MovePick::MovePicker mp = MovePick::MovePicker(pos, 0, MOVE_NONE, MOVE_NONE);

					if (mp.get_next(false) == MOVE_NONE)
						fens.push_back(pos.get_fen());
				}

				string result;

				if (pos.is_draw())
					result = " \"1/2-1/2\";";
				else if ((score > EVAL_THRESHOLD && pos.side == BLACK) || (score < -EVAL_THRESHOLD && pos.side == WHITE))
					result = " \"1-0\";";
				else if ((score > EVAL_THRESHOLD && pos.side == WHITE) || (score < -EVAL_THRESHOLD && pos.side == BLACK))
					result = " \"0-1\";";
				else
					exit(EXIT_FAILURE);

				// 5. record result
				for (auto& it : fens)
					out << it << result << endl;
			}

			out.close();
		}

	} // namespace Generator

} // namespace Clovis
