#include "uci.h"

using namespace std;

namespace Clovis {
	
	namespace UCI {

		const char* version_no = "Clovis v1.8";
		const char* authors = "Jonathan McDermid";

		// main loop for UCI communication
		void loop(int argc, char* argv[])
		{
			Position pos(START_POS);

			string token;
			string cmd;

			do {
				if (argc == 1 && !getline(cin, cmd))
					cmd = "quit";
				std::istringstream is(cmd);
				token.clear();
				is >> skipws >> token;
				if (token == "quit" || token == "stop")	break;
				else if (token == "uci")
					cout << "id name " << version_no << endl
					<< "option name Hash type spin default 16 min 1 max 10000" << endl
					<< "option name Threads type spin default 1 min 1 max 1" << endl
					<< "id author " << authors << endl
					<< "uciok" << endl;
				else if (token == "go")         go(pos, is);
				else if (token == "position")   position(pos, is);
				else if (token == "ucinewgame") Search::clear();
				else if (token == "isready")    cout << "readyok" << endl;
				else if (token == "tune")		Tuner::tune();
				else if (token == "setoption")	set_option(is);
				else if (token == "perft")		Perft::test_perft();
				else if (token == "test")		test();
			} while (token != "quit" && argc == 1);
		}

		void set_option(std::istringstream& is)
		{
			// format for option setting is setoption name X value Y
			string token, name, value;

			// ignore "name" token
			is >> token;

			while (is >> token && token != "value")
				name = token;

			while (is >> token)
				value = token;

			if (name == "Hash")
			{
				int mb = stoi(value);
				mb = max(mb, 1);
				mb = min(mb, 10000);
				tt.resize(mb);
			}

			if (name == "Threads")
			{
				return;
			}
		}

		// begin search
		void go(Position& pos, std::istringstream& is)
		{
			Search::SearchLimits limits;
			string token;

			while (is >> token) 
			{
				if (token == "wtime")			is >> limits.time[WHITE];
				else if (token == "btime")		is >> limits.time[BLACK];
				else if (token == "winc")		is >> limits.inc[WHITE];
				else if (token == "binc")		is >> limits.inc[BLACK];
				else if (token == "movestogo")	is >> limits.moves_left;
				else if (token == "depth")		is >> limits.depth;
				else if (token == "nodes")		is >> limits.nodes;
				else if (token == "movetime")	is >> limits.move_time;
				else if (token == "mate")		is >> limits.mate;
				else if (token == "perft")		is >> limits.perft;
				else if (token == "infinite")	limits.infinite = 1;
			}

			Move best_move, ponder_move;
			int score;
			U64 nodes;
			Search::start_search(pos, limits, best_move, ponder_move, score, nodes);
		}

		// set position to input description
		void position(Position& pos, istringstream& is) 
		{
			Move move;
			string token, fen;
			is >> token;
			if (token == "startpos") 
			{
				fen = START_POS;
				is >> token;
			}
			else if (token == "fen")
				while (is >> token && token != "moves")
					fen += token + " ";
			else
				return;

			pos.set(fen.c_str());

			while (is >> token && (move = UCI::to_move(pos, token)) != MOVE_NONE) 
				pos.do_move(move);

			pos.print_position();
		}

		void test()
		{
			cout << "Tests will only work if NDEBUG line is commented out in types.h!" << endl;
			MovePick::test_movepicker(); 
			Eval::test_eval();
		}

		// convert string to move if it is legal
		Move to_move(const Position& pos, string& str) 
		{
			if (str.length() == 5)
				str[4] = char(tolower(str[4]));

			for (const auto& move : MoveGen::MoveList(pos))
				if (str == move2str(move))
					return move;

			return MOVE_NONE;
		}

	} // namespace UCI

} // namespace Clovis
