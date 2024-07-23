#include <fstream>
#include <sstream>
#include <vector>

#include "evaluate.hpp"
#include "move_selector.hpp"
#include "search.hpp"

int main(const int argc, char* argv[])
{
    clovis::bitboards::init_bitboards();

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; ++i) { args.emplace_back(argv[i]); }

    if (argc < 3)
    {
        std::cerr << "Usage: " << args[0] << " <input_file> <output_file> [number_of_positons]\n";
        return EXIT_FAILURE;
    }

    clovis::bitboards::init_bitboards();

    std::ifstream ifs(args[1]);
    if (!ifs.is_open())
    {
        std::cerr << "Error opening input file: " << args[1] << '\n';
        return EXIT_FAILURE;
    }

    std::ofstream ofs(args[2]);
    if (!ofs.is_open())
    {
        std::cerr << "Error opening output file: " << args[2] << '\n';
        return EXIT_FAILURE;
    }

    std::size_t number_of_positions = argc == 4 ? static_cast<std::size_t>(std::stoi(argv[3])) : std ::numeric_limits<std::size_t>::max();
    std::string line;
    std::string result;
    std::string fen;
    int counter = 0;

    while (counter++ < number_of_positions && !ifs.eof())
    {
        std::vector<clovis::Key> keys;

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

                    // dont take positions if they are too early in the game or if there is a checkmate within 10 moves
                    if (pos.get_full_move_clock() > 8 && token[token.length() - 1] != '#' && token[token.length() - 1] != '+')
                    {
                        if (std::find(keys.begin(), keys.end(), pos.get_key()) != keys.end()) { continue; }

                        clovis::search::SearchLimits limits;
                        limits.depth = 1;
                        clovis::search::SearchInfo info;
                        clovis::search::start_search(pos, limits, info);
                        // it would be better to check if the next token is the result (a draw) to determine if this position has no moves
                        clovis::move_gen::MoveList ml(pos);
                        ml.remove_illegal(pos);

                        if (ml.size() == 0) { continue; }

                        // if there is a checkmate on the principal variation, we ignore this position
                        if (info.score < clovis::MIN_CHECKMATE_SCORE && info.score > -clovis::MIN_CHECKMATE_SCORE)
                        {
                            for (const auto& it : info.pv_line)
                            {
                                if (!pos.do_move(it)) { exit(EXIT_FAILURE); }
                            }

                            if (std::find(keys.begin(), keys.end(), pos.get_key()) == keys.end())
                            {
                                // if the evaluation is completely different from the result of the game, we will ignore this position
                                int eval = pos.get_side() == clovis::WHITE ? clovis::eval::evaluate<false>(pos) : -clovis::eval::evaluate<false>(pos);
                                if ((result == "1-0" && eval > -500) || (result == "0-1" && eval < 500) ||
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

    return EXIT_SUCCESS;
}
