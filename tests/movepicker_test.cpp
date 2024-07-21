#include <gtest/gtest.h>

#include "movepicker.hpp"

using namespace clovis;

class MovePickerTest : public testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MovePickerTest, TT_MOVE_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Move tt_move = pos.parse("e4");
    move_pick::MovePicker mp(pos, 0, MOVE_NONE, tt_move);

    ASSERT_EQ(mp.get_next(true), tt_move);
}

TEST_F(MovePickerTest, TT_MOVE_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Move tt_move = pos.parse("e4");
    move_pick::MovePicker mp(pos, 0, MOVE_NONE, tt_move);

    ASSERT_EQ(mp.get_next(true), tt_move);
}
