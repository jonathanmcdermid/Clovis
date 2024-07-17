#include <gtest/gtest.h>

#include "clovis/engine/evaluate.hpp"

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

    ASSERT_FALSE(is_fianchetto<BLACK>(pos, B7));

    ASSERT_TRUE(is_fianchetto<BLACK>(pos, G7));
}

TEST_F(EvaluateTest, OPEN_FILE_TEST)
{
    // 8 . . . . . . . .
    // 7 . P . . . . p .
    // 6 . . . . . . P .
    // 5 p . . p . . P .
    // 4 . p . . . . . .
    // 3 . . . . P . . .
    // 2 P . . . . . . .
    // 1 . . . . . . . .
    //   a b c d e f g h

    Position pos("8/1P4p1/6P1/p2p2P1/1p6/4P3/P7/8 w - - 0 1");

    Bitboard pawns = pos.get_pc_bb(W_PAWN) | pos.get_pc_bb(B_PAWN);

    ASSERT_FALSE(is_open_file(pawns, FILE_A));
    ASSERT_FALSE(is_open_file(pawns, FILE_B));
    ASSERT_FALSE(is_open_file(pawns, FILE_D));
    ASSERT_FALSE(is_open_file(pawns, FILE_E));
    ASSERT_FALSE(is_open_file(pawns, FILE_G));

    ASSERT_TRUE(is_open_file(pawns, FILE_C));
    ASSERT_TRUE(is_open_file(pawns, FILE_F));
    ASSERT_TRUE(is_open_file(pawns, FILE_H));
}

TEST_F(EvaluateTest, EVALUATE_TEST)
{
    // 8 r n b q k b n r
    // 7 p p p p p p p p
    // 6 . . . . . . . .
    // 5 . . . . . . . .
    // 4 . . . . . . . .
    // 3 . . . . . . . .
    // 2 P P P P P P P P
    // 1 R N B Q K B N R
    //   a b c d e f g h

    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    ASSERT_EQ(evaluate<false>(pos), eval::TEMPO_BONUS.mg);
}

TEST_F(EvaluateTest, TRACE_TEST_BASIC)
{
    // 8 r n b q k b n r
    // 7 p p p p p p p p
    // 6 . . . . . . . .
    // 5 . . . . . . . .
    // 4 . . . . . . . .
    // 3 . . . . . . . .
    // 2 P P P P P P P P
    // 1 R N B Q K B N R
    //   a b c d e f g h

    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    evaluate<true>(pos);

    ASSERT_EQ(0, T[DOUBLE_PAWN][WHITE]);
    ASSERT_EQ(0, T[DOUBLE_PAWN][BLACK]);
    ASSERT_EQ(0, T[ISOLATED_PAWN][WHITE]);
    ASSERT_EQ(0, T[ISOLATED_PAWN][BLACK]);
    ASSERT_EQ(1, T[BISHOP_PAIR][WHITE]);
    ASSERT_EQ(1, T[BISHOP_PAIR][BLACK]);
    ASSERT_EQ(0, T[ROOK_FULL][WHITE]);
    ASSERT_EQ(0, T[ROOK_FULL][BLACK]);
    ASSERT_EQ(0, T[ROOK_SEMI][WHITE]);
    ASSERT_EQ(0, T[ROOK_SEMI][BLACK]);
    ASSERT_EQ(-2, T[ROOK_CLOSED][WHITE]);
    ASSERT_EQ(-2, T[ROOK_CLOSED][BLACK]);
    ASSERT_EQ(1, T[TEMPO][WHITE]);
    ASSERT_EQ(0, T[TEMPO][BLACK]);
    ASSERT_EQ(0, T[KING_OPEN][WHITE]);
    ASSERT_EQ(0, T[KING_OPEN][BLACK]);
    ASSERT_EQ(0, T[KING_ADJ_OPEN][WHITE]);
    ASSERT_EQ(0, T[KING_ADJ_OPEN][BLACK]);
    ASSERT_EQ(0, T[KNIGHT_OUTPOST][WHITE]);
    ASSERT_EQ(0, T[KNIGHT_OUTPOST][BLACK]);
    ASSERT_EQ(0, T[BISHOP_OUTPOST][WHITE]);
    ASSERT_EQ(0, T[BISHOP_OUTPOST][BLACK]);
    ASSERT_EQ(0, T[WEAK_QUEEN][WHITE]);
    ASSERT_EQ(0, T[WEAK_QUEEN][BLACK]);
    ASSERT_EQ(0, T[ROOK_OUR_PASSER][WHITE]);
    ASSERT_EQ(0, T[ROOK_OUR_PASSER][BLACK]);
    ASSERT_EQ(0, T[ROOK_THEIR_PASSER][WHITE]);
    ASSERT_EQ(0, T[ROOK_THEIR_PASSER][BLACK]);
    ASSERT_EQ(-2, T[TALL_PAWN][WHITE]);
    ASSERT_EQ(-2, T[TALL_PAWN][BLACK]);
    ASSERT_EQ(0, T[FIANCHETTO][WHITE]);
    ASSERT_EQ(0, T[FIANCHETTO][BLACK]);
    ASSERT_EQ(0, T[ROOK_ON_SEVENTH][WHITE]);
    ASSERT_EQ(0, T[ROOK_ON_SEVENTH][BLACK]);
}

