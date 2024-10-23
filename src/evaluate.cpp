#include "evaluate.hpp"

#include <cstring>

namespace clovis::eval {

std::array<std::array<int, 2>, TI_MISC> T;

bool is_open_file(const Bitboard pawns, const File f) { return !(bitboards::FILE_MASKS[f] & pawns); }

bool is_doubled_pawn(const Bitboard bb, const Square sq) { return bitboards::multiple_bits(bb & bitboards::FILE_MASKS[sq]); }

bool is_isolated_pawn(const Bitboard bb, const Square sq) { return !(bb & ISOLATED_MASKS[sq]); }

template <Colour US> bool is_passed_pawn(const Bitboard bb, const Square sq) { return !(bb & PASSED_MASKS[US][sq]); }

template <Colour US> bool is_candidate_passer(const Position& pos, Square sq)
{
    // a candidate passers is an open file pawn with no square on its path controlled by more enemy pawns than friendly pawns
    if (pos.pieces<make_piece(PAWN, ~US)>() & ROOK_ON_PASSER_MASKS[US][sq]) { return false; }

    while (true)
    {
        if (std::popcount(bitboards::PAWN_ATTACKS[US][sq + pawn_push(US)] & pos.pieces<make_piece(PAWN, ~US)>()) >
            std::popcount(bitboards::PAWN_ATTACKS[~US][sq + pawn_push(US)] & pos.pieces<make_piece(PAWN, US)>()))
        {
            return false;
        }
        // we don't have to worry about starting on the 7th or 8th rank
        if (rank_of(sq) == relative_rank(US, RANK_6)) { break; }
        sq += pawn_push(US);
    }

    return true;
}

template <Colour US> bool is_outpost(const Square sq, const EvalInfo& ei)
{
    return (OUTPOST_MASKS[US] & sq & ~ei.potential_pawn_attacks[~US] & ei.pawn_attacks[US]);
}

template <Colour US> bool is_fianchetto(const Position& pos, const Square sq)
{
    return FIANCHETTO_BISHOP_MASK[US] & sq && // TODO: pos.get_pc(sq + pawn_push(US)) == make_piece(PAWN, US) &&
           CENTER_MASK[US] & bitboards::get_attacks<BISHOP>(pos.piece_types<PAWN>(), sq);
}

template <Colour US> bool is_tall_pawn(const Position& pos, const Square sq)
{
    return bitboards::multiple_bits(bitboards::PAWN_ATTACKS[US][sq] & pos.pieces<make_piece(PAWN, US)>());
}

template <Colour US, PieceType PT, bool TRACE> void king_danger(const Bitboard attacks, EvalInfo& ei)
{
    const Bitboard or_att_bb = attacks & OUTER_RING[ei.ksq[~US]];
    const Bitboard ir_att_bb = attacks & INNER_RING[ei.ksq[~US]];

    if (or_att_bb || ir_att_bb)
    {
        ei.weight[US] += INNER_RING_ATTACK[PT] * std::popcount(ir_att_bb) + OUTER_RING_ATTACK[PT] * std::popcount(or_att_bb);

        if constexpr (PT != PAWN) { ++ei.n_att[US]; }
        if constexpr (TRACE) { T[SAFETY_INNER_RING + PT][US] += std::popcount(ir_att_bb); }
        if constexpr (TRACE) { T[SAFETY_OUTER_RING + PT][US] += std::popcount(or_att_bb); }
    }
}

template <Colour US, PieceType PT> void psqt_trace(const Square sq)
{
    if constexpr (PT == PAWN) { ++T[PAWN_PSQT + SOURCE_32[relative_square(US, sq)]][US]; }
    if constexpr (PT == KNIGHT) { ++T[KNIGHT_PSQT + SOURCE_16[sq]][US]; }
    if constexpr (PT == BISHOP) { ++T[BISHOP_PSQT + SOURCE_16[sq]][US]; }
    if constexpr (PT == ROOK) { ++T[ROOK_PSQT + SOURCE_16[sq]][US]; }
    if constexpr (PT == QUEEN) { ++T[QUEEN_PSQT + SOURCE_16[relative_square(US, sq)]][US]; }
    if constexpr (PT == KING) { ++T[KING_PSQT + SOURCE_16[sq]][US]; }
}

template <Colour US, PieceType PT> Bitboard worthy_trades(const Position& pos)
{
    static_assert(PT >= KNIGHT && PT <= QUEEN);

    return PT == QUEEN  ? pos.pieces<make_piece(KING, ~US), make_piece(QUEEN, ~US)>()
           : PT == ROOK ? pos.pieces<make_piece(KING, ~US), make_piece(QUEEN, ~US), make_piece(ROOK, ~US)>()
                        : pos.get_occ_bb(~US) ^ pos.pieces<make_piece(PAWN, ~US)>();
}

template <Colour US, PieceType PT, bool SAFETY, bool TRACE> void evaluate_majors(const Position& pos, EvalInfo& ei, Score& score)
{
    static_assert(PT >= KNIGHT && PT <= QUEEN);

    Bitboard bb = pos.pieces<make_piece(PT, US)>();

    const Bitboard transparent_occ = PT == BISHOP ? pos.get_occ_bb(BOTH) ^ pos.pieces<W_QUEEN, B_QUEEN, make_piece(ROOK, ~US)>() ^ ei.ksq[~US]
                                     : PT == ROOK ? pos.get_occ_bb(BOTH) ^ pos.pieces<W_QUEEN, B_QUEEN, make_piece(ROOK, US)>() ^ ei.ksq[~US]
                                                  : pos.get_occ_bb(BOTH);

    while (bb)
    {
        Square sq = bitboards::pop_lsb(bb);
        score += PIECE_TABLE[make_piece(PT, US)][sq];
        Bitboard attacks = bitboards::get_attacks<PT>(transparent_occ, sq);

        if (pos.get_blockers<US>() & sq)
        {
            Bitboard pinners = pos.get_pinners<~US>();

            while (pinners)
            {
                const Square pinner_sq = bitboards::pop_lsb(pinners);

                if (bitboards::between_squares(ei.ksq[US], pinner_sq) & sq)
                {
                    attacks &= bitboards::between_squares(ei.ksq[US], pinner_sq) | pinner_sq;
                    break;
                }
            }
        }

        const Bitboard trades = worthy_trades<US, PT>(pos);
        const Bitboard safe_attacks = attacks & (~ei.pawn_attacks[~US] | trades);

        score += QUIET_MOBILITY_BONUS[PT] * std::popcount(safe_attacks & ~pos.get_occ_bb(BOTH));
        score += CAPTURE_MOBILITY_BONUS[PT] * std::popcount(safe_attacks & pos.get_occ_bb(~US));

        if constexpr (SAFETY) { king_danger<US, PT, TRACE>(safe_attacks, ei); }
        if constexpr (TRACE) { psqt_trace<US, PT>(sq); }
        if constexpr (TRACE) { T[QUIET_MOBILITY + PT][US] += std::popcount(safe_attacks & ~pos.get_occ_bb(BOTH)); }
        if constexpr (TRACE) { T[CAPTURE_MOBILITY + PT][US] += std::popcount(safe_attacks & pos.get_occ_bb(~US)); }
        if constexpr (PT == KNIGHT)
        {
            if (is_outpost<US>(sq, ei))
            {
                score += KNIGHT_OUTPOST_BONUS;
                if constexpr (TRACE) { ++T[KNIGHT_OUTPOST][US]; }
            }
        }
        if constexpr (PT == BISHOP)
        {
            if (bb)
            {
                score += BISHOP_PAIR_BONUS;
                if constexpr (TRACE) { ++T[BISHOP_PAIR][US]; }
            }
            if (is_fianchetto<US>(pos, sq))
            {
                score += FIANCHETTO_BONUS;
                if constexpr (TRACE) { ++T[FIANCHETTO][US]; }
            }
            else
            {
                if (is_outpost<US>(sq, ei))
                {
                    score += BISHOP_OUTPOST_BONUS;
                    if constexpr (TRACE) { ++T[BISHOP_OUTPOST][US]; }
                }
                if (is_tall_pawn<US>(pos, sq))
                {
                    score -= TALL_PAWN_PENALTY;
                    if constexpr (TRACE) { --T[TALL_PAWN][US]; }
                }
            }
        }
        if constexpr (PT == ROOK)
        {
            if (!(bitboards::FILE_MASKS[sq] & (pos.piece_types<PAWN>())))
            {
                score += ROOK_OPEN_FILE_BONUS;
                if constexpr (TRACE) { ++T[ROOK_FULL][US]; }
            }
            else
            {
                if (!(bitboards::FILE_MASKS[sq] & pos.pieces<make_piece(PAWN, US)>()))
                {
                    score += ROOK_SEMI_OPEN_FILE_BONUS;
                    if constexpr (TRACE) { ++T[ROOK_SEMI][US]; }
                }
                else if (bitboards::FILE_MASKS[sq] & pos.pieces<make_piece(PAWN, ~US)>())
                {
                    score -= ROOK_CLOSED_FILE_PENALTY;
                    if constexpr (TRACE) { --T[ROOK_CLOSED][US]; }
                }
                if (attacks & ROOK_ON_PASSER_MASKS[US][sq] & ei.passers[US])
                {
                    score += ROOK_ON_OUR_PASSER_FILE;
                    if constexpr (TRACE) { ++T[ROOK_OUR_PASSER][US]; }
                }
                if (safe_attacks & ROOK_ON_PASSER_MASKS[~US][sq] & ei.passers[~US])
                {
                    score += ROOK_ON_THEIR_PASSER_FILE;
                    if constexpr (TRACE) { ++T[ROOK_THEIR_PASSER][US]; }
                }
            }
            if (rank_of(sq) == relative_rank(US, RANK_7) && rank_of(ei.ksq[~US]) == relative_rank(US, RANK_8))
            {
                score += ROOK_ON_SEVENTH_RANK;
                if constexpr (TRACE) { ++T[ROOK_ON_SEVENTH][US]; }
            }
        }
        if constexpr (PT == QUEEN)
        {
            if (pos.weak_queen<US>(sq))
            {
                score -= WEAK_QUEEN_PENALTY;
                if constexpr (TRACE) { --T[WEAK_QUEEN][US]; }
            }
        }
    }
}

template <Colour US, bool TRACE> Score evaluate_all(const Position& pos, EvalInfo& ei)
{
    Score score;

    // Conditions for whether or not we consider king safety
    if (pos.pieces<make_piece(QUEEN, US)>() && pos.get_game_phase() > 8)
    {
        // we use the number of attacking major pieces in king safety, which are not calculated yet
        evaluate_majors<US, KNIGHT, true, TRACE>(pos, ei, score);
        evaluate_majors<US, BISHOP, true, TRACE>(pos, ei, score);
        evaluate_majors<US, ROOK, true, TRACE>(pos, ei, score);
        evaluate_majors<US, QUEEN, true, TRACE>(pos, ei, score);

        // we don't count kings or pawns in n_att so the max should be 7, barring promotion trolling
        assert(ei.n_att[US] < 8);

        // virtual mobility is a metric that considers the number of squares near the king that are in danger
        const int virtual_mobility = std::popcount(
            // relevant squares are the same as the queen moves from the kings position (king moves + sliders)
            bitboards::get_attacks<QUEEN>
            // for occupancy, we consider the pieces defending the king and attacking pawns TODO: why attacking pawns?
            (pos.get_occ_bb(~US) | pos.pieces<make_piece(PAWN, US)>(), ei.ksq[~US]) &
            // ignore squares that are defended by the kings pawns
            ~ei.pawn_attacks[~US]);

        if (virtual_mobility > 4)
        {
            ei.weight[US] += VIRTUAL_MOBILITY * std::min(13, virtual_mobility);
            if constexpr (TRACE) { T[SAFETY_VIRTUAL_MOBILITY][US] = std::min(13, virtual_mobility); }
        }

        if (ei.weight[US] > 0)
        {
            // we are calculating the king safety of the enemy, so we add instead of subtract
            score.mg += ei.weight[US] * ei.weight[US] / (720 - ATTACK_FACTOR * ei.n_att[US]);
            if constexpr (TRACE) { T[SAFETY_N_ATT][US] = ei.n_att[US]; }
        }
        else if constexpr (TRACE)
        {
            for (int i = TI_SAFETY; i < TI_N; ++i) { T[i][US] = 0; }
        }
    }
    else
    {
        evaluate_majors<US, KNIGHT, false, TRACE>(pos, ei, score);
        evaluate_majors<US, BISHOP, false, TRACE>(pos, ei, score);
        evaluate_majors<US, ROOK, false, TRACE>(pos, ei, score);
        evaluate_majors<US, QUEEN, false, TRACE>(pos, ei, score);

        if constexpr (TRACE)
        {
            for (int i = TI_SAFETY; i < TI_N; ++i) { T[i][US] = 0; }
        }
    }

    return score;
}

template <Colour US, bool TRACE> Score evaluate_pawns(const Position& pos, EvalInfo& ei)
{
    Score score;
    Bitboard bb = pos.pieces<make_piece(PAWN, US)>();

    while (bb)
    {
        const Square sq = bitboards::pop_lsb(bb);

        if constexpr (TRACE) { psqt_trace<US, PAWN>(sq); }

        score += PIECE_TABLE[make_piece(PAWN, US)][sq];

        if (is_doubled_pawn(pos.pieces<make_piece(PAWN, US)>(), sq))
        {
            score -= DOUBLE_PAWN_PENALTY;
            if constexpr (TRACE) { --T[DOUBLE_PAWN][US]; }
        }
        if (is_isolated_pawn(pos.pieces<make_piece(PAWN, US)>(), sq))
        {
            score -= ISOLATED_PAWN_PENALTY;
            if constexpr (TRACE) { --T[ISOLATED_PAWN][US]; }
        }
        if (is_passed_pawn<US>(pos.pieces<make_piece(PAWN, ~US)>(), sq))
        {
            ei.passers[US] |= sq;
            if (rank_of(sq) != relative_rank(US, RANK_7))
            {
                score += PASSED_TABLE[US][sq];
                if constexpr (TRACE) { ++T[PASSED_PAWN_PSQT + SOURCE_32[relative_square(US, sq)]][US]; }
            }
        }
        else if (is_candidate_passer<US>(pos, sq))
        {
            score += CANDIDATE_PASSER[relative_rank(US, rank_of(sq))];
            if constexpr (TRACE) { ++T[CANDIDATE_PASSER_PSQT + relative_rank(US, rank_of(sq))][US]; }
        }

        ei.pawn_attacks[US] |= bitboards::PAWN_ATTACKS[US][sq];
        ei.potential_pawn_attacks[US] |= OUTPOST_PAWN_MASKS[US][sq];
    }

    king_danger<US, PAWN, TRACE>(bitboards::shift<pawn_push(US)>(pos.pieces<make_piece(PAWN, US)>()), ei);

    const File kf = file_of(ei.ksq[US]);
    const File cf = kf == FILE_H ? FILE_G : kf == FILE_A ? FILE_B : kf;

    for (File f = cf - 1; f <= cf + 1; ++f)
    {
        if (const Bitboard fp = pos.pieces<make_piece(PAWN, US)>() & bitboards::FILE_MASKS[f]; fp)
        {
            //	ei.weight[~US] -= *shield_table[US][US == WHITE ?
            // bitboards::lsb(fp) : msb(fp)]; 	if constexpr (TRACE)
            //--T[SAFETY_PAWN_SHIELD + source32[relative_square(US, US ==
            // WHITE ?
            // bitboards::lsb(fp) : msb(fp))]][~US];
        }
        else
        {
            ei.weight[~US] += SHIELD_TABLE[0][f];
            if constexpr (TRACE) { ++T[SAFETY_PAWN_SHIELD + SOURCE_32[f]][~US]; }
        }

        if (is_open_file(pos.piece_types<PAWN>(), f))
        {
            if (f == kf)
            {
                score -= KING_OPEN_PENALTY;
                if constexpr (TRACE) { --T[KING_OPEN][US]; }
            }
            else
            {
                score -= KING_ADJACENT_OPEN_PENALTY;
                if constexpr (TRACE) { --T[KING_ADJ_OPEN][US]; }
            }
        }
    }

    score += PIECE_TABLE[make_piece(KING, US)][ei.ksq[US]];
    if constexpr (TRACE) { psqt_trace<US, KING>(ei.ksq[US]); }

    return score;
}

template <bool TRACE> int evaluate(const Position& pos)
{
    Score score = (pos.get_side() == WHITE) ? TEMPO_BONUS : -TEMPO_BONUS;

    if constexpr (TRACE) { memset(T.data(), 0, sizeof(T)); }
    if constexpr (TRACE) { ++T[TEMPO][pos.get_side()]; }

    EvalInfo ei(tt.probe_pawn(pos.get_pawn_key()));

    if (TRACE || ei.key != pos.get_pawn_key())
    {
        ei = EvalInfo();
        ei.key = pos.get_pawn_key();
        ei.ksq[WHITE] = bitboards::lsb(pos.pieces<W_KING>());
        ei.ksq[BLACK] = bitboards::lsb(pos.pieces<B_KING>());
        ei.score = evaluate_pawns<WHITE, TRACE>(pos, ei) - evaluate_pawns<BLACK, TRACE>(pos, ei);
        tt.new_pawn_entry(ei);
    }

    score += ei.score + evaluate_all<WHITE, TRACE>(pos, ei) - evaluate_all<BLACK, TRACE>(pos, ei);

    const int game_phase = std::min(pos.get_game_phase(), MAX_GAME_PHASE);
    int eval = (score.mg * game_phase + score.eg * (MAX_GAME_PHASE - game_phase)) / MAX_GAME_PHASE;
    if (pos.get_side() == BLACK) { eval = -eval; }

    /*
    int scaling;

    if (game_phase == 2 && popcount(pos.pieces<W_BISHOP>()) == 1 &&
    popcount(pos.pieces<B_BISHOP>()) == 1
            && bool(pos.pieces<W_BISHOP>() & light_mask) ==
    bool(pos.pieces<B_BISHOP>() & dark_mask)) scaling =
    opposite_bishops_scaling; else scaling = MAX_SCALING;

    if (scaling < MAX_SCALING)
            eval = eval * scaling / MAX_SCALING;
    */

    return eval;
}

// explicit template instantiations
template bool is_passed_pawn<WHITE>(const Bitboard bb, const Square sq);
template bool is_passed_pawn<BLACK>(const Bitboard bb, const Square sq);
template bool is_candidate_passer<WHITE>(const Position& pos, Square sq);
template bool is_candidate_passer<BLACK>(const Position& pos, Square sq);
template bool is_outpost<WHITE>(const Square sq, const EvalInfo& ei);
template bool is_outpost<BLACK>(const Square sq, const EvalInfo& ei);
template bool is_fianchetto<WHITE>(const Position& pos, const Square sq);
template bool is_fianchetto<BLACK>(const Position& pos, const Square sq);
template bool is_tall_pawn<WHITE>(const Position& pos, const Square sq);
template bool is_tall_pawn<BLACK>(const Position& pos, const Square sq);
template int evaluate<true>(const Position& pos);
template int evaluate<false>(const Position& pos);

} // namespace clovis::eval
