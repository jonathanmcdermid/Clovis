#include <fstream>
#include <sstream>
#include <vector>

#include "clovis/engine/evaluate.hpp"
#include "clovis/engine/movepicker.hpp"
#include "clovis/engine/search.hpp"

int main(const int argc, char* argv[])
{
    clovis::bitboards::init_bitboards();

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i) { args.emplace_back(argv[i]); }

    std::ifstream ifs(args[1]);
    std::ofstream ofs(args[2]);
    std::string line;
    std::string result;
    std::string fen;

    while (!ifs.eof())
    {
        while (getline(ifs, line))
        {
            if (line.find("Result") != std::string::npos)
            {
                size_t start = line.find('\"') + 1;
                size_t end = line.rfind('\"');
                result = line.substr(start, end - start);
                break;
            }
        }

        while (getline(ifs, line))
        {
            if (line.find("FEN") != std::string::npos)
            {
                size_t start = line.find('\"') + 1;
                size_t end = line.rfind('\"');
                fen = line.substr(start, end - start);
                break;
            }
        }

        clovis::Position pos(fen.c_str());

        while (getline(ifs, line))
        {
            if (line.find(std::to_string(pos.get_full_move_clock()) + "... ") != std::string::npos ||
                line.find(std::to_string(pos.get_full_move_clock()) + ". ") != std::string::npos)
            {
                break;
            }
        }

        bool live = true;
        std::vector<clovis::Key> keys;

        while (true)
        {
            std::istringstream ss(line);
            while (true)
            {
                std::string token;
                ss >> std::skipws >> token;
                if (token.empty()) { break; }
                if (token == result)
                {
                    live = false;
                    break;
                }
                if (token.find('.') == std::string::npos)
                {
                    if (!pos.do_move(pos.parse(token))) { exit(EXIT_FAILURE); }

                    if (pos.get_full_move_clock() > 8 && token[token.length() - 1] != '#' && token[token.length() - 1] != '+')
                    {
                        clovis::search::SearchLimits limits;
                        limits.depth = 1;
                        clovis::search::SearchInfo info;
                        clovis::search::start_search(pos, limits, info);

                        if (info.score < clovis::MIN_CHECKMATE_SCORE && info.score > -clovis::MIN_CHECKMATE_SCORE)
                        {
                            for (const auto& it : info.pv_line)
                            {
                                if (!pos.do_move(it)) { exit(EXIT_FAILURE); }
                            }

                            if (std::find(keys.begin(), keys.end(), pos.get_key()) == keys.end())
                            {
                                if (const int eval = pos.get_side() == clovis::WHITE ? clovis::eval::evaluate<false>(pos) : -clovis::eval::evaluate<false>(pos);
                                    (result == "1-0" && eval > -500) || (result == "0-1" && eval < 500) ||
                                    (result == "1/2-1/2" && (eval > -500 && eval < 500)))
                                {
                                    keys.push_back(pos.get_key());
                                    ofs << pos.get_fen() + " \"" + result + "\";" << '\n';
                                }
                            }

                            std::for_each(std::make_reverse_iterator(info.pv_line.last), std::make_reverse_iterator(info.pv_line.moves.data()),
                                          [&](const clovis::Move& m) { pos.undo_move(m); });
                        }
                    }
                }
            }
            if (!live || !getline(ifs, line)) { break; }
        }
    }

    ifs.close();
    ofs.close();

    return EXIT_SUCCESS;
}
