#include <sstream>

#include "uci.h"
#include "search.h"

using namespace std;

namespace clovis {
	
	namespace uci {

		constexpr string_view version_no = "Clovis III";
		constexpr string_view authors = "Jonathan McDermid";

		void set_option(istringstream& is) {

			// format for option setting is setoption name X value Y
			string token, name, value;

			// ignore "name" token
			is >> token;

			while (is >> token && token != "value")
				name = token;

			while (is >> token)
				value = token;

			if (name == "Hash")
				tt.resize(clamp(stoi(value), 1, 10000));
		}

		// begin search
		void go(Position& pos, istringstream& is) {

			search::SearchLimits limits;
			string token;

			while (is >> token) {
				if      (token == "wtime")		is >> limits.time[WHITE];
				else if (token == "btime")		is >> limits.time[BLACK];
				else if (token == "winc")		is >> limits.inc[WHITE];
				else if (token == "binc")		is >> limits.inc[BLACK];
				else if (token == "movestogo")	is >> limits.moves_left;
				else if (token == "depth")		is >> limits.depth;
				else if (token == "nodes")		is >> limits.nodes;
				else if (token == "movetime")	is >> limits.move_time;
				else if (token == "mate")		is >> limits.mate;
				else if (token == "perft")		is >> limits.perft;
				else if (token == "infinite")	limits.infinite = true;
			}

			search::SearchInfo info;
			search::start_search(pos, limits, info);
		}

		// convert string to move if it is legal
		Move to_move(const Position& pos, string& str) {

			if (str.length() == 5)
				str[4] = static_cast<char>(tolower(str[4]));

			for (const auto& move : move_gen::MoveList(pos))
				if (str == move2str(move))
					return move;

			return MOVE_NONE;
		}

		// set position to input description
		void position(Position& pos, istringstream& is) {

			string token, fen;

			is >> token;
			if (token == "startpos") {
				fen = START_POS;
				is >> token;
			}
			else if (token == "fen")
				while (is >> token && token != "moves")
					fen += token + " ";
			else
				return;

			pos.set(fen.c_str());
			
			while (is >> token) {
				const Move move = to_move(pos, token);
				if (move == MOVE_NONE)
					break;
				pos.do_move(move);
			}
		}

		// main loop for uci communication
		void loop(const int argc, char* argv[]) {

			Position pos(START_POS);
			string token, cmd;

			for (int i = 0; i < argc; ++i)
				cmd += string(argv[i]) + " ";

			do {
				if (argc == 1 && !getline(cin, cmd))
					cmd = "quit";
				istringstream is(cmd);
				token.clear();
				is >> skipws >> token;
				if (token == "quit" || token == "stop")	break;
				if (token == "uci")
					cout << "id name " << version_no << endl
					<< "option name Hash type spin default 16 min 1 max 10000" << endl
					<< "option name Threads type spin default 1 min 1 max 1" << endl
					<< "id author " << authors << endl
					<< "uciok" << endl;
				else if (token == "go")         go(pos, is);
				else if (token == "position")   position(pos, is);
				else if (token == "ucinewgame") search::clear();
				else if (token == "isready")    cout << "readyok" << endl;
				else if (token == "setoption")	set_option(is);
			} while (token != "quit" && argc == 1);
		}

	} // namespace uci

} // namespace clovis
