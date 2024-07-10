#include "evaluate.h"

#include <cstring>

namespace clovis::eval {

std::array<std::array<int, PHASE_N>, TI_MISC> T;

bool is_doubled_pawn(const Bitboard bb, const Square sq) { return bitboards::multiple_bits(bb & bitboards::file_masks[sq]); }

bool is_isolated_pawn(const Bitboard bb, const Square sq) { return !(bb & isolated_masks[sq]); }

template <Colour US> bool is_passed_pawn(const Bitboard bb, const Square sq) { return !(bb & passed_masks[US][sq]); }

template <Colour US> bool is_candidate_passer(const Position& pos, Square sq)
{
    if (pos.pc_bb[make_piece(PAWN, ~US)] & rook_on_passer_masks[US][sq]) { return false; }

    while (true) {
        if (std::popcount(bitboards::pawn_attacks[US][sq + pawn_push(US)] & pos.pc_bb[make_piece(PAWN, ~US)]) >
            std::popcount(bitboards::pawn_attacks[~US][sq + pawn_push(US)] & pos.pc_bb[make_piece(PAWN, US)]))
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
    return (outpost_masks[US] & sq & ~ei.potential_pawn_attacks[~US] & ei.pawn_attacks[US]);
}

template <Colour US> bool is_fianchetto(const Position& pos, const Square sq)
{
    return fianchetto_bishop_mask[US] & sq && center_mask[US] & bitboards::get_attacks<BISHOP>(pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN], sq);
}

inline bool is_open_file(const Position& pos, const File f) { return !(bitboards::file_masks[f] & (pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN])); }

template <Colour US, PieceType PT, bool TRACE> void king_danger(const Bitboard attacks, EvalInfo& ei)
{
    const Bitboard or_att_bb = attacks & outer_ring[ei.ksq[~US]];
    const Bitboard ir_att_bb = attacks & inner_ring[ei.ksq[~US]];

    if (or_att_bb || ir_att_bb)
    {
        ei.weight[US] += inner_ring_attack[PT] * std::popcount(ir_att_bb) + outer_ring_attack[PT] * std::popcount(or_att_bb);

        if constexpr (PT != PAWN) { ++ei.n_att[US]; }
        if constexpr (TRACE) { T[SAFETY_INNER_RING + PT][US] += std::popcount(ir_att_bb); }
        if constexpr (TRACE) { T[SAFETY_OUTER_RING + PT][US] += std::popcount(or_att_bb); }
    }
}

template <Colour US, PieceType PT> void psqt_trace(const Square sq)
{
    if constexpr (PT == PAWN) { ++T[PAWN_PSQT + source32[relative_square(US, sq)]][US]; }
    if constexpr (PT == KNIGHT) { ++T[KNIGHT_PSQT + source16[sq]][US]; }
    if constexpr (PT == BISHOP) { ++T[BISHOP_PSQT + source16[sq]][US]; }
    if constexpr (PT == ROOK) { ++T[ROOK_PSQT + source16[sq]][US]; }
    if constexpr (PT == QUEEN) { ++T[QUEEN_PSQT + source32[relative_square(US, sq)]][US]; }
    if constexpr (PT == KING) { ++T[KING_PSQT + source16[sq]][US]; }
}

template <Colour US, PieceType PT> Bitboard worthy_trades(const Position& pos)
{
    static_assert(PT >= KNIGHT && PT <= QUEEN);

    return (PT == QUEEN)  ? pos.pc_bb[make_piece(KING, ~US)] | pos.pc_bb[make_piece(QUEEN, ~US)]
           : (PT == ROOK) ? pos.pc_bb[make_piece(KING, ~US)] | pos.pc_bb[make_piece(QUEEN, ~US)] | pos.pc_bb[make_piece(ROOK, ~US)]
                          : pos.occ_bb[~US] ^ pos.pc_bb[make_piece(PAWN, ~US)];
}