TEST_F(EvaluateTest, TRACE_TEST_COMPLEX)
{
    // 8 . . . k r . . .
    // 7 . . p . . R b .
    // 6 P . p p . p . p
    // 5 . P n . n . . .
    // 4 r . P . . B . .
    // 3 . . . . . . P .
    // 2 . . . . Q P B P
    // 1 R . . . K . . .
    //   a b c d e f g h

    Position pos("3kr3/2p2Rb1/P1pp1p1p/1Pn1n3/r1P2B2/6P1/4QPBP/R3K3 w - - 0 1");

    evaluate<true>(pos);

    ASSERT_EQ(0, T[DOUBLE_PAWN][WHITE]);
    ASSERT_EQ(-2, T[DOUBLE_PAWN][BLACK]);
    ASSERT_EQ(0, T[ISOLATED_PAWN][WHITE]);
    ASSERT_EQ(-2, T[ISOLATED_PAWN][BLACK]);
    ASSERT_EQ(1, T[BISHOP_PAIR][WHITE]);
    ASSERT_EQ(0, T[BISHOP_PAIR][BLACK]);
    ASSERT_EQ(0, T[ROOK_FULL][WHITE]);
    ASSERT_EQ(1, T[ROOK_FULL][BLACK]);
    ASSERT_EQ(0, T[ROOK_SEMI][WHITE]);
    ASSERT_EQ(1, T[ROOK_SEMI][BLACK]);
    ASSERT_EQ(-1, T[ROOK_CLOSED][WHITE]);
    ASSERT_EQ(0, T[ROOK_CLOSED][BLACK]);
    ASSERT_EQ(1, T[TEMPO][WHITE]);
    ASSERT_EQ(0, T[TEMPO][BLACK]);
    ASSERT_EQ(-1, T[KING_OPEN][WHITE]);
    ASSERT_EQ(0, T[KING_OPEN][BLACK]);
    ASSERT_EQ(0, T[KING_ADJ_OPEN][WHITE]);
    ASSERT_EQ(-1, T[KING_ADJ_OPEN][BLACK]);
    ASSERT_EQ(0, T[KNIGHT_OUTPOST][WHITE]);
    ASSERT_EQ(1, T[KNIGHT_OUTPOST][BLACK]);
    ASSERT_EQ(1, T[BISHOP_OUTPOST][WHITE]);
    ASSERT_EQ(0, T[BISHOP_OUTPOST][BLACK]);
    ASSERT_EQ(-1, T[WEAK_QUEEN][WHITE]);
    ASSERT_EQ(0, T[WEAK_QUEEN][BLACK]);
    // ASSERT_EQ(1, T[ROOK_OUR_PASSER][WHITE]); TODO: problem identified!
    // ASSERT_EQ(0, T[ROOK_OUR_PASSER][BLACK]);
    // ASSERT_EQ(0, T[ROOK_THEIR_PASSER][WHITE]);
    // ASSERT_EQ(1, T[ROOK_THEIR_PASSER][BLACK]);
    ASSERT_EQ(0, T[TALL_PAWN][WHITE]);
    ASSERT_EQ(-1, T[TALL_PAWN][BLACK]);
    ASSERT_EQ(1, T[FIANCHETTO][WHITE]);
    ASSERT_EQ(0, T[FIANCHETTO][BLACK]);
    ASSERT_EQ(1, T[ROOK_ON_SEVENTH][WHITE]);
    ASSERT_EQ(0, T[ROOK_ON_SEVENTH][BLACK]);
}
