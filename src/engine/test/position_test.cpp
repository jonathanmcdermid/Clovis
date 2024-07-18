#include <gtest/gtest.h>

#include "clovis/engine/position.hpp"

using namespace clovis;

class PositionTest : public testing::Test
{
  public:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PositionTest, BOARD_STATE_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    ASSERT_EQ(pos.get_fen(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_FALSE(pos.is_king_in_check());
    ASSERT_TRUE(pos.is_stm_major());
    ASSERT_FALSE(pos.is_draw());
    ASSERT_EQ(pos.get_side(), WHITE);
    ASSERT_EQ(pos.get_pc(A1), ROOK);
    ASSERT_EQ(pos.get_pc_bb(W_KING), sq_bb(E1));
    ASSERT_EQ(pos.get_occ_bb(WHITE), A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1 | A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2);
    ASSERT_EQ(pos.get_en_passant(), SQ_NONE);
    ASSERT_EQ(pos.get_full_move_clock(), 1);
    ASSERT_EQ(pos.get_castle_rights(), ALL_CASTLING);
    ASSERT_EQ(pos.get_game_phase(), MAX_GAME_PHASE);
}

TEST_F(PositionTest, SET_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Key before_key = pos.get_key();
    Key before_pawn_key = pos.get_pawn_key();

    pos.set("3kr3/2p2Rb1/P1pp1p1p/1Pn1n3/r1P2B2/6P1/4QPBP/R3K3 b - - 0 5");

    ASSERT_EQ(pos.get_fen(), "3kr3/2p2Rb1/P1pp1p1p/1Pn1n3/r1P2B2/6P1/4QPBP/R3K3 b - - 0 5");
    ASSERT_FALSE(pos.is_king_in_check());
    ASSERT_TRUE(pos.is_stm_major());
    ASSERT_FALSE(pos.is_draw());
    ASSERT_EQ(pos.get_side(), BLACK);
    ASSERT_EQ(pos.get_pc(E8), B_ROOK);
    ASSERT_EQ(pos.get_pc_bb(B_KING), sq_bb(D8));
    ASSERT_EQ(pos.get_occ_bb(WHITE), A1 | E1 | E2 | F2 | G2 | H2 | G3 | F4 | C4 | B5 | A6 | F7);
    ASSERT_EQ(pos.get_en_passant(), SQ_NONE);
    ASSERT_EQ(pos.get_full_move_clock(), 5);
    ASSERT_EQ(pos.get_castle_rights(), 0);
    ASSERT_EQ(pos.get_game_phase(), 17);
    ASSERT_NE(pos.get_key(), before_key);
    ASSERT_NE(pos.get_pawn_key(), before_pawn_key);
}

TEST_F(PositionTest, VALID_MOVE_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Key before_key = pos.get_key();
    Key before_pawn_key = pos.get_pawn_key();
    Move move = pos.parse("e4");

    ASSERT_TRUE(pos.do_move(move));
    ASSERT_EQ(pos.get_fen(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    ASSERT_FALSE(pos.is_king_in_check());
    ASSERT_TRUE(pos.is_stm_major());
    ASSERT_FALSE(pos.is_draw());
    ASSERT_EQ(pos.get_side(), BLACK);
    ASSERT_EQ(pos.get_pc(E2), NO_PIECE);
    ASSERT_EQ(pos.get_pc_bb(W_PAWN), A2 | B2 | C2 | D2 | F2 | G2 | H2 | E4);
    ASSERT_EQ(pos.get_occ_bb(WHITE), A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1 | A2 | B2 | C2 | D2 | F2 | G2 | H2 | E4);
    ASSERT_EQ(pos.get_en_passant(), E3);
    ASSERT_EQ(pos.get_full_move_clock(), 1);
    ASSERT_EQ(pos.get_castle_rights(), ALL_CASTLING);
    ASSERT_EQ(pos.get_game_phase(), MAX_GAME_PHASE);
    ASSERT_NE(pos.get_key(), before_key);
    ASSERT_NE(pos.get_pawn_key(), before_pawn_key);

    pos.undo_move(move);

    ASSERT_EQ(pos.get_fen(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_FALSE(pos.is_king_in_check());
    ASSERT_TRUE(pos.is_stm_major());
    ASSERT_FALSE(pos.is_draw());
    ASSERT_EQ(pos.get_side(), WHITE);
    ASSERT_EQ(pos.get_pc(A1), ROOK);
    ASSERT_EQ(pos.get_pc_bb(W_KING), sq_bb(E1));
    ASSERT_EQ(pos.get_occ_bb(WHITE), A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1 | A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2);
    ASSERT_EQ(pos.get_en_passant(), SQ_NONE);
    ASSERT_EQ(pos.get_full_move_clock(), 1);
    ASSERT_EQ(pos.get_castle_rights(), ALL_CASTLING);
    ASSERT_EQ(pos.get_game_phase(), MAX_GAME_PHASE);
    ASSERT_EQ(pos.get_key(), before_key);
    ASSERT_EQ(pos.get_pawn_key(), before_pawn_key);
}

TEST_F(PositionTest, NULL_MOVE_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Key before_key = pos.get_key();
    Key before_pawn_key = pos.get_pawn_key();

    pos.do_null_move();

    ASSERT_EQ(pos.get_fen(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
    ASSERT_FALSE(pos.is_king_in_check());
    ASSERT_TRUE(pos.is_stm_major());
    ASSERT_FALSE(pos.is_draw());
    ASSERT_EQ(pos.get_side(), BLACK);
    ASSERT_EQ(pos.get_pc(A1), ROOK);
    ASSERT_EQ(pos.get_pc_bb(W_KING), sq_bb(E1));
    ASSERT_EQ(pos.get_occ_bb(WHITE), A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1 | A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2);
    ASSERT_EQ(pos.get_en_passant(), SQ_NONE);
    ASSERT_EQ(pos.get_full_move_clock(), 1);
    ASSERT_EQ(pos.get_castle_rights(), ALL_CASTLING);
    ASSERT_EQ(pos.get_game_phase(), MAX_GAME_PHASE);
    ASSERT_NE(pos.get_key(), before_key);
    ASSERT_EQ(pos.get_pawn_key(), before_pawn_key);

    pos.undo_null_move();

    ASSERT_EQ(pos.get_fen(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ASSERT_FALSE(pos.is_king_in_check());
    ASSERT_TRUE(pos.is_stm_major());
    ASSERT_FALSE(pos.is_draw());
    ASSERT_EQ(pos.get_side(), WHITE);
    ASSERT_EQ(pos.get_pc(A1), ROOK);
    ASSERT_EQ(pos.get_pc_bb(W_KING), sq_bb(E1));
    ASSERT_EQ(pos.get_occ_bb(WHITE), A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1 | A2 | B2 | C2 | D2 | E2 | F2 | G2 | H2);
    ASSERT_EQ(pos.get_en_passant(), SQ_NONE);
    ASSERT_EQ(pos.get_full_move_clock(), 1);
    ASSERT_EQ(pos.get_castle_rights(), ALL_CASTLING);
    ASSERT_EQ(pos.get_game_phase(), MAX_GAME_PHASE);
    ASSERT_EQ(pos.get_key(), before_key);
    ASSERT_EQ(pos.get_pawn_key(), before_pawn_key);
}

// bool see_ge(Move move, int threshold) const;
// template <Colour US> [[nodiscard]] Square get_pinner(Square sq) const;
// template <Colour US> [[nodiscard]] bool is_discovery_threat(Square sq) const;
// template <Colour US> [[nodiscard]] bool is_attacked(Square sq) const;
//  void print_position() const;
