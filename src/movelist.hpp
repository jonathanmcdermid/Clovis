#pragma once

#include <algorithm>

#include "position.hpp"

namespace clovis::move_gen {

constexpr int MAX_MOVES = 256;

template <typename T, MoveType M> T* generate(const Position& pos, T* moves);
template <typename T> void print_moves(const T* m, const T* end);

class MoveList
{
  public:
    explicit MoveList(const Position& pos) : last(generate<Move, MoveType::ALL>(pos, moves.data())) {}
    [[nodiscard]] int size() const { return static_cast<int>(last - moves.data()); }
    [[nodiscard]] auto begin() const { return moves.data(); }
    [[nodiscard]] auto end() const { return last; }
    void remove_illegal(Position& pos)
    {
        last = std::remove_if(moves.data(), last, [&pos](const Move& move) { return pos.do_move(move) ? pos.undo_move(move), false : true; });
    }

  private:
    std::array<Move, MAX_MOVES> moves{};
    Move* last;
};

} // namespace clovis::move_gen
