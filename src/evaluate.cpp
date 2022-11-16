#include "evaluate.h"

namespace Clovis {

    namespace Eval {

        enum GamePhase : int {
            MG, EG,
            PHASE_N = 2
        };

        Score pawn_table[32] = {
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        Score(123,262), Score(166,271), Score(148,218), Score(163,199),
        Score(87,115), Score(99,117), Score(129,111), Score(129,115),
        Score(73,97), Score(93,94), Score(95,95), Score(115,87),
        Score(63,88), Score(74,89), Score(90,87), Score(109,84),
        Score(75,79), Score(82,83), Score(92,88), Score(94,94),
        Score(68,82), Score(89,83), Score(90,98), Score(83,101),
        Score(0,0), Score(0,0), Score(0,0), Score(0,0),
        };
        Score knight_table[32] = {
                Score(149,247), Score(239,261), Score(233,294), Score(324,276),
                Score(267,276), Score(298,292), Score(392,272), Score(337,301),
                Score(321,271), Score(401,278), Score(376,306), Score(397,302),
                Score(347,284), Score(346,311), Score(358,321), Score(362,326),
                Score(322,291), Score(346,302), Score(346,320), Score(346,326),
                Score(316,285), Score(342,292), Score(348,302), Score(352,313),
                Score(318,264), Score(297,288), Score(334,293), Score(346,295),
                Score(275,259), Score(324,258), Score(311,283), Score(318,289),
        };
        Score bishop_table[32] = {
                Score(269,304), Score(302,300), Score(225,313), Score(250,316),
                Score(279,310), Score(318,310), Score(317,312), Score(315,307),
                Score(315,313), Score(355,304), Score(361,306), Score(352,303),
                Score(324,310), Score(324,315), Score(338,318), Score(358,316),
                Score(326,305), Score(332,311), Score(324,319), Score(350,315),
                Score(331,303), Score(339,310), Score(348,312), Score(333,321),
                Score(332,294), Score(357,294), Score(342,303), Score(335,307),
                Score(300,298), Score(322,309), Score(326,298), Score(315,310),
        };
        Score rook_table[32] = {
                Score(478,537), Score(478,537), Score(449,547), Score(504,532),
                Score(487,536), Score(487,536), Score(529,524), Score(526,522),
                Score(460,532), Score(490,529), Score(473,531), Score(471,531),
                Score(449,534), Score(456,529), Score(475,533), Score(471,527),
                Score(444,530), Score(466,525), Score(455,531), Score(470,524),
                Score(449,521), Score(465,522), Score(471,516), Score(470,519),
                Score(444,523), Score(473,515), Score(474,518), Score(480,518),
                Score(478,507), Score(475,519), Score(495,515), Score(500,511),
        };
        Score queen_table[32] = {
                Score(903,992), Score(921,995), Score(931,1000), Score(956,988),
                Score(915,979), Score(867,1014), Score(923,1001), Score(881,1036),
                Score(940,974), Score(935,970), Score(920,986), Score(907,1021),
                Score(912,1003), Score(893,1015), Score(902,995), Score(897,1004),
                Score(916,983), Score(907,1003), Score(907,990), Score(902,995),
                Score(912,993), Score(931,959), Score(918,981), Score(921,965),
                Score(908,969), Score(927,944), Score(945,936), Score(938,953),
                Score(935,938), Score(930,935), Score(931,944), Score(944,943),
        };
        Score king_table[32] = {
                Score(-28,-42), Score(130,-44), Score(79,-19), Score(19,-23),
                Score(35,-11), Score(35,9), Score(27,20), Score(67,2),
                Score(35,-4), Score(106,12), Score(98,18), Score(40,9),
                Score(-36,-4), Score(-3,22), Score(-1,29), Score(-29,31),
                Score(-76,-6), Score(-46,11), Score(-69,34), Score(-118,46),
                Score(-38,-8), Score(-26,8), Score(-60,25), Score(-87,36),
                Score(11,-24), Score(2,-6), Score(-54,18), Score(-80,26),
                Score(11,-59), Score(31,-34), Score(-27,-13), Score(-11,-24),
        };
        Score passed_pawn_bonus[32] = {
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
                 Score(6, 70),  Score(8, 37),  Score(43, 57),  Score(29, 64),
                 Score(25, 139),  Score(23, 124),  Score(28, 96),  Score(13, 67),
                 Score(18, 76),  Score(6, 71),  Score(14, 47),  Score(6, 37),
                 Score(21, 41),  Score(0, 41),  Score(0, 25),  Score(0, 20),
                 Score(15, 15),  Score(11, 18),  Score(0, 6),  Score(0, 4),
                 Score(4, 13),  Score(21, 8),  Score(23, 0),  Score(0, 3),
                 Score(0, 0),  Score(0, 0),  Score(0, 0),  Score(0, 0),
        };
        Score double_pawn_penalty = Score(2, 8);
        Score isolated_pawn_penalty = Score(21, 9);
        Score bishop_pair_bonus = Score(35, 42);
        Score rook_open_file_bonus = Score(36, 0);
        Score rook_semi_open_file_bonus = Score(8, 14);
        Score mobility[7] = { Score(0, 0), Score(0, 0), Score(3, 1), Score(5, 2), Score(4, 3), Score(2, 8), Score(0, 0), };

