#include <gtest/gtest.h>

#include "src/evaluate.h"

using namespace clovis;
using namespace clovis::eval;

class EvaluateTest : public testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EvaluateTest, DOUBLED_PAWN_TEST)
{
    // 8 . . . . . . . .
    // 7 . . . x . . . .
    // 6 . . . x . . . .
    // 5 . . . x . . . .
    // 4 . . . . . . . .
    // 3 . x . . . . . x
    // 2 x . . . . . . x
    // 1 . . . . . . . .
    //   a b c d e f g h

    Bitboard bb = A2 | B3 | D5 | D6 | D7 | H2 | H3;

    ASSERT_FALSE(is_doubled_pawn(bb, A2));
    ASSERT_FALSE(is_doubled_pawn(bb, B3));

    ASSERT_TRUE(is_doubled_pawn(bb, D5));
    ASSERT_TRUE(is_doubled_pawn(bb, D6));
    ASSERT_TRUE(is_doubled_pawn(bb, D7));
    ASSERT_TRUE(is_doubled_pawn(bb, H2));
    ASSERT_TRUE(is_doubled_pawn(bb, H3));
}

TEST_F(EvaluateTest, ISOLATED_PAWN_TEST)
{
    // 8 . . . . . . . .
    // 7 . . . x . . . .
    // 6 . . . . . . . .
    // 5 . . . . . . . .
    // 4 . . . . . . . .
    // 3 . x . . . . . x
    // 2 x . . . . . . x
    // 1 . . . . . . . .
    //   a b c d e f g h

    Bitboard bb = A2 | B3 | D7 | H2 | H3;

    ASSERT_FALSE(is_isolated_pawn(bb, A2));
    ASSERT_FALSE(is_isolated_pawn(bb, B3));

    ASSERT_TRUE(is_isolated_pawn(bb, D7));
    ASSERT_TRUE(is_isolated_pawn(bb, H2));
    ASSERT_TRUE(is_isolated_pawn(bb, H3));
}

TEST_F(EvaluateTest, PASSED_PAWN_TEST)
{
    // 8 . . . . . . . .
    // 7 . . . x . . . .
    // 6 . . . . . . . .
    // 5 . . . . . . . .
    // 4 . . . . . . . .
    // 3 . x . . . . . x
    // 2 x . . . . . . x
    // 1 . . . . . . . .
    //   a b c d e f g h

    Bitboard enemies = A2 | B3 | D7 | H2 | H3;

    ASSERT_FALSE(is_passed_pawn<WHITE>(enemies, B2));
    ASSERT_FALSE(is_passed_pawn<WHITE>(enemies, C6));

    ASSERT_TRUE(is_passed_pawn<WHITE>(enemies, B4));
    ASSERT_TRUE(is_passed_pawn<WHITE>(enemies, C7));
    ASSERT_TRUE(is_passed_pawn<WHITE>(enemies, F7));
}

TEST_F(EvaluateTest, CANDIDATE_PASSER_TEST)
{
    // 8 . . . . . . . .
    // 7 p . . . . . . .
    // 6 . . . . . . . .
    // 5 P P . . . p p p
    // 4 . . . . . . . .
    // 3 . . . . . P . P
    // 2 . . . . . . . .
    // 1 . . . . . . . .
    //   a b c d e f g h

    Position pos("8/p7/8/PP3ppp/8/5P1P/8/8 w - - 0 1");

    ASSERT_FALSE(is_candidate_passer<WHITE>(pos, A5));
    ASSERT_FALSE(is_candidate_passer<WHITE>(pos, F3));
    ASSERT_FALSE(is_candidate_passer<WHITE>(pos, H3));
    
    ASSERT_TRUE(is_candidate_passer<WHITE>(pos, B5));

    ASSERT_FALSE(is_candidate_passer<BLACK>(pos, A7));
    ASSERT_FALSE(is_candidate_passer<BLACK>(pos, F5));
    ASSERT_FALSE(is_candidate_passer<BLACK>(pos, H5));
    
    ASSERT_TRUE(is_candidate_passer<BLACK>(pos, G5));
}

