#pragma once

#include "movepicker.hpp"
#include "tt.hpp"

namespace clovis::search {

struct SearchLimits
{
    constexpr SearchLimits() = default;

    int move_time{0}, start_time{0}, moves_left{20}, depth{0}, mate{0}, perft{0};
    std::array<int, 2> time{{5000, 5000}}, inc{{0, 0}};
    bool infinite{false};
    uint64_t nodes{0};
};

struct Line
{
    constexpr Line() = default;
    [[nodiscard]] int move_count() const { return static_cast<int>(last - moves.data()); }
    [[nodiscard]] const Move* begin() const { return moves.data(); }
    [[nodiscard]] const Move* end() const { return last; }

    std::array<Move, MAX_PLY> moves{};
    Move* last{moves.data()};
};

struct SearchInfo
{
    constexpr SearchInfo() = default;

    int score{0};
    uint64_t nodes{0ULL};
    Line pv_line;
};

void start_search(Position& pos, const SearchLimits& limits, SearchInfo& info);
void clear();

} // namespace clovis::search
