#include <gtest/gtest.h>

#include "position.hpp"

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

TEST_F(PositionTest, SEE_TEST)
{
    struct SeeData
    {
        std::string fen;
        std::string move;
        int val;
    };

    const std::array<SeeData, 18> seeData = {
        {// {"4k3/8/8/1r1p4/B7/4N3/8/4K3 w - - 0 1", "Nxd5", PIECE_VALUE[PAWN]},
         {"4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -", "hxg4", 0},
         {"4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - -", "hxg4", 0},
         {"4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -", "Bxf3", 0},
         {"2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -", "dxe5", PIECE_VALUE[PAWN]},
         {"7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - -", "Re8", 0},
         {"6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - -", "Re8", -PIECE_VALUE[ROOK]},
         {"7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - -", "Re8", -PIECE_VALUE[ROOK]},
         // {"6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", "f8=Q", PIECE_VALUE[BISHOP] - PIECE_VALUE[PAWN]},
         // {"6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -", "f8=N", PIECE_VALUE[KNIGHT] - PIECE_VALUE[PAWN]},
         // {"7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", "f8=Q", PIECE_VALUE[QUEEN] - PIECE_VALUE[PAWN]},
         // {"7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -", "f8=B", PIECE_VALUE[BISHOP] - PIECE_VALUE[PAWN]},
         // {"7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - -", "f8=R", -PIECE_VALUE[PAWN]},
         {"8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - -", "Rxf1+", 0},
         {"8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - -", "Rxf1+", 0},
         {"2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - -", "Qxc1", 2 * PIECE_VALUE[ROOK] - PIECE_VALUE[QUEEN]},
         {"r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq -", "Nxe5", PIECE_VALUE[PAWN]},
         {"6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - -", "Bxe5", 0},
         // {"3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6", "gxh6", 0},
         // {"3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6", "gxh6", PIECE_VALUE[PAWN]},
         {"2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - -", "Rxc8", PIECE_VALUE[ROOK]},
         {"2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - -", "Rxc8", PIECE_VALUE[BISHOP]},
         {"2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - -", "Rxc5", PIECE_VALUE[BISHOP]},
         {"8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - -", "Bxc6", PIECE_VALUE[PAWN] - PIECE_VALUE[BISHOP]},
         {"4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", "Rxe4", PIECE_VALUE[PAWN] - PIECE_VALUE[ROOK]},
         {"4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -", "Bxe4", PIECE_VALUE[PAWN]}}};

    for (const auto& it : seeData)
    {
        Position pos(it.fen.c_str());
        Move move = pos.parse(it.move);

        ASSERT_EQ(pos.see(move), it.val);
    }
}

TEST_F(PositionTest, DISCOVERY_TEST)
{
    Position pos("4k3/6q1/8/8/8/2P5/1B6/4K3 w - - 0 1");
    ASSERT_TRUE(pos.is_discovery_threat<BLACK>(G7));
    pos.set("4k3/6q1/8/8/2p5/2P5/1B6/4K3 w - - 0 1");
    ASSERT_FALSE(pos.is_discovery_threat<BLACK>(G7));
    pos.set("4k3/6q1/8/8/1b6/2P5/1B6/4K3 w - - 0 1");
    ASSERT_TRUE(pos.is_discovery_threat<BLACK>(G7));
    // pos.set("4k3/6q1/8/b7/8/2P5/1B6/4K3 w - - 0 1"); TODO: problem detected! immobile pawn is not recognized
    // ASSERT_FALSE(pos.is_discovery_threat<BLACK>(G7));
}

TEST_F(PositionTest, ATTACKED_TEST)
{
    Position pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    ASSERT_TRUE(pos.is_attacked<BLACK>(A2));
    ASSERT_FALSE(pos.is_attacked<WHITE>(A2));
    ASSERT_TRUE(pos.is_attacked<WHITE>(A7));
    ASSERT_FALSE(pos.is_attacked<BLACK>(A7));
    ASSERT_FALSE(pos.is_attacked<BLACK>(A1));
    ASSERT_FALSE(pos.is_attacked<WHITE>(A8));
}
