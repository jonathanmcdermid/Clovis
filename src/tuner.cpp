#include "tuner.h"

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <thread>

namespace clovis::tuner {

using TVector = std::array<std::array<double, PHASE_N>, TI_MISC>;

std::vector<TEntry> entries;
TVector params;

constexpr int MAX_EPOCHS = 1000000;

inline double sigmoid(const double k, const double e) { return 1.0 / (1.0 + exp(-k * e / 400.0)); }

template <typename T> void add_param(T t, const TraceIndex ti)
{
    if constexpr (std::is_same<T, Score>())
    {
        assert(ti < TI_SAFETY);
        params[ti][MG] = t.mg;
        params[ti][EG] = t.eg;
    }
    else
    {
        static_assert(std::is_same<T, short>());
        assert(ti >= TI_SAFETY);
        params[ti][MG] = t;
        params[ti][EG] = 0.0;
    }
}

void init_params()
{
    using namespace eval;

    for (const auto& it : PAWN_SOURCE) { add_param<Score>(it, static_cast<TraceIndex>(&it + PAWN_PSQT - PAWN_SOURCE.data())); }
    for (const auto& it : KNIGHT_SOURCE) { add_param<Score>(it, static_cast<TraceIndex>(&it + KNIGHT_PSQT - KNIGHT_SOURCE.data())); }
    for (const auto& it : BISHOP_SOURCE) { add_param<Score>(it, static_cast<TraceIndex>(&it + BISHOP_PSQT - BISHOP_SOURCE.data())); }
    for (const auto& it : ROOK_SOURCE) { add_param<Score>(it, static_cast<TraceIndex>(&it + ROOK_PSQT - ROOK_SOURCE.data())); }
    for (const auto& it : QUEEN_SOURCE) { add_param<Score>(it, static_cast<TraceIndex>(&it + QUEEN_PSQT - QUEEN_SOURCE.data())); }
    for (const auto& it : KING_SOURCE) { add_param<Score>(it, static_cast<TraceIndex>(&it + KING_PSQT - KING_SOURCE.data())); }
    for (const auto& it : PASSED_PAWN) { add_param<Score>(it, static_cast<TraceIndex>(&it + PASSED_PAWN_PSQT - PASSED_PAWN.data())); }
    for (const auto& it : CANDIDATE_PASSER) { add_param<Score>(it, static_cast<TraceIndex>(&it + CANDIDATE_PASSER_PSQT - CANDIDATE_PASSER.data())); }
    for (const auto& it : QUIET_MOBILITY_BONUS) { add_param<Score>(it, static_cast<TraceIndex>(&it + QUIET_MOBILITY - QUIET_MOBILITY_BONUS.data())); }
    for (const auto& it : CAPTURE_MOBILITY_BONUS)
    {
        add_param<Score>(it, static_cast<TraceIndex>(&it + CAPTURE_MOBILITY - CAPTURE_MOBILITY_BONUS.data()));
    }

    add_param<Score>(DOUBLE_PAWN_PENALTY, DOUBLE_PAWN);
    add_param<Score>(ISOLATED_PAWN_PENALTY, ISOLATED_PAWN);
    add_param<Score>(BISHOP_PAIR_BONUS, BISHOP_PAIR);
    add_param<Score>(ROOK_OPEN_FILE_BONUS, ROOK_FULL);
    add_param<Score>(ROOK_SEMI_OPEN_FILE_BONUS, ROOK_SEMI);
    add_param<Score>(ROOK_CLOSED_FILE_PENALTY, ROOK_CLOSED);
    add_param<Score>(TEMPO_BONUS, TEMPO);
    add_param<Score>(KING_OPEN_PENALTY, KING_OPEN);
    add_param<Score>(KING_ADJACENT_OPEN_PENALTY, KING_ADJ_OPEN);
    add_param<Score>(KNIGHT_OUTPOST_BONUS, KNIGHT_OUTPOST);
    add_param<Score>(BISHOP_OUTPOST_BONUS, BISHOP_OUTPOST);
    add_param<Score>(WEAK_QUEEN_PENALTY, WEAK_QUEEN);
    add_param<Score>(ROOK_ON_OUR_PASSER_FILE, ROOK_OUR_PASSER);
    add_param<Score>(ROOK_ON_THEIR_PASSER_FILE, ROOK_THEIR_PASSER);
    add_param<Score>(TALL_PAWN_PENALTY, TALL_PAWN);
    add_param<Score>(FIANCHETTO_BONUS, FIANCHETTO);
    add_param<Score>(ROOK_ON_SEVENTH_RANK, ROOK_ON_SEVENTH);

    for (const auto& it : PAWN_SHIELD) { add_param<short>(it, static_cast<TraceIndex>(&it + SAFETY_PAWN_SHIELD - PAWN_SHIELD.data())); }
    for (const auto& it : INNER_RING_ATTACK) { add_param<short>(it, static_cast<TraceIndex>(&it + SAFETY_INNER_RING - INNER_RING_ATTACK.data())); }
    for (const auto& it : OUTER_RING_ATTACK) { add_param<short>(it, static_cast<TraceIndex>(&it + SAFETY_OUTER_RING - OUTER_RING_ATTACK.data())); }

    add_param<short>(VIRTUAL_MOBILITY, SAFETY_VIRTUAL_MOBILITY);
    add_param<short>(ATTACK_FACTOR, SAFETY_N_ATT);
}

double linear_eval(const TEntry& entry, TGradient* tg)
{
    std::array<double, PHASE_N> normal{};
    double safety = 0.0;
    std::array<std::array<double, COLOUR_N>, EVAL_TYPE_N> mg{};
    std::array<std::array<double, COLOUR_N>, EVAL_TYPE_N> eg{};

    for (const auto& it : entry.tuples)
    {
        const EvalType et = it.index >= TI_SAFETY ? SAFETY : NORMAL;

        mg[et][WHITE] += it.coefficient[WHITE] * params[it.index][MG];
        mg[et][BLACK] += it.coefficient[BLACK] * params[it.index][MG];
        eg[et][WHITE] += it.coefficient[WHITE] * params[it.index][EG];
        eg[et][BLACK] += it.coefficient[BLACK] * params[it.index][EG];
    }

    normal[MG] = mg[NORMAL][WHITE] - mg[NORMAL][BLACK];
    normal[EG] = eg[NORMAL][WHITE] - eg[NORMAL][BLACK];

    safety += mg[SAFETY][WHITE] * mg[SAFETY][WHITE] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[WHITE]);
    safety -= mg[SAFETY][BLACK] * mg[SAFETY][BLACK] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[BLACK]);

    const double eval = ((normal[MG] + safety) * entry.phase + normal[EG] * (MAX_GAME_PHASE - entry.phase)) / MAX_GAME_PHASE;

    if (tg)
    {
        tg->eval = eval;
        tg->safety[WHITE] = mg[SAFETY][WHITE];
        tg->safety[BLACK] = mg[SAFETY][BLACK];
    }

    return eval;
}

