#include "movepicker.h"

using namespace std;

namespace Clovis {

	namespace MovePick {

        int history_table[2 * 64 * 64];
        Move killers[2 * MAX_PLY];
        int killer_bonus[MAX_PLY + 1];

        // MVV-LVA lookup table [attacker][victim]
        int mvv_lva[15][15];

        inline static bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
            return lhs.score > rhs.score;
        }

        // initialize lookup tables and precalculated values
        void init_movepicker()
        {
            for (int i = 0; i <= MAX_PLY; ++i)
                killer_bonus[i] = min(i * i, 400);

            for (PieceType att = PAWN; att <= KING; ++att)
            {
                for (PieceType vic = PAWN; vic <= KING; ++vic)
                {
                    mvv_lva[make_piece(att, WHITE)][make_piece(vic, BLACK)] = (vic - 1) * 8 - att + 6;
                    mvv_lva[make_piece(att, BLACK)][make_piece(vic, WHITE)] = (vic - 1) * 8 - att + 6;
                }
            }
        }

        void MovePicker::update_history(Move best_move, int depth)
        {
            // when stage = init captures, it means the quiet TT move caused a beta cutoff
            // when this happens, the tt move history entry is not updated
            assert(stage >= QUIETS);
            assert(!move_capture(best_move));

            ScoredMove* last_searched_quiet = (stage == FINISHED) ? last : curr;

            for (ScoredMove* sm = end_bad_caps; sm < last_searched_quiet; ++sm)
            {
                assert(!move_capture(*sm));
                int* history_entry = get_history_entry_ptr(pos.side_to_move(), *sm);
                *history_entry += (*sm == best_move)
                    ? 32 * killer_bonus[depth] - *history_entry * killer_bonus[depth] / 512
                    :-32 * killer_bonus[depth] - *history_entry * killer_bonus[depth] / 512;
            }
        }

		// return the next ordered move
		Move MovePicker::get_next(bool skip_quiets)
		{
            switch (stage)
            {
            case TT_MOVE:
                ++stage;
                if (tt_move != MOVE_NONE && (!skip_quiets || move_capture(tt_move)))
                    return tt_move;
            case INIT_CAPTURES:
                curr = end_bad_caps = moves;
                last = gen_cap_moves<ScoredMove>(pos, moves);
                score_captures();
                sort(moves, last, sm_score_comp);
                ++stage;
            case WINNING_CAPTURES:
                while (curr < last)
                {
                    assert(move_capture(*curr));
                    if (curr->move == tt_move)
                        ++curr;
                    else if (pos.see(*curr))
                        return *curr++;
                    else
                        *end_bad_caps++ = *curr++;
                }
                ++stage;
            case INIT_QUIETS:
                if (!skip_quiets)
                {
                    curr = end_bad_caps;
                    last = gen_quiet_moves<ScoredMove>(pos, moves);
                    score_quiets();
                    sort(end_bad_caps, last, sm_score_comp);
                }
                ++stage;
            case QUIETS:
                while (!skip_quiets && curr < last)
                {
                    assert(!move_capture(*curr));
                    if (*curr != tt_move)
                        return *curr++;
                    ++curr;
                }
                curr = moves;
                ++stage;
            case LOSING_CAPTURES:
                while (curr < end_bad_caps)
                {
                    assert(move_capture(*curr));
                    if (*curr != tt_move)
                        return *curr++;
                    ++curr;
                }
                ++stage;
                break;
            default:
                break;
            }

	        return MOVE_NONE;
		}

        void MovePicker::score_captures()
        {
            for (ScoredMove* sm = moves; sm < last; ++sm)
                // promotions supercede mvv-lva
                sm->score = mvv_lva[move_piece_type(*sm)][pos.piece_on(move_to_sq(*sm))] + move_promotion_type(*sm) * (1 << 6);
        }