template <Colour US, PieceType PT, bool SAFETY, bool TRACE> void evaluate_majors(const Position& pos, EvalInfo& ei, Score& score)
{
    static_assert(PT >= KNIGHT && PT <= QUEEN);

    Bitboard bb = pos.pc_bb[make_piece(PT, US)];

    const Bitboard transparent_occ =
        PT == BISHOP ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK, ~US)] ^ ei.ksq[~US]
        : PT == ROOK ? pos.occ_bb[BOTH] ^ pos.pc_bb[W_QUEEN] ^ pos.pc_bb[B_QUEEN] ^ pos.pc_bb[make_piece(ROOK, US)] ^ ei.ksq[~US]
                     : pos.occ_bb[BOTH];

    while (bb)
    {
        Square sq = bitboards::pop_lsb(bb);
        score += piece_table[make_piece(PT, US)][sq];
        Bitboard attacks = bitboards::get_attacks<PT>(transparent_occ, sq);
        Square pinner = pos.get_pinner<US>(sq);

        if (pinner != SQ_NONE) { attacks &= bitboards::between_squares(ei.ksq[US], pinner) | pinner; }

        const Bitboard trades = worthy_trades<US, PT>(pos);
        const Bitboard safe_attacks = attacks & (~ei.pawn_attacks[~US] | trades);

        score += quiet_mobility[PT] * std::popcount(safe_attacks & ~pos.occ_bb[BOTH]);
        score += capture_mobility[PT] * std::popcount(safe_attacks & pos.occ_bb[~US]);

        if constexpr (SAFETY) { king_danger<US, PT, TRACE>(safe_attacks, ei); }
        if constexpr (TRACE) { psqt_trace<US, PT>(sq); }
        if constexpr (TRACE) { T[QUIET_MOBILITY + PT][US] += std::popcount(safe_attacks & ~pos.occ_bb[BOTH]); }
        if constexpr (TRACE) { T[CAPTURE_MOBILITY + PT][US] += std::popcount(safe_attacks & pos.occ_bb[~US]); }
        if constexpr (PT == KNIGHT)
        {
            if (is_outpost<US>(sq, ei))
            {
                score += knight_outpost_bonus;
                if constexpr (TRACE) { ++T[KNIGHT_OUTPOST][US]; }
            }
        }
        if constexpr (PT == BISHOP)
        {
            if (bb)
            {
                score += bishop_pair_bonus;
                if constexpr (TRACE) { ++T[BISHOP_PAIR][US]; }
            }
            if (is_fianchetto<US>(pos, sq))
            {
                score += fianchetto_bonus;
                if constexpr (TRACE) { ++T[FIANCHETTO][US]; }
            }
            else
            {
                if (is_outpost<US>(sq, ei))
                {
                    score += bishop_outpost_bonus;
                    if constexpr (TRACE) { ++T[BISHOP_OUTPOST][US]; }
                }
                if (bitboards::multiple_bits(bitboards::pawn_attacks[US][sq] & pos.pc_bb[make_piece(PAWN, US)]))
                {
                    score -= tall_pawn_penalty;
                    if constexpr (TRACE) { --T[TALL_PAWN][US]; }
                }
            }
        }
        if constexpr (PT == ROOK)
        {
            if (!(bitboards::file_masks[sq] & (pos.pc_bb[W_PAWN] | pos.pc_bb[B_PAWN])))
            {
                score += rook_open_file_bonus;
                if constexpr (TRACE) { ++T[ROOK_FULL][US]; }
            }
            else
            {
                if (!(bitboards::file_masks[sq] & pos.pc_bb[make_piece(PAWN, US)]))
                {
                    score += rook_semi_open_file_bonus;
                    if constexpr (TRACE) { ++T[ROOK_SEMI][US]; }
                }
                else if (bitboards::file_masks[sq] & pos.pc_bb[make_piece(PAWN, ~US)])
                {
                    score -= rook_closed_file_penalty;
                    if constexpr (TRACE) { --T[ROOK_CLOSED][US]; }
                }
                if (attacks & rook_on_passer_masks[US][sq] & ei.passers[US])
                {
                    score += rook_on_our_passer_file;
                    if constexpr (TRACE) { ++T[ROOK_OUR_PASSER][US]; }
                }
                if (safe_attacks & rook_on_passer_masks[~US][sq] & ei.passers[~US])
                {
                    score += rook_on_their_passer_file;
                    if constexpr (TRACE) { ++T[ROOK_THEIR_PASSER][US]; }
                }
            }
            if (rank_of(sq) == relative_rank(US, RANK_7) && rank_of(ei.ksq[~US]) == relative_rank(US, RANK_8))
            {
                score += rook_on_seventh;
                if constexpr (TRACE) { ++T[ROOK_ON_SEVENTH][US]; }
            }
        }
        if constexpr (PT == QUEEN)
        {
            if (pos.discovery_threat<US>(sq))
            {
                score -= weak_queen_penalty;
                if constexpr (TRACE) { --T[WEAK_QUEEN][US]; }
            }
        }
    }
}

