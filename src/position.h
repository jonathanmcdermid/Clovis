#pragma once

#include <string>

#include "bitboard.h"

namespace clovis {

constexpr std::string_view piece_str = " PNBRQK  pnbrqk";

// linked list implementation for board state info
struct BoardState {
    constexpr BoardState() = default;

    int castle{0}, hmc{0}, fmc{0}, ply_null{0}, game_phase{0};
    Piece captured_piece{NO_PIECE};
    Square en_passant{SQ_NONE};
    BoardState *prev{nullptr};
    Key key{0ULL}, pawn_key{0ULL};
};

struct Position {
    explicit Position(const char *fen) : side{WHITE}, bs{nullptr}, pc_table{}, pc_bb{}, occ_bb{} {
        set(fen);
    }
    std::string get_fen() const;
    void set(const char *fen);
    bool see_ge(Move move, int threshold) const;
    void do_null_move();
    void undo_null_move();
    [[nodiscard]] bool do_move(Move move);
    void undo_move(Move move);
    void print_position() const;
    void print_bitboards() const;
    Key make_key() const;
    Key make_pawn_key() const;
    void put_piece(Piece pc, Square sq);
    void replace_piece(Piece pc, Square sq);
    void remove_piece(Square sq);
    Square get_smallest_attacker(Bitboard attackers, Colour stm) const;
    bool is_repeat() const;

    template <bool NM>
    void new_board_state();
    template <Colour US>
    Square get_pinner(Square sq) const;
    template <Colour US>
    bool discovery_threat(Square sq) const;
    template <Colour US>
    bool is_insufficient() const;
    template <Colour US>
    bool is_attacked(Square sq) const;

    Bitboard consider_xray(Bitboard occ, Square to, PieceType pt) const;
    Bitboard attackers_to(Square sq) const;
    int get_game_phase() const;
    bool is_king_in_check() const;
    bool stm_has_promoted() const;
    bool is_draw_50() const;
    bool is_draw() const;
    bool is_material_draw() const;

    Colour side;
    BoardState *bs;
    std::array<Piece, SQ_N> pc_table;
    std::array<Bitboard, 15> pc_bb;
    std::array<Bitboard, COLOUR_N + 1> occ_bb;
};

// returns whether or not a square is attacked by opposing side
template <Colour US>
bool Position::is_attacked(const Square sq) const {
    return (
        (pc_bb[make_piece(PAWN, ~US)] & bitboards::pawn_attacks[US][sq]) ||
        (pc_bb[make_piece(KNIGHT, ~US)] & bitboards::knight_attacks[sq]) ||
        (pc_bb[make_piece(BISHOP, ~US)] & bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq)) ||
        (pc_bb[make_piece(ROOK, ~US)] & bitboards::get_attacks<ROOK>(occ_bb[BOTH], sq)) ||
        (pc_bb[make_piece(QUEEN, ~US)] & bitboards::get_attacks<QUEEN>(occ_bb[BOTH], sq)) ||
        (pc_bb[make_piece(KING, ~US)] & bitboards::king_attacks[sq]));
}

template <Colour US>
bool Position::is_insufficient() const {
    return (
        std::popcount(pc_bb[make_piece(PAWN, US)]) == 0 &&
        (std::popcount(pc_bb[make_piece(ROOK, US)]) == 0) &&
        (std::popcount(pc_bb[make_piece(QUEEN, US)]) == 0) &&
        (std::popcount(pc_bb[make_piece(KNIGHT, US)]) < 3) &&
        (std::popcount(pc_bb[make_piece(BISHOP, US)]) +
             std::popcount(pc_bb[make_piece(KNIGHT, US)]) <
         2));
}

// updates a bitboard of attackers after a piece has moved to include
// possible x ray attackers
inline Bitboard Position::consider_xray(
    const Bitboard occ, const Square to, const PieceType pt) const {
    return (pt == PAWN || pt == BISHOP)
               ? occ & (bitboards::get_attacks<BISHOP>(occ, to) &
                        (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]))
           : pt == ROOK  ? occ & (bitboards::get_attacks<ROOK>(occ, to) &
                                 (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK]))
           : pt == QUEEN ? consider_xray(occ, to, BISHOP) | consider_xray(occ, to, ROOK)
                         : 0ULL;
}

inline Bitboard Position::attackers_to(const Square sq) const {
    return (bitboards::pawn_attacks[BLACK][sq] & pc_bb[W_PAWN]) |
           (bitboards::pawn_attacks[WHITE][sq] & pc_bb[B_PAWN]) |
           (bitboards::knight_attacks[sq] & (pc_bb[W_KNIGHT] | pc_bb[B_KNIGHT])) |
           (bitboards::king_attacks[sq] & (pc_bb[W_KING] | pc_bb[B_KING])) |
           (bitboards::get_attacks<ROOK>(occ_bb[BOTH], sq) &
            (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK])) |
           (bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq) &
            (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]));
}

inline int Position::get_game_phase() const { return std::min(bs->game_phase, MAX_GAME_PHASE); }

inline bool Position::is_king_in_check() const {
    return (side == WHITE) ? is_attacked<WHITE>(bitboards::lsb(pc_bb[W_KING]))
                           : is_attacked<BLACK>(bitboards::lsb(pc_bb[B_KING]));
}

inline bool Position::stm_has_promoted() const {
    return pc_bb[make_piece(KNIGHT, side)] | pc_bb[make_piece(BISHOP, side)] |
           pc_bb[make_piece(ROOK, side)] | pc_bb[make_piece(QUEEN, side)];
}

inline bool Position::is_draw_50() const { return (bs->hmc >= 100); }

inline bool Position::is_draw() const { return is_repeat() || is_material_draw() || is_draw_50(); }

inline bool Position::is_material_draw() const {
    return is_insufficient<WHITE>() && is_insufficient<BLACK>();
}

} // namespace clovis
