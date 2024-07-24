#include "search.hpp"

#include <chrono>
#include <climits>
#include <iomanip>

#include "evaluate.hpp"

namespace clovis::search {

constexpr std::array<double, MAX_PLY + 1> LOG_TABLE = {
    0.000000, 0.000000, 0.693147, 1.098612, 1.386294, 1.609438, 1.791759, 1.945910, 2.079442, 2.197225, 2.302585, 2.397895, 2.484907,
    2.564949, 2.639057, 2.708050, 2.772589, 2.833213, 2.890372, 2.944439, 2.995732, 3.044522, 3.091042, 3.135494, 3.178054, 3.218876,
    3.258097, 3.295837, 3.332205, 3.367296, 3.401197, 3.433987, 3.465736, 3.496508, 3.526361, 3.555348, 3.583519, 3.610918, 3.637586,
    3.663562, 3.688879, 3.713572, 3.737670, 3.761200, 3.784190, 3.806662, 3.828641, 3.850148, 3.871201, 3.891820, 3.912023, 3.931826,
    3.951244, 3.970292, 3.988984, 4.007333, 4.025352, 4.043051, 4.060443, 4.077537, 4.094345, 4.110874, 4.127134, 4.143135, 4.158883};

constexpr int LMR_DEPTH = 2;
constexpr int LMR_HISTORY_MIN = 2;
constexpr int LMR_HISTORY_MAX = 2;
constexpr int LMR_HISTORY_DIVISOR = 4000;
constexpr int LMR_REDUCTION = 2;

constexpr auto LMR_TABLE = [] {
    std::array<std::array<int, 64>, MAX_PLY + 1> arr{};

    for (int depth = 1; depth <= MAX_PLY; ++depth)
    {
        for (int ordered = 1; ordered < 64; ++ordered)
        {
            arr[depth][ordered] = static_cast<int>(0.75 + LOG_TABLE[depth] * LOG_TABLE[ordered] / 2.25);
        }
    }

    return arr;
}();

constexpr std::array<int, 2> IID_DEPTH = {5, 7};
constexpr std::array<int, 2> IID_REDUCTION = {5, 4};
constexpr std::array<int, 2> IID_FACTOR = {1, 4};
constexpr std::array<int, 2> IID_DIVISOR = {2, 4};

constexpr auto IID_TABLE = [] {
    std::array<std::array<int, MAX_PLY + 1>, 2> arr{};

    for (int depth = 1; depth <= MAX_PLY; ++depth)
    {
        for (int i = 0; i < 2; ++i) { arr[i][depth] = (IID_FACTOR[i] * depth - IID_REDUCTION[i]) / (IID_DIVISOR[i] + 1); }
    }

    return arr;
}();

constexpr int FUTILITY_PRUNING_FACTOR = 75;
constexpr int FUTILITY_PRUNING_DEPTH = 8;

constexpr int NULL_MOVE_DEPTH = 3;
constexpr int NULL_MOVE_REDUCTION = 3;

constexpr int ASPIRATION_DEPTH = 3;
constexpr int DELTA = 45;

constexpr int DRAW_SCORE = 0;

bool stop = false;

std::chrono::steady_clock::time_point start_time;
int64_t allocated_time;

// reset transposition table, set search to standard conditions
void clear()
{
    move_selector::clear();
    tt.clear();
}

template <NodeType N> int quiescence(Position& pos, int alpha, int beta, uint64_t& nodes, const int ply, Line& pv_line)
{
    constexpr bool PV_NODE = N == NodeType::ROOT || N == NodeType::PV;

    assert(PV_NODE || (alpha == beta - 1));

    ++nodes;

    if (pos.is_draw()) { return DRAW_SCORE; }
    if (ply >= MAX_PLY) { return eval::evaluate<false>(pos); }

    const auto tte = tt.probe(pos.get_key());

    if (!PV_NODE && tte.key == pos.get_key() &&
        (tte.flags == HashFlag::EXACT || (tte.flags == HashFlag::BETA && tte.eval >= beta) || (tte.flags == HashFlag::ALPHA && tte.eval <= alpha)))
    {
        return tte.eval;
    }

    const bool in_check = pos.is_king_in_check();
    const int old_alpha = alpha;

    if (!in_check)
    {
        int eval = eval::evaluate<false>(pos);

        // use TT score instead of static eval if valid TTE and either
        // 1. alpha flag + lower tt score than static eval
        // 2. beta flag + higher tt score than static eval
        if (tte.key == pos.get_key() && ((tte.flags == HashFlag::ALPHA) == (tte.eval < eval))) { eval = tte.eval; }
        if (eval >= beta) { return beta; }
        if (eval > alpha) { alpha = eval; }
    }

    move_selector::MoveSelector mp(pos, 0, MOVE_NONE, (tte.key == pos.get_key()) ? tte.move : MOVE_NONE);
    Move best_move = MOVE_NONE;
    int best_eval = INT_MIN;

    while (const Move curr_move = mp.get_next(in_check))
    {
        // illegal move or non capture
        if (!pos.do_move(curr_move)) { continue; }

        Line line;
        int eval = -quiescence<N>(pos, -beta, -alpha, nodes, ply + 1, line);

        pos.undo_move(curr_move);

        // fail high
        if (eval >= beta)
        {
            tt.new_entry(pos.get_key(), 0, beta, HashFlag::BETA, curr_move);
            return beta;
        }
        if (eval > best_eval)
        {
            best_eval = eval;
            best_move = curr_move;
            // new best move found
            if (eval > alpha)
            {
                if constexpr (PV_NODE)
                {
                    pv_line.last = pv_line.moves.data();
                    *pv_line.last++ = curr_move;
                    for (const auto& m : line) { *pv_line.last++ = m; }
                }
                alpha = eval;
            }
        }
    }

    if (in_check && best_eval == INT_MIN) { return ply - CHECKMATE_SCORE; }

    tt.new_entry(pos.get_key(), 0, alpha, alpha > old_alpha ? HashFlag::EXACT : HashFlag::ALPHA, best_move);

    return alpha;
}

template <NodeType N> int negamax(Position& pos, int alpha, int beta, int depth, const int ply, const Move prev_move, uint64_t& nodes, Line& pv_line)
{
    constexpr bool NULL_NODE = N == NodeType::NULL_MOVE;
    constexpr bool ROOT_NODE = N == NodeType::ROOT;
    constexpr bool PV_NODE = N == NodeType::ROOT || N == NodeType::PV;

    assert(PV_NODE || (alpha == beta - 1));

    if (nodes & 2047 && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count() > allocated_time)
    {
        stop = true;
        return 0;
    }

    if (depth <= 0) { return quiescence<N>(pos, alpha, beta, nodes, ply, pv_line); }

    ++nodes;

    if (ply >= MAX_PLY) { return eval::evaluate<false>(pos); }

    // mate distance pruning
    // if me have found a mate, no point in finding a longer mate
    alpha = std::max(alpha, -CHECKMATE_SCORE + ply);
    beta = std::min(beta, CHECKMATE_SCORE - ply + 1);

    if (alpha >= beta) { return alpha; }

    auto tte = tt.probe(pos.get_key());

    if (tte.key == pos.get_key())
    {
        if (PV_NODE && tte.move != MOVE_NONE)
        {
            assert(pline.last == pline.moves.data());
            *pv_line.last++ = tte.move;
        }
        if (!PV_NODE && tte.depth >= depth &&
            (tte.flags == HashFlag::EXACT || (tte.flags == HashFlag::BETA && tte.eval >= beta) ||
             (tte.flags == HashFlag::ALPHA && tte.eval <= alpha)))
        {
            return tte.eval;
        }
    }

    const bool in_check = pos.is_king_in_check();

    if (!in_check)
    {
        int score = tte.key == pos.get_key() ? tte.eval : eval::evaluate<false>(pos);

        // reverse futility pruning
        // if evaluation is above a certain threshold, we can trust that it will maintain it in the future
        if (!PV_NODE && depth <= FUTILITY_PRUNING_DEPTH && score - depth * FUTILITY_PRUNING_FACTOR > beta) { return score; }

        // null move pruning
        if (!PV_NODE && !NULL_NODE && depth >= NULL_MOVE_DEPTH && pos.is_stm_major())
        {
            pos.do_null_move();
            Line line;
            score = -negamax<NodeType::NULL_MOVE>(pos, -beta, -beta + 1, depth - NULL_MOVE_REDUCTION, ply + 1, MOVE_NULL, nodes, line);
            pos.undo_null_move();

            if (score >= beta) { return beta; }
        }

        // internal iterative deepening
        if (tte.key != pos.get_key() && depth >= IID_DEPTH[PV_NODE])
        {
            Line line;
            negamax<N == NodeType::NULL_MOVE ? NodeType::NON_PV : N>(pos, alpha, beta, IID_TABLE[PV_NODE][depth], ply, prev_move, nodes, line);
            tte = tt.probe(pos.get_key());

            if (tte.key == pos.get_key())
            {
                if constexpr (PV_NODE)
                {
                    assert(pline.last == pline.moves.data());
                    *pv_line.last++ = tte.move;
                }
            }
        }
    }
    else { ++depth; }

    move_selector::MoveSelector mp(pos, ply, prev_move, tte.key == pos.get_key() ? tte.move : MOVE_NONE);
    Move best_move = MOVE_NONE;
    int best_score = INT_MIN;
    int moves_searched = 0;
    auto hash_flag = HashFlag::ALPHA;
    bool play_quiets = true;

    while (const Move curr_move = mp.get_next(play_quiets))
    {
        // illegal move
        if (!pos.do_move(curr_move)) { continue; }

        int score = INT_MIN;
        Line line;
        ++moves_searched;

        if (pos.is_draw()) { score = DRAW_SCORE; }
        else
        {
            if (moves_searched > 1 && depth > LMR_DEPTH && move_capture(curr_move) == NO_PIECE && move_promotion_type(curr_move) == NO_PIECE)
            {
                const int lmr_history_value =
                    std::clamp(move_selector::get_history_entry(~pos.get_side(), curr_move) / LMR_HISTORY_DIVISOR, -LMR_HISTORY_MIN, LMR_HISTORY_MAX);
                // reduction factor
                int R = LMR_TABLE[depth][std::min(moves_searched, 63)];
                // reduce for pv nodes
                R -= PV_NODE;
                // reduce for killers
                R -= move_selector::is_killer(curr_move, ply);
                // reduce based on history heuristic and lmr reduction
                R = std::clamp(R - lmr_history_value, 0, depth - LMR_REDUCTION);
                // search current move with reduced depth:
                score = -negamax<NodeType::NON_PV>(pos, -alpha - 1, -alpha, depth - R - 1, ply + 1, curr_move, nodes, line);
                // if search does not fail low, we search again without
                // reduction
                if (R && score > alpha) { score = -negamax<NodeType::NON_PV>(pos, -alpha - 1, -alpha, depth - 1, ply + 1, curr_move, nodes, line); }
            }
            else if (!PV_NODE || moves_searched > 1)
            {
                score = -negamax<NodeType::NON_PV>(pos, -alpha - 1, -alpha, depth - 1, ply + 1, curr_move, nodes, line);
            }
            // full PV search if all options are exhausted
            if (PV_NODE && (moves_searched == 1 || ((ROOT_NODE || score < beta) && score > alpha)))
            {
                score = -negamax<NodeType::PV>(pos, -beta, -alpha, depth - 1, ply + 1, curr_move, nodes, line);
            }
        }

        pos.undo_move(curr_move);

        if (stop) { return 0; }

        // fail high
        if (score >= beta)
        {
            if (move_capture(curr_move) == NO_PIECE)
            {
                mp.update_history<HashFlag::BETA>(curr_move, depth);
                move_selector::update_killers(curr_move, ply);
                move_selector::update_counter_entry(pos.get_side(), prev_move, curr_move);
            }
            tt.new_entry(pos.get_key(), depth, beta, HashFlag::BETA, curr_move);
            return beta;
        }

        if (score > best_score)
        {
            // new best move found
            best_move = curr_move;
            best_score = score;

            if (score > alpha)
            {
                if constexpr (PV_NODE)
                {
                    pv_line.last = pv_line.moves.data();
                    *pv_line.last++ = curr_move;
                    for (const auto& m : line) { *pv_line.last++ = m; }
                }
                hash_flag = HashFlag::EXACT;
                // new best move found
                alpha = score;
            }
        }

        if (!ROOT_NODE && score > -MIN_CHECKMATE_SCORE && moves_searched >= (4 + depth * depth)) { play_quiets = false; }
    }

    // no legal moves
    if (moves_searched == 0) { return in_check ? ply - CHECKMATE_SCORE : -DRAW_SCORE; }

    tt.new_entry(pos.get_key(), depth, best_score, hash_flag, best_move);

    if (hash_flag == HashFlag::EXACT && move_capture(best_move) == NO_PIECE) { mp.update_history<HashFlag::EXACT>(best_move, depth); }

    return alpha;
}

// begin search
void start_search(Position& pos, const SearchLimits& limits, SearchInfo& info)
{
    move_gen::MoveList ml(pos);
    ml.remove_illegal(pos);

    if (ml.size() > 1)
    {
        start_time = std::chrono::steady_clock::now();
        allocated_time = limits.depth ? LLONG_MAX : 5 * limits.time[pos.get_side()] / (limits.moves_left + 5);

        int beta = CHECKMATE_SCORE;
        int alpha = -beta;

        move_selector::reset_killers();
        move_selector::age_history();

        info.nodes = 0;

        for (int depth = 1; depth <= MAX_PLY && (limits.depth == 0 || depth <= limits.depth); ++depth)
        {
            info.score = negamax<NodeType::ROOT>(pos, alpha, beta, depth, 0, MOVE_NONE, info.nodes, info.pv_line);

            if (stop)
            {
                stop = false;
                break;
            }

            if (info.score <= alpha)
            {
                assert(depth > ASPIRATION_DEPTH);
                alpha = -CHECKMATE_SCORE;
                --depth;
                continue;
            }

            if (info.score >= beta)
            {
                assert(depth > ASPIRATION_DEPTH);
                beta = CHECKMATE_SCORE;
                --depth;
                continue;
            }

            const auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();

            std::cout << "info depth " << depth << " score cp " << info.score << " nodes " << info.nodes << " time " << elapsed_time << " nps "
                      << 1000ULL * info.nodes / (elapsed_time + 1) << " pv ";

            for (const auto& it : info.pv_line) { std::cout << it << " "; }

            std::cout << '\n';

            if (elapsed_time > allocated_time / 3) { break; }

            if (depth > ASPIRATION_DEPTH)
            {
                alpha = info.score - DELTA;
                beta = info.score + DELTA;
            }
        }
    }
    else
    {
        assert(ml.size() != 0);
        *info.pv_line.last++ = *ml.begin();
    }

    std::cout << "bestmove " << info.pv_line.moves[0] << '\n';
}

} // namespace clovis::search