template <Colour US, bool TRACE> Score evaluate_all(const Position& pos, EvalInfo& ei)
{
    Score score;

    if (pos.pc_bb[make_piece(QUEEN, US)] && pos.get_game_phase() > 8)
    {
        evaluate_majors<US, KNIGHT, true, TRACE>(pos, ei, score);
        evaluate_majors<US, BISHOP, true, TRACE>(pos, ei, score);
        evaluate_majors<US, ROOK, true, TRACE>(pos, ei, score);
        evaluate_majors<US, QUEEN, true, TRACE>(pos, ei, score);

        // we don't count kings or pawns in n_att so the max should be 7, barring promotion trolling
        assert(ei.n_att[US] < 10);

        if (const int mob =
                std::popcount(bitboards::get_attacks<QUEEN>(pos.occ_bb[~US] ^ pos.pc_bb[make_piece(PAWN, US)], ei.ksq[~US]) & ~ei.pawn_attacks[~US]);
            mob > 4)
        {
            ei.weight[US] += virtual_mobility * std::min(13, mob);
            if constexpr (TRACE) { T[SAFETY_VIRTUAL_MOBILITY][US] = std::min(13, mob); }
        }

        if (ei.weight[US] > 0)
        {
            score.mg += ei.weight[US] * ei.weight[US] / (720 - attack_factor * ei.n_att[US]);
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
    Bitboard bb = pos.pc_bb[make_piece(PAWN, US)];

    while (bb)
    {
        const Square sq = bitboards::pop_lsb(bb);

        if constexpr (TRACE) { psqt_trace<US, PAWN>(sq); }

        score += piece_table[make_piece(PAWN, US)][sq];

        if (is_doubled_pawn(pos.pc_bb[make_piece(PAWN, US)], sq))
        {
            score -= double_pawn_penalty;
            if constexpr (TRACE) { --T[DOUBLE_PAWN][US]; }
        }
        if (is_isolated_pawn(pos.pc_bb[make_piece(PAWN, US)], sq))
        {
            score -= isolated_pawn_penalty;
            if constexpr (TRACE) { --T[ISOLATED_PAWN][US]; }
        }
        if (is_passed_pawn<US>(pos.pc_bb[make_piece(PAWN, ~US)], sq))
        {
            ei.passers[US] |= sq;
            if (rank_of(sq) != relative_rank(US, RANK_7))
            {
                score += passed_table[US][sq];
                if constexpr (TRACE) { ++T[PASSED_PAWN + source32[relative_square(US, sq)]][US]; }
            }
        }
        else if (is_candidate_passer<US>(pos, sq))
        {
            score += candidate_passer[relative_rank(US, rank_of(sq))];
            if constexpr (TRACE) { ++T[CANDIDATE_PASSER + relative_rank(US, rank_of(sq))][US]; }
        }

        ei.pawn_attacks[US] |= bitboards::pawn_attacks[US][sq];
        ei.potential_pawn_attacks[US] |= outpost_pawn_masks[US][sq];
    }

    king_danger<US, PAWN, TRACE>(bitboards::shift<pawn_push(US)>(pos.pc_bb[make_piece(PAWN, US)]), ei);

    const File kf = file_of(ei.ksq[US]);
    const File cf = kf == FILE_H ? FILE_G : kf == FILE_A ? FILE_B : kf;

    for (File f = cf - 1; f <= cf + 1; ++f)
    {
        if (const Bitboard fp = pos.pc_bb[make_piece(PAWN, US)] & bitboards::file_masks[f]; fp)
        {
            //	ei.weight[~US] -= *shield_table[US][US == WHITE ?
            // bitboards::lsb(fp) : msb(fp)]; 	if constexpr (TRACE)
            //--T[SAFETY_PAWN_SHIELD + source32[relative_square(US, US ==
            // WHITE ?
            // bitboards::lsb(fp) : msb(fp))]][~US];
        }
        else
        {
            ei.weight[~US] += shield_table[0][f];
            if constexpr (TRACE) { ++T[SAFETY_PAWN_SHIELD + source32[f]][~US]; }
        }

        if (is_open_file(pos, f))
        {
            if (f == kf)
            {
                score -= king_open_penalty;
                if constexpr (TRACE) { --T[KING_OPEN][US]; }
            }
            else
            {
                score -= king_adjacent_open_penalty;
                if constexpr (TRACE) { --T[KING_ADJ_OPEN][US]; }
            }
        }
    }

    score += piece_table[make_piece(KING, US)][ei.ksq[US]];
    if constexpr (TRACE) { psqt_trace<US, KING>(ei.ksq[US]); }

    return score;
}

template <bool TRACE> int evaluate(const Position& pos)
{
    Score score = (pos.side == WHITE) ? tempo_bonus : -tempo_bonus;

    if constexpr (TRACE) { memset(T.data(), 0, sizeof(T)); }
    if constexpr (TRACE) { ++T[TEMPO][pos.side]; }

    EvalInfo ei(tt.probe_pawn(pos.bs->pawn_key));

    if (TRACE || ei.key != pos.bs->pawn_key)
    {
        ei = EvalInfo();
        ei.key = pos.bs->pawn_key;
        ei.ksq[WHITE] = bitboards::lsb(pos.pc_bb[W_KING]);
        ei.ksq[BLACK] = bitboards::lsb(pos.pc_bb[B_KING]);
        ei.score = evaluate_pawns<WHITE, TRACE>(pos, ei) - evaluate_pawns<BLACK, TRACE>(pos, ei);
        tt.new_pawn_entry(ei);
    }

    score += ei.score + evaluate_all<WHITE, TRACE>(pos, ei) - evaluate_all<BLACK, TRACE>(pos, ei);

    const int game_phase = pos.get_game_phase();
    int eval = (score.mg * game_phase + score.eg * (MAX_GAME_PHASE - game_phase)) / MAX_GAME_PHASE;
    if (pos.side == BLACK) { eval = -eval; }

    /*
    int scaling;

    if (game_phase == 2 && popcount(pos.pc_bb[W_BISHOP]) == 1 &&
    popcount(pos.pc_bb[B_BISHOP]) == 1
            && bool(pos.pc_bb[W_BISHOP] & light_mask) ==
    bool(pos.pc_bb[B_BISHOP] & dark_mask)) scaling =
    opposite_bishops_scaling; else scaling = MAX_SCALING;

    if (scaling < MAX_SCALING)
            eval = eval * scaling / MAX_SCALING;
    */

    return eval;
}

// explicit template instantiations
template int evaluate<true>(const Position& pos);
template int evaluate<false>(const Position& pos);

} // namespace clovis::eval