        Score* piece_table[7] =
        {
            NULL,
            pawn_table,
            knight_table,
            bishop_table,
            rook_table,
            queen_table,
            king_table
        };

        Score* passed_table[SQ_N];

        Score* score_table[15][SQ_N];

        Bitboard file_masks[SQ_N];

        Bitboard rank_masks[SQ_N];

        Bitboard isolated_masks[SQ_N];

        Bitboard passed_masks[COLOUR_N][SQ_N];

        inline bool doubled_pawn(Bitboard bb, Square sq) {
            return popcnt(bb & file_masks[sq]) >= 2;
        }

        inline bool isolated_pawn(Bitboard bb, Square sq) {
            return !(bb & isolated_masks[sq]);
        }

        inline bool passed_pawn(Bitboard bb, Square sq, Colour side) {
            return !(bb & passed_masks[side][sq]);
        }

        void init_eval()
        {
            init_values();
            init_masks();
        }

        void init_values()
        {
            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                for (Square sq = SQ_ZERO; sq < 32; ++sq)
                {
                    int r = sq / 4;
                    int f = sq & 0x3;

                    score_table[make_piece(pt, WHITE)][8 * (7 - r) + f] = &piece_table[pt][sq];
                    score_table[make_piece(pt, WHITE)][8 * (7 - r) + (7 - f)] = &piece_table[pt][sq];

                    score_table[make_piece(pt, BLACK)][8 * r + f] = &piece_table[pt][sq];
                    score_table[make_piece(pt, BLACK)][8 * r + (7 - f)] = &piece_table[pt][sq];
                }
            }
            for (Square sq = SQ_ZERO; sq < 32; ++sq)
            {
                int r = sq / 4;
                int f = sq & 0x3;

                passed_table[8 * (7 - r) + f] = &passed_pawn_bonus[sq];
                passed_table[8 * (7 - r) + (7 - f)] = &passed_pawn_bonus[sq];
            }
        }

        void init_masks()
        {
            // file and rank masks
            for (File f = FILE_A; f <= FILE_H; ++f)
            {
                for (Rank r = RANK_1; r <= RANK_8; ++r)
                {
                    Square sq = make_square(f, r);

                    file_masks[sq] = set_file_rank_mask(f, RANK_NONE);
                    rank_masks[sq] = set_file_rank_mask(FILE_NONE, r);

                    isolated_masks[sq] |= set_file_rank_mask(f - 1, RANK_NONE);
                    isolated_masks[sq] |= set_file_rank_mask(f + 1, RANK_NONE);
                }
            }
            for (Rank r = RANK_1; r <= RANK_8; ++r)
            {
                for (File f = FILE_A; f <= FILE_H; ++f)
                {
                    Square sq = make_square(f, r);

                    passed_masks[WHITE][sq] |= set_file_rank_mask(f - 1, RANK_NONE);
                    passed_masks[WHITE][sq] |= set_file_rank_mask(f, RANK_NONE);
                    passed_masks[WHITE][sq] |= set_file_rank_mask(f + 1, RANK_NONE);

                    for (int i = 0; i < r + 1; ++i)
                        passed_masks[WHITE][sq] &= ~rank_masks[i * 8 + f];

                    passed_masks[BLACK][sq] |= set_file_rank_mask(f - 1, RANK_NONE);
                    passed_masks[BLACK][sq] |= set_file_rank_mask(f, RANK_NONE);
                    passed_masks[BLACK][sq] |= set_file_rank_mask(f + 1, RANK_NONE);

                    for (int i = 0; i < 8 - r; ++i)
                        passed_masks[BLACK][sq] &= ~rank_masks[(7 - i) * 8 + f];
                }
            }
        }

