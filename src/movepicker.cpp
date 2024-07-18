#include "movepicker.hpp"

namespace clovis::move_pick {

std::array<int, COLOUR_FROM_TO_SIZE> history_table;
std::array<Move, COLOUR_FROM_TO_SIZE> counter_table;
std::array<KEntry, MAX_PLY> killer_table;

// return the next ordered move
Move MovePicker::get_next(const bool play_quiets)
{
    switch (stage)
    {
    case TT_MOVE:
        ++stage;
        if (tt_move != MOVE_NONE && (play_quiets || move_capture(tt_move) || move_promotion_type(tt_move))) { return tt_move; }
        [[fallthrough]];
    case INIT_CAPTURES:
        curr = last_bad_cap = moves.data();
        last = move_gen::generate<ScoredMove, CAPTURE_MOVES>(pos, moves.data());
        score_captures();
        std::stable_sort(moves.data(), last, [](const ScoredMove& lhs, const ScoredMove& rhs) { return lhs.score > rhs.score; });
        ++stage;
        [[fallthrough]];
    case WINNING_CAPTURES:
        while (curr < last)
        {
            assert(move_capture(*curr) || piece_type(move_promotion_type(*curr)) == QUEEN);
            if (curr->move != tt_move)
            {
                if (pos.see(*curr) >= (play_quiets ? -100 : 0)) { return *curr++; }
                if (play_quiets) { *last_bad_cap++ = *curr; }
            }
            ++curr;
        }
        ++stage;
        [[fallthrough]];
    case INIT_QUIETS:
        if (play_quiets)
        {
            curr = last_bad_cap;
            last = move_gen::generate<ScoredMove, QUIET_MOVES>(pos, curr);
            score_quiets();
            std::stable_sort(last_bad_cap, last, [](const ScoredMove& lhs, const ScoredMove& rhs) { return lhs.score > rhs.score; });
        }
        ++stage;
        [[fallthrough]];
    case QUIETS:
        while (play_quiets && curr < last)
        {
            assert(!move_capture(*curr) || piece_type(move_promotion_type(*curr)) != QUEEN);
            if (*curr != tt_move) { return *curr++; }
            ++curr;
        }
        curr = moves.data();
        ++stage;
        [[fallthrough]];
    case LOSING_CAPTURES:
        if (play_quiets)
        {
            while (curr < last_bad_cap)
            {
                assert(move_capture(*curr));
                if (*curr != tt_move) { return *curr++; }
                ++curr;
            }
        }
        ++stage;
        [[fallthrough]];
    default: break;
    }

    return MOVE_NONE;
}

void MovePicker::score_captures()
{
    for (auto& sm : std::ranges::subrange(moves.data(), last))
    {
        // promotions supersede mvv-lva
        sm.score = MVV_LVA[move_piece_type(sm)][pos.get_pc(move_to_sq(sm))] + (move_promotion_type(sm) << 6);
    }
}

void MovePicker::score_quiets()
{
    const auto counter = get_counter_entry(pos.get_side(), prev_move);

    for (auto& sm : std::ranges::subrange(last_bad_cap, last))
    {
        sm.score = sm == killer_table[ply].primary     ? 22000
                   : sm == killer_table[ply].secondary ? 21000
                   : sm == counter                     ? 20000
                                                       : get_history_entry(pos.get_side(), sm);
    }
}

} // namespace clovis::move_pick