template <bool STATIC> double mse(const double k)
{
    const unsigned int n_threads = std::thread::hardware_concurrency();
    std::atomic<double> total(0.0);
    std::vector<std::thread> threads(n_threads);

    const std::size_t chunk_size = entries.size() / n_threads;

    auto compute_chunk = [&](std::size_t start, std::size_t end) {
        for (std::size_t i = start; i < end; ++i)
        {
            const auto& it = entries[i];
            total += std::pow(it.result - sigmoid(k, (STATIC ? it.static_eval : linear_eval(it, nullptr))), 2);
        }
    };

    for (unsigned int i = 0; i < n_threads; ++i) { threads[i] = std::thread(compute_chunk, i * chunk_size, i * chunk_size + chunk_size); }

    for (unsigned int i = 0; i < n_threads; ++i) { threads[i].join(); }

    return total / static_cast<double>(entries.size());
}

void update_single_gradient(const TEntry& entry, TVector& gradient, const double k)
{
    TGradient tg;

    const double e = linear_eval(entry, &tg);
    const double s = sigmoid(k, e);
    const double a = (entry.result - s) * s * (1 - s);

    const std::array<double, PHASE_N> base = {a * entry.phase, a * (MAX_GAME_PHASE - entry.phase)};

    for (const auto& it : entry.tuples)
    {
        if (it.index < TI_SAFETY)
        {
            gradient[it.index][MG] += base[MG] * (it.coefficient[WHITE] - it.coefficient[BLACK]);
            gradient[it.index][EG] += base[EG] * (it.coefficient[WHITE] - it.coefficient[BLACK]);
        }
        else
        {
            gradient[it.index][MG] +=
                2 * base[MG] * tg.safety[WHITE] * it.coefficient[WHITE] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[WHITE]);
            gradient[it.index][MG] -=
                2 * base[MG] * tg.safety[BLACK] * it.coefficient[BLACK] / (720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[BLACK]);
        }
    }

    gradient[SAFETY_N_ATT][MG] +=
        base[MG] * pow(tg.safety[WHITE], 2.0) * entry.n_att[WHITE] / pow(720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[WHITE], 2.0);
    gradient[SAFETY_N_ATT][MG] -=
        base[MG] * pow(tg.safety[BLACK], 2.0) * entry.n_att[BLACK] / pow(720.0 - params[SAFETY_N_ATT][MG] * entry.n_att[BLACK], 2.0);
}