        void test_eval()
        {
            //test pawn bonus eval

            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   |   |   |   |   | 8
            //  +---+---+---+---+---+---+---+---+
            //  |   |   | P |   |   |   |   |   | 7
            //  +---+---+---+---+---+---+---+---+
            //  |   |   | P | p |   |   |   |   | 6
            //  +---+---+---+---+---+---+---+---+
            //  | K | P |   |   |   |   |   | r | 5
            //  +---+---+---+---+---+---+---+---+
            //  |   | R |   |   |   | p |   | k | 4
            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   |   | P |   |   | 3
            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   | P |   | P |   | 2
            //  +---+---+---+---+---+---+---+---+
            //  |   |   |   |   |   |   |   |   | 1
            //  +---+---+---+---+---+---+---+---+
            //    a   b   c   d   e   f   g   h

            // 3 white passed pawns
            // 1 white double pawns
            // 2 black isolated pawns

            cout << "Running evaluate tests..." << endl;

            Position pos("8/2P5/2Pp4/KP5r/1R3p1k/5P2/4P1P1/8 w - - ");

            assert(!doubled_pawn(pos.piece_bitboard[W_PAWN], B5));
            assert(doubled_pawn(pos.piece_bitboard[W_PAWN], C6));
            assert(doubled_pawn(pos.piece_bitboard[W_PAWN], C7));
            
            assert(!isolated_pawn(pos.piece_bitboard[W_PAWN], B5));
            assert(isolated_pawn(pos.piece_bitboard[B_PAWN], D6));
            assert(isolated_pawn(pos.piece_bitboard[B_PAWN], F4));
            assert(!isolated_pawn(pos.piece_bitboard[W_PAWN], F4));

            assert(passed_pawn(pos.piece_bitboard[B_PAWN], B5, WHITE));
            assert(passed_pawn(pos.piece_bitboard[B_PAWN], C6, WHITE));
            assert(passed_pawn(pos.piece_bitboard[B_PAWN], C7, WHITE));
            assert(!passed_pawn(pos.piece_bitboard[W_PAWN], D6, BLACK));

            Score expected_result;

            expected_result -= double_pawn_penalty * 2;

            expected_result -= isolated_pawn_penalty * -2;
            
            expected_result += *passed_table[relative_square(WHITE, B5)];
            expected_result += *passed_table[relative_square(WHITE, C6)];
            expected_result += *passed_table[relative_square(WHITE, C7)];

            assert(evaluate_pawns(pos) == expected_result);

            cout << "Evaluate tests complete!" << endl;
        }

        Bitboard set_file_rank_mask(File file_number, Rank rank_number)
        {
            Bitboard mask = 0ULL;

            for (Rank r = RANK_1; r <= RANK_8; ++r)
            {
                for (File f = FILE_A; f <= FILE_H; ++f)
                {
                    Square sq = make_square(f, r);

                    if (file_number != FILE_NONE)
                    {
                        if (f == file_number)
                            mask |= sq;
                    }

                    else if (rank_number != RANK_NONE)
                    {
                        if (r == rank_number)
                            mask |= sq;
                    }
                }
            }

            return mask;
        }

        Score evaluate(const Position& pos)
        {
            Score score[COLOUR_N];

            if (popcnt(pos.piece_bitboard[W_BISHOP]) >= 2)
                score[WHITE] += bishop_pair_bonus;

            if (popcnt(pos.piece_bitboard[B_BISHOP]) >= 2)
                score[BLACK] += bishop_pair_bonus;

            for (PieceType pt = PAWN; pt <= KING; ++pt)
            {
                Square sq;
                Colour us = WHITE, them;
                Piece piece;

            repeat:

                them = other_side(us);
                piece = make_piece(pt, us);

                Bitboard bb = pos.piece_bitboard[piece];
                
                while (bb)
                {
                    sq = pop_lsb(bb);
                    score[us] += *score_table[piece][sq];
                    if (pt >= PAWN && pt < KING)
                    {
                        score[us] += mobility[pt] * (popcnt(Bitboards::get_attacks(pos.occ_bitboard[BOTH], sq, pt) & ~pos.occ_bitboard[us]));
                        if (pt == ROOK)
                        {
                            if (!(file_masks[sq] & (pos.piece_bitboard[W_PAWN] | pos.piece_bitboard[B_PAWN])))
                                score[us] += rook_open_file_bonus;
                            else if (!(file_masks[sq] & pos.piece_bitboard[make_piece(PAWN, us)]))
                                score[us] += rook_semi_open_file_bonus;
                        }
                    }
                }

                if (us == WHITE)
                {
                    us = BLACK;
                    goto repeat;
                }
            }

            return score[WHITE] - score[BLACK];
        }

        Score evaluate_pawns(const Position& pos)
        {
            Piece piece;
            Bitboard bb;
            Colour us = WHITE, them;
            Bitboard enemy_pawns;

            Score score[COLOUR_N];

        repeat:

            them = other_side(us);
            enemy_pawns = pos.piece_bitboard[make_piece(PAWN, them)];
            piece = make_piece(PAWN, us);
            bb = pos.piece_bitboard[piece];

            while (bb)
            {
                Square sq = pop_lsb(bb);

                if (doubled_pawn(pos.piece_bitboard[piece], sq))
                    score[us] -= double_pawn_penalty;

                if (isolated_pawn(pos.piece_bitboard[piece], sq))
                    score[us] -= isolated_pawn_penalty;
                
                if (passed_pawn(enemy_pawns, sq, us))
                    score[us] += *passed_table[relative_square(us, sq)];
            }

            if (us == WHITE)
            {
                us = BLACK;
                goto repeat;
            }

            return score[WHITE] - score[BLACK];
        }

    } // namespace Eval

} // namespace Clovis