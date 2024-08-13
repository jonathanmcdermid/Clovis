#pragma once

#include <algorithm>
#include <cassert>

#include "movelist.hpp"
#include "tt.hpp"

namespace clovis::move_selector {

constexpr size_t COLOUR_FROM_TO_SIZE = 2 * SQ_N * SQ_N;

enum class StageType
{
    TT_MOVE,
    INIT_CAPTURES,
    WINNING_CAPTURES,
    INIT_QUIETS,
    QUIETS,
    LOSING_CAPTURES,
    FINISHED
};

struct KillerEntry
{
    Move primary{MOVE_NONE};
    Move secondary{MOVE_NONE};
};

extern std::array<int, COLOUR_FROM_TO_SIZE> history_table;
extern std::array<Move, COLOUR_FROM_TO_SIZE> counter_table;
extern std::array<KillerEntry, MAX_PLY> killer_table;

constexpr auto HISTORY_BONUS = [] {
    std::array<int, MAX_PLY + 1> arr{};

    for (int i = 0; i <= MAX_PLY; ++i) { arr[i] = 32 * std::min(i * i, 400); }

    return arr;
}();

constexpr auto MVV_LVA = [] {
    std::array<std::array<int, 15>, 15> arr{};

    for (const auto c : {WHITE, BLACK})
    {
        for (PieceType p1 = PAWN; p1 <= KING; ++p1)
        {
            for (PieceType p2 = PAWN; p2 <= KING; ++p2) { arr[make_piece(p1, c)][make_piece(p2, ~c)] = KING - p1 + KING * p2; }
        }
    }

    return arr;
}();

inline size_t cft_index(const Colour c, const Move m) { return SQ_N * SQ_N * c + move_from_sq(m) * SQ_N + move_to_sq(m); }
inline int& get_history_entry(const Colour c, const Move m) { return history_table[cft_index(c, m)]; }
inline Move& get_counter_entry(const Colour c, const Move m) { return counter_table[cft_index(c, m)]; }

inline void age_history()
{
    for (auto& it : history_table) { it >>= 4; }
}

inline void reset_counter() { counter_table.fill(MOVE_NONE); }
inline void reset_killers() { killer_table.fill({MOVE_NONE, MOVE_NONE}); }
inline void reset_history() { history_table.fill(0); }

inline void clear()
{
    reset_counter();
    reset_killers();
    reset_history();
}

inline void update_killers(const Move m, const int ply)
{
    if (killer_table[ply].primary != m)
    {
        killer_table[ply].secondary = killer_table[ply].primary;
        killer_table[ply].primary = m;
    }
}

inline void update_history_entry(const Move m, const Colour c, const int bonus)
{
    int& history_entry = history_table[cft_index(c, m)];
    history_entry += bonus - (history_entry * abs(bonus) >> 14);
}

inline void update_counter_entry(const Colour c, const Move prev, const Move curr) { counter_table[cft_index(c, prev)] = curr; }

inline bool is_killer(const Move m, const int ply) { return m == killer_table[ply].primary || m == killer_table[ply].secondary; }

class MoveSelector
{
  public:
    MoveSelector(const Position& p, const int pl, const Move pm, const Move ttm)
        : pos(p), ply(pl), curr(moves.data()), last(moves.data()), last_bad_cap(moves.data()), prev_move(pm), tt_move(ttm)
    {
    }
    Move get_next(bool play_quiets);
    template <HashFlag HF> void update_history(Move best_move, int depth) const;

  private:
    void score_captures();
    void score_quiets();

    const Position& pos;
    std::array<ScoredMove, move_gen::MAX_MOVES> moves;
    int ply;
    StageType stage{StageType::TT_MOVE};
    ScoredMove* curr;
    ScoredMove* last;
    ScoredMove* last_bad_cap;
    Move prev_move;
    Move tt_move;
};

template <HashFlag HF> void MoveSelector::update_history(const Move best_move, const int depth) const
{
    assert(!move_capture(best_move) || move_promotion_type(best_move));
    update_history_entry(best_move, pos.get_side(), HISTORY_BONUS[depth]);

    for (const auto& sm : std::ranges::subrange(last_bad_cap, HF == HashFlag::EXACT ? last : curr))
    {
        assert(!move_capture(sm) || move_promotion_type(sm));
        if (sm != best_move) { update_history_entry(sm, pos.get_side(), -HISTORY_BONUS[depth]); }
    }
}

} // namespace clovis::move_selector