TVector compute_gradient(const double k)
{
    TVector gradient{};

    for (auto& entry : entries) { update_single_gradient(entry, gradient, k); }

    return gradient;
}

void print_table(const std::string& name, const int index, const int size, const int cols)
{
    std::cout << "constexpr std::array<" << (index < TI_SAFETY ? "Score" : "short") << ", " << size << "> " << name << " = {{" << '\n';

    for (int i = 0; i < size; ++i)
    {
        if (!(i % cols)) { std::cout << '\t'; }
        if (index < TI_SAFETY) { std::cout << Score(params[index + i]) << "," << ((i % cols == (cols - 1)) ? "\n" : " "); }
        else { std::cout << round(params[index + i][MG]) << "," << ((i % cols == (cols - 1)) ? "\n" : " "); }
    }

    std::cout << "}};" << '\n' << '\n';
}

void print_params()
{
    using namespace eval;
    // TODO: print_table is broken as it doesn't print as std::arrays
    print_table("PAWN_SOURCE", PAWN_PSQT, sizeof(PAWN_SOURCE) / sizeof(Score), 4);
    print_table("KNIGHT_SOURCE", KNIGHT_PSQT, sizeof(KNIGHT_SOURCE) / sizeof(Score), 4);
    print_table("BISHOP_SOURCE", BISHOP_PSQT, sizeof(BISHOP_SOURCE) / sizeof(Score), 4);
    print_table("ROOK_SOURCE", ROOK_PSQT, sizeof(ROOK_SOURCE) / sizeof(Score), 4);
    print_table("QUEEN_SOURCE", QUEEN_PSQT, sizeof(QUEEN_SOURCE) / sizeof(Score), 4);
    print_table("KING_SOURCE", KING_PSQT, sizeof(KING_SOURCE) / sizeof(Score), 4);
    print_table("PASSED_PAWN", PASSED_PAWN_PSQT, sizeof(PASSED_PAWN) / sizeof(Score), 4);
    print_table("CANDIDATE_PASSER", CANDIDATE_PASSER_PSQT, sizeof(CANDIDATE_PASSER) / sizeof(Score), 8);
    print_table("QUIET_MOBILITY_BONUS", QUIET_MOBILITY, 7, 7);
    print_table("CAPTURE_MOBILITY_BONUS", CAPTURE_MOBILITY, 7, 7);

    std::cout << "constexpr Score DOUBLE_PAWN_PENALTY = " << DOUBLE_PAWN_PENALTY << ";" << '\n'
              << "constexpr Score ISOLATED_PAWN_PENALTY = " << ISOLATED_PAWN_PENALTY << ";" << '\n'
              << "constexpr Score BISHOP_PAIR_BONUS = " << BISHOP_PAIR_BONUS << ";" << '\n'
              << "constexpr Score ROOK_OPEN_FILE_BONUS = " << ROOK_OPEN_FILE_BONUS << ";" << '\n'
              << "constexpr Score ROOK_SEMI_OPEN_FILE_BONUS = " << ROOK_SEMI_OPEN_FILE_BONUS << ";" << '\n'
              << "constexpr Score ROOK_CLOSED_FILE_PENALTY = " << ROOK_CLOSED_FILE_PENALTY << ";" << '\n'
              << "constexpr Score TEMPO_BONUS = " << TEMPO_BONUS << ";" << '\n'
              << "constexpr Score KING_OPEN_PENALTY = " << KING_OPEN_PENALTY << ";" << '\n'
              << "constexpr Score KING_ADJACENT_OPEN_PENALTY = " << KING_ADJACENT_OPEN_PENALTY << ";" << '\n'
              << "constexpr Score KNIGHT_OUTPOST_BONUS = " << KNIGHT_OUTPOST_BONUS << ";" << '\n'
              << "constexpr Score BISHOP_OUTPOST_BONUS = " << BISHOP_OUTPOST_BONUS << ";" << '\n'
              << "constexpr Score WEAK_QUEEN_PENALTY = " << WEAK_QUEEN_PENALTY << ";" << '\n'
              << "constexpr Score ROOK_ON_OUR_PASSER_FILE = " << ROOK_ON_OUR_PASSER_FILE << ";" << '\n'
              << "constexpr Score ROOK_ON_THEIR_PASSER_FILE = " << ROOK_ON_THEIR_PASSER_FILE << ";" << '\n'
              << "constexpr Score TALL_PAWN_PENALTY = " << TALL_PAWN_PENALTY << ";" << '\n'
              << "constexpr Score FIANCHETTO_BONUS = " << FIANCHETTO_BONUS << ";" << '\n'
              << "constexpr Score ROOK_ON_SEVENTH_RANK = " << ROOK_ON_SEVENTH_RANK << ";" << '\n'
              << '\n';

    print_table("PAWN_SHIELD", SAFETY_PAWN_SHIELD, sizeof(PAWN_SHIELD) / sizeof(short), 4);
    print_table("INNER_RING_ATTACK", SAFETY_INNER_RING, 7, 7);
    print_table("OUTER_RING_ATTACK", SAFETY_OUTER_RING, 7, 7);

    std::cout << "constexpr short ATTACK_FACTOR = " << ATTACK_FACTOR << ";" << '\n'
              << "constexpr short VIRTUAL_MOBILITY = " << VIRTUAL_MOBILITY << ";" << '\n';
}

double find_k()
{
    double start = -10;
    double end = 10;
    double step = 1;
    double best = mse<true>(start);

    for (int epoch = 0; epoch < 10; ++epoch)
    {
        double curr = start;

        while (true)
        {
            if (const double error = mse<true>(curr); error <= best)
            {
                best = error;
                start = curr;
            }
            curr += step;
            if (curr > end) { break; }
        }

        std::cout.precision(17);
        std::cout << "Epoch [" << epoch << "] Error = [" << best << "], K = [" << start << "]" << '\n';

        end = start + step;
        start -= step;
        step /= 10.0;
    }

    return start;
}

void tune_eval(std::vector<std::string>& args)
{
    if (args.size() != 3)
    {
        std::cerr << "Error: Please provide the path to a .epd file as the second argument.\n";
        exit(EXIT_FAILURE);
    }

    init_params();

    TVector adaptive_gradient{};
    std::ifstream ifs(args.at(2));
    std::string line;

    while (getline(ifs, line))
    {
        if (line.empty()) { continue; }

        TEntry entry;
        memset(eval::T.data(), 0, sizeof(eval::T));
        const size_t idx = line.find('\"');
        const size_t idx_end = line.find('\"', idx + 1);

        if (const std::string res = line.substr(idx + 1, idx_end - idx - 1); res == "1-0") { entry.result = 1.0; }
        else if (res == "0-1") { entry.result = 0.0; }
        else if (res == "1/2-1/2") { entry.result = 0.5; }
        else { exit(EXIT_FAILURE); }

        Position pos(line.substr(0, idx).c_str());

        entry.phase = pos.get_game_phase();

        entry.static_eval = eval::evaluate<true>(pos);
        if (pos.side == BLACK) { entry.static_eval = -entry.static_eval; }

        entry.stm = pos.side;

        entry.n_att[WHITE] = eval::T[SAFETY_N_ATT][WHITE];
        entry.n_att[BLACK] = eval::T[SAFETY_N_ATT][BLACK];

        for (int j = 0; j < TI_N; ++j)
        {
            if ((j < TI_SAFETY && eval::T[j][WHITE] - eval::T[j][BLACK] != 0) ||
                (j >= TI_SAFETY && (eval::T[j][WHITE] != 0 || eval::T[j][BLACK] != 0)))
            {
                entry.tuples.emplace_back(j, eval::T[j][WHITE], eval::T[j][BLACK]);
            }
        }

        entries.push_back(entry);
    }

    ifs.close();

    const double k = find_k();
    double rate = 1.0;

    std::cout << mse<true>(k) << '\n';
    std::cout << mse<false>(k) << '\n';

    for (int epoch = 1; epoch < MAX_EPOCHS; ++epoch)
    {
        TVector gradient = compute_gradient(k);

        for (size_t i = 0; i < TI_MISC; ++i)
        {
            adaptive_gradient[i][MG] += pow((k / 200.0) * gradient[i][MG] / 16384, 2.0);
            adaptive_gradient[i][EG] += pow((k / 200.0) * gradient[i][EG] / 16384, 2.0);

            params[i][MG] += (k / 200.0) * (gradient[i][MG] / 16384) * (rate / sqrt(1e-8 + adaptive_gradient[i][MG]));
            params[i][EG] += (k / 200.0) * (gradient[i][EG] / 16384) * (rate / sqrt(1e-8 + adaptive_gradient[i][EG]));

            params[i][MG] = std::max(0.0, params[i][MG]);
            params[i][EG] = std::max(0.0, params[i][EG]);
        }

        if (epoch && epoch % 250 == 0) { rate = rate / 1.01; }
        if (epoch % 100 == 0) { print_params(); }

        std::cout << "Epoch [" << epoch << "] Error = [" << mse<false>(k) << "], Rate = [" << rate << "]" << '\n';
    }
}

} // namespace clovis::tuner