        void MovePicker::score_quiets()
        {
            for (ScoredMove* sm = end_bad_caps; sm < last; ++sm)
            {
                if (killers[ply * 2] == *sm)
                    sm->score = 22000;
                else if (killers[ply * 2 + 1] == *sm)
                    sm->score = 21000;
                else if (move_promotion_type(*sm) || move_castling(*sm))
                    sm->score = 20000 + move_promotion_type(*sm);
                else
                    sm->score = get_history_entry(pos.side_to_move(), *sm);
            }
        }

        void MovePicker::print()
        {
            cout << "\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\tscore\n\n";

            int move_count = 0;
            for (ScoredMove* sm = moves; sm != last; ++sm, ++move_count)
            {
                cout << sq2str(move_from_sq(*sm))
                    << sq2str(move_to_sq(*sm))
                    << piece_str[move_promotion_type(*sm)] << '\t'
                    << piece_str[move_piece_type(*sm)] << '\t'
                    << int(move_capture(*sm)) << '\t'
                    << int(move_double(*sm)) << '\t'
                    << int(move_enpassant(*sm)) << '\t'
                    << int(move_castling(*sm)) << '\t'
                    << sm->score << '\n';

            }
            cout << "\n\nTotal move count:" << move_count;
        }

        void test_movepicker()
        {
            Position pos("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -");

            Move tt_move = encode_move(E2, A6, W_BISHOP, NO_PIECE, 0, 0, 0, 0);
            MovePick::MovePicker mp(pos, 0, tt_move);

            Move curr_move;

            // valid move
            Move primary = encode_move(C3, A4, W_KNIGHT, NO_PIECE, 0, 0, 0, 0);
            // invalid move for this position
            Move secondary = encode_move(A2, A6, W_ROOK, NO_PIECE, 0, 0, 0, 0);

            vector<Move> win_caps;
            vector<Move> quiets;
            vector<Move> lose_caps;

            MovePick::update_killers(primary, 0);
            MovePick::update_killers(primary, 0);
            MovePick::update_killers(secondary, 0);

            while ((curr_move = mp.get_next(false)) != MOVE_NONE)
            {
                switch (mp.get_stage())
                {
                case INIT_CAPTURES:
                    assert(curr_move == tt_move);
                    break;
                case WINNING_CAPTURES:
                    assert(curr_move != tt_move);
                    // if there is a move preceding curr, it must have a higher value victim or an equal value victim and an equal or lower valued attacker
                    // test does not account for promotions
                    assert(win_caps.empty()
                        || pos.piece_on(move_to_sq(curr_move)) <= pos.piece_on(move_to_sq(win_caps.back()))
                        || (pos.piece_on(move_to_sq(curr_move)) == pos.piece_on(move_to_sq(win_caps.back()))
                            && pos.piece_on(move_from_sq(curr_move)) >= pos.piece_on(move_from_sq(win_caps.back()))));
                    win_caps.push_back(curr_move);
                    break;
                case QUIETS:
                    // if curr is primary, must be first quiet
                    // or if curr is secondary, must be first quiet or preceded by primary
                    // or if curr is promotion, must only be preceded by killers or equal or better promotions
                    // or if curr is castle, must only be preceded by killers, promos, or other castles
                    assert(curr_move != tt_move);
                    assert((curr_move == primary) == quiets.empty()
                        || (curr_move == secondary) == (quiets.empty() || quiets[0] == primary)
                        || ((move_promotion_type(curr_move) != NO_PIECE) == quiets.empty()
                            || quiets.back() == primary
                            || quiets.back() == secondary
                            || move_promotion_type(quiets.back()) >= move_promotion_type(curr_move))
                        || ((move_castling(curr_move) == quiets.empty())
                            || quiets.back() == primary
                            || quiets.back() == secondary
                            || move_promotion_type(quiets.back()) != NO_PIECE
                            || move_castling(quiets.back())));
                    // if curr is not a special move, must only be preceded by special moves, or moves with higher HH score
                    quiets.push_back(curr_move);
                    break;
                case LOSING_CAPTURES:
                    assert(curr_move != tt_move);
                    lose_caps.push_back(curr_move);
                    break;
                }
            }
        }

	} // namespace MovePick

} // namespace Clovis