TEST_F(EvaluateTest, OUTPOST_TEST_DISABLED)
{
    // 8 . . . . . . . .
    // 7 . . . . p . . .
    // 6 . . . . . . . .
    // 5 . p . p . P p .
    // 4 . N . N . b P .
    // 3 . . P . . P . .
    // 2 . . . . . . . .
    // 1 . . . . . . . .
    //   a b c d e f g h

    // Position pos("8/4p3/8/1p1p1Pp1/1N1N1bP1/2P2P2/8/8 w - - 0 1");
    // EvalInfo ei;
    // 
    // we need to populate the potential pawn attack tables, this is done in evaluate_pawns
    // TODO: this doesnt seem to work, we may need to set up ei object beforehand
    // evaluate_pawns<WHITE, false>(pos, ei);
    // evaluate_pawns<BLACK, false>(pos, ei);
    //
    // ASSERT_EQ(ei.potential_pawn_attacks[WHITE], 0ULL);
    // ASSERT_EQ(ei.potential_pawn_attacks[BLACK], 0ULL);
    // 
    // ASSERT_FALSE(is_outpost<WHITE>(D4, ei));
    // 
    // ASSERT_TRUE(is_outpost<WHITE>(B4, ei));
    // ASSERT_TRUE(is_outpost<BLACK>(F4, ei));
}

TEST_F(EvaluateTest, IS_FIANCHETTO_TEST)
{
    // 8 . . . . . . . .
    // 7 . b . . . . b .
    // 6 . . . . . . p .
    // 5 . . . . . . . .
    // 4 . p . P . . . .
    // 3 . P . . . . P .
    // 2 . B . . . . B .
    // 1 . . . . . . . .
    //   a b c d e f g h

    Position pos("8/1b4b1/6p1/8/1p1P4/1P4P1/1B4B1/8 w - - 0 1");
    
    ASSERT_FALSE(is_fianchetto<WHITE>(pos, B2));

    ASSERT_TRUE(is_fianchetto<WHITE>(pos, G2));

    // TODO: problem identified! ASSERT_FALSE(is_fianchetto<BLACK>(pos, B7));
    
    ASSERT_TRUE(is_fianchetto<BLACK>(pos, G7));
}

TEST_F(EvaluateTest, OPEN_FILE_TEST)
{
    // 8 . . . . . . . .
    // 7 . . . . . . . .
    // 6 . . . . . . . .
    // 5 . . . . . . . .
    // 4 . . . . . . . .
    // 3 . . . . . . . .
    // 2 . . . . . . . .
    // 1 . . . . . . . .
    //   a b c d e f g h

    // Position pos("");
    // ASSERT_FALSE(is_open_file(pos.get_pc_bb(W_PAWN) | pos.get_pc_bb(B_PAWN), FILE_A));
}

TEST_F(EvaluateTest, EVALUATE_TEST)
{
    // 8 . . . . . . . .
    // 7 . . . . . . . .
    // 6 . . . . . . . .
    // 5 . . . . . . . .
    // 4 . . . . . . . .
    // 3 . . . . . . . .
    // 2 . . . . . . . .
    // 1 . . . . . . . .
    //   a b c d e f g h

    // Position pos(START_POSITION.data());
    // ASSERT_EQ(evaluate<false>(pos), eval::TEMPO_BONUS.mg);
}

TEST_F(EvaluateTest, TRACE_TEST)
{
    // 8 . . . . . . . .
    // 7 . . . . . . . .
    // 6 . . . . . . . .
    // 5 . . . . . . . .
    // 4 . . . . . . . .
    // 3 . . . . . . . .
    // 2 . . . . . . . .
    // 1 . . . . . . . .
    //   a b c d e f g h

    // Position pos(START_POSITION.data());
    // ASSERT_EQ(evaluate<true>(pos), eval::TEMPO_BONUS.mg);
}
