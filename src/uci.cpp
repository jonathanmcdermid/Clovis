#include <iostream>
#include <sstream>
#include <string>

#include "uci.h"

using namespace std;

namespace Clovis {

	namespace UCI {

		const char* version_no = "Clovis v0.2";
		const char* authors = "Jonathan McDermid";
		const char* start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
		const char* promo_str = " pnbrqk  pnbrqk";

		void loop(int argc, char* argv[])
		{
			Position pos;

			pos.set(start_pos);

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
					<< "id author " << authors << endl
					<< "uciok" << endl;
				else if (token == "go")         go(pos, is);
				else if (token == "position")   position(pos, is);
				else if (token == "ucinewgame") Search::clear();
				else if (token == "isready")    cout << "readyok" << endl;
			} while (token != "quit" && argc == 1);
		}

		void go(Position& pos, std::istringstream& is)
		{
			Search::SearchLimits limits;
			string token;

			while (is >> token) {
				if (token == "wtime")			is >> limits.time[WHITE];
				else if (token == "btime")		is >> limits.time[BLACK];
				else if (token == "winc")		is >> limits.inc[WHITE];
				else if (token == "binc")		is >> limits.inc[BLACK];
				else if (token == "movestogo")	is >> limits.moves_left;
				else if (token == "depth")		is >> limits.depth;
				else if (token == "nodes")		is >> limits.nodes;
				else if (token == "movetime")	is >> limits.moveTime;
				else if (token == "mate")		is >> limits.mate;
				else if (token == "perft")		is >> limits.perft;
				else if (token == "infinite")	limits.infinite = 1;
			}

			Search::start_search(pos, limits);
		}

		void position(Position& pos, istringstream& is) 
		{
			Move m;
			string token, fen;
			is >> token;
			if (token == "startpos") {
				fen = start_pos;
				is >> token;
			}
			else if (token == "fen")
				while (is >> token && token != "moves")
					fen += token + " ";
			else
				return;

			pos.set(fen.c_str());

			while (is >> token && (m = UCI::to_move(pos, token)) != MOVE_NONE) {
				pos.do_move(m);
			}
		}

		Move to_move(const Position& pos, string& str) 
		{

			if (str.length() == 5)
				str[4] = char(tolower(str[4]));

			for (const auto& m : MoveList(pos))
				if (str == UCI::move2str(m))
					return m;

			return MOVE_NONE;
		}

		string move2str(Move m) 
		{

			Square from = move_from_sq(m);
			Square to = move_to_sq(m);

			string move = sq2str(from) + sq2str(to);

			if (move_promotion_type(m))
				move += " pnbrqk  pnbrqk"[move_promotion_type(m)];

			return move;
		}

	} // namespace UCI

} // namespace Clovis