#include "uci.h"

#include <sstream>

#include "search.h"

namespace clovis::uci {

constexpr std::string_view version_no = "Clovis III";
constexpr std::string_view authors = "Jonathan McDermid";
constexpr std::string_view start_position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void set_option(std::istringstream& is)
{
    // format for option setting is setoption name X value Y
    std::string token;
    std::string name;
    std::string value;

    // ignore "name" token
    is >> token;

    while (is >> token && token != "value") { name = token; }

    while (is >> token) { value = token; }

    if (name == "Hash") { tt.resize(std::clamp(stoi(value), 1, 10000)); }
}

// begin search
void go(Position& pos, std::istringstream& is)
{
    search::SearchLimits limits;
    std::string token;

    while (is >> token)
    {
        if (token == "wtime") { is >> limits.time[WHITE]; }
        else if (token == "btime") { is >> limits.time[BLACK]; }
        else if (token == "winc") { is >> limits.inc[WHITE]; }
        else if (token == "binc") { is >> limits.inc[BLACK]; }
        else if (token == "movestogo") { is >> limits.moves_left; }
        else if (token == "depth") { is >> limits.depth; }
        else if (token == "nodes") { is >> limits.nodes; }
        else if (token == "movetime") { is >> limits.move_time; }
        else if (token == "mate") { is >> limits.mate; }
        else if (token == "perft") { is >> limits.perft; }
        else if (token == "infinite") { limits.infinite = true; }
    }

    search::SearchInfo info;
    search::start_search(pos, limits, info);
}

// convert std::string to move if it is legal
Move to_move(const Position& pos, std::string& str)
{
    if (str.length() == 5) { str[4] = static_cast<char>(tolower(str[4])); }

    for (const auto& move : move_gen::MoveList(pos))
    {
        if (str == move2str(move)) { return move; }
    }

    return MOVE_NONE;
}

// set position to input description
void position(Position& pos, std::istringstream& is)
{
    std::string token;
    std::string fen;

    is >> token;
    if (token == "startpos")
    {
        fen = start_position;
        is >> token;
    }
    else if (token == "fen")
    {
        while (is >> token && token != "moves") { fen += token + " "; }
    }
    else { return; }

    pos.set(fen.c_str());

    while (is >> token)
    {
        const Move move = to_move(pos, token);
        if (move == MOVE_NONE) { break; }
        if (!pos.do_move(move)) { exit(EXIT_FAILURE); }
    }
}

// main loop for uci communication
void loop(const int argc, char* argv[])
{
    Position pos(start_position.data());
    std::string token;
    std::string cmd;

    for (int i = 0; i < argc; ++i) { cmd += std::string(argv[i]) + " "; }

    do {
        if (argc == 1 && !getline(std::cin, cmd)) { cmd = "quit"; }
        std::istringstream is(cmd);
        token.clear();
        is >> std::skipws >> token;
        if (token == "quit" || token == "stop") { break; }
        if (token == "go") { go(pos, is); }
        else if (token == "position") { position(pos, is); }
        else if (token == "ucinewgame") { search::clear(); }
        else if (token == "isready") { std::cout << "readyok" << '\n'; }
        else if (token == "setoption") { set_option(is); }
        else if (token == "uci")
        {
            std::cout << "id name " << version_no << '\n'
                      << "option name Hash type spin default 16 min 1 max 10000" << '\n'
                      << "option name Threads type spin default 1 min 1 max 1" << '\n'
                      << "id author " << authors << '\n'
                      << "uciok" << '\n';
        }
    } while (token != "quit" && argc == 1);
}

} // namespace clovis::uci
