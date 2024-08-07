#include <gtest/gtest.h>

#include "move_selector.hpp"

using namespace clovis;

class MoveSelectorTest : public testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(MoveSelectorTest, TT_MOVE_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Move tt_move = pos.parse("e4");
    move_selector::MoveSelector mp(pos, 0, MOVE_NONE, tt_move);

    ASSERT_EQ(mp.get_next(true), tt_move);
}

TEST_F(MoveSelectorTest, MOVE_ORDERING_TEST)
{
    Position pos("8/P7/1P6/8/7k/6p1/pn6/K6N w - - 0 1");
    Move tt_move = pos.parse("b7");
    move_selector::MoveSelector mp(pos, 0, MOVE_NONE, tt_move);

    // TT move
    ASSERT_EQ(mp.get_next(true), tt_move);
    // Promotions
    ASSERT_EQ(mp.get_next(true), pos.parse("a8=Q"));
    // Winning captures
    ASSERT_EQ(mp.get_next(true), pos.parse("Kxb2"));
    ASSERT_EQ(mp.get_next(true), pos.parse("Kxa2"));
    // Underpromotions
    ASSERT_EQ(mp.get_next(true), pos.parse("a8=N"));
    ASSERT_EQ(mp.get_next(true), pos.parse("a8=B"));
    ASSERT_EQ(mp.get_next(true), pos.parse("a8=R"));
    // Quiets
    ASSERT_EQ(mp.get_next(true), pos.parse("Nf2"));
    ASSERT_EQ(mp.get_next(true), pos.parse("Kb1")); // illegal
    // Losing captures
    ASSERT_EQ(mp.get_next(true), pos.parse("Nxg3"));
    // No moves left
    ASSERT_EQ(mp.get_next(true), MOVE_NONE);
}

TEST_F(MoveSelectorTest, KILLER_TEST)
{
    Position pos("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    move_selector::MoveSelector mp(pos, 0, MOVE_NONE, MOVE_NONE);
    move_selector::update_killers(pos.parse("Kd2"), 0);
    move_selector::update_killers(pos.parse("Ke2"), 0);
    move_selector::update_killers(pos.parse("Ke2"), 0);
    move_selector::update_killers(pos.parse("Kf2"), 1);

    ASSERT_EQ(mp.get_next(true), pos.parse("Ke2"));
    ASSERT_EQ(mp.get_next(true), pos.parse("Kd2"));
}
