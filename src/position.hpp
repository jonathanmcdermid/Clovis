#pragma once

#include <memory>
#include <string>

#include "bitboard.hpp"

namespace clovis {

constexpr std::string_view PIECE_STR = " PNBRQK  pnbrqk";

struct Position
{
  public:
    explicit Position(const char* fen) { set(fen); }

    // State Modifiers
    void set(const char* fen);
    void do_null_move();
    void undo_null_move();
    [[nodiscard]] bool do_move(Move move);
    void undo_move(Move move);

    // State Queries
    [[nodiscard]] std::string get_fen() const;
    [[nodiscard]] bool is_king_in_check() const;
    [[nodiscard]] bool is_stm_major() const;
    [[nodiscard]] bool is_draw() const;
    [[nodiscard]] int see(Move move) const;

    template <Colour US> [[nodiscard]] Square get_pinner(Square sq) const;
    template <Colour US> [[nodiscard]] bool is_discovery_threat(Square sq) const;
    template <Colour US> [[nodiscard]] bool is_attacked(Square sq) const;

    // Member Accessors
    [[nodiscard]] Colour get_side() const { return side; }
    [[nodiscard]] Piece get_pc(Square sq) const { return pc_table[sq]; }
    [[nodiscard]] Bitboard get_pc_bb(Piece pc) const { return pc_bb[pc]; }
    [[nodiscard]] Bitboard get_occ_bb(Colour col) const { return occ_bb[col]; }
    [[nodiscard]] Key get_key() const { return bs->key; }
    [[nodiscard]] Key get_pawn_key() const { return bs->pawn_key; }
    [[nodiscard]] Square get_en_passant() const { return bs->en_passant; }
    [[nodiscard]] int get_full_move_clock() const { return bs->fmc; }
    [[nodiscard]] int get_castle_rights() const { return bs->castle; }
    [[nodiscard]] int get_game_phase() const { return bs->game_phase; }

    // Utility Functions
    void print_position() const;
    [[nodiscard]] Move parse(std::string move) const;

  private:
    // State Modifiers
    void reset();
    void put_piece(Piece pc, Square sq);
    void replace_piece(Piece pc, Square sq);
    void remove_piece(Square sq);

    template <bool NM> void new_board_state();

    // State Queries
    [[nodiscard]] bool is_repeat() const;
    [[nodiscard]] bool is_draw_50() const;
    [[nodiscard]] bool is_material_draw() const;

    template <Colour US> [[nodiscard]] bool is_insufficient() const;

    // Utility Functions
    [[nodiscard]] Key make_key() const;
    [[nodiscard]] Key make_pawn_key() const;
    [[nodiscard]] Square get_smallest_attacker(Bitboard attackers, Colour stm) const;
    [[nodiscard]] Bitboard consider_xray(Bitboard occ, Square to, PieceType pt) const;
    [[nodiscard]] Bitboard attackers_to(Square sq) const;

    // Nested BoardState Struct
    struct BoardState
    {
        constexpr BoardState() = default;

        int castle{0}, hmc{0}, fmc{0}, ply_null{0}, game_phase{0};
        Piece captured_piece{NO_PIECE};
        Square en_passant{SQ_NONE};
        Key key{0ULL}, pawn_key{0ULL};
        std::unique_ptr<BoardState> prev;
    };

    // Member Variables
    Colour side{WHITE};
    std::array<Piece, SQ_N> pc_table{};
    std::array<Bitboard, 15> pc_bb{};
    std::array<Bitboard, 3> occ_bb{};
    std::unique_ptr<BoardState> bs;
};

// updates a bitboard of attackers after a piece has moved to include possible x ray attackers
inline Bitboard Position::consider_xray(const Bitboard occ, const Square to, const PieceType pt) const
{
    return (pt == PAWN || pt == BISHOP)
               ? occ & (bitboards::get_attacks<BISHOP>(occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]))
           : pt == ROOK  ? occ & (bitboards::get_attacks<ROOK>(occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK]))
           : pt == QUEEN ? consider_xray(occ, to, BISHOP) | consider_xray(occ, to, ROOK)
                         : 0ULL;
}

inline Bitboard Position::attackers_to(const Square sq) const
{
    return (bitboards::PAWN_ATTACKS[BLACK][sq] & pc_bb[W_PAWN]) | (bitboards::PAWN_ATTACKS[WHITE][sq] & pc_bb[B_PAWN]) |
           (bitboards::KNIGHT_ATTACKS[sq] & (pc_bb[W_KNIGHT] | pc_bb[B_KNIGHT])) | (bitboards::KING_ATTACKS[sq] & (pc_bb[W_KING] | pc_bb[B_KING])) |
           (bitboards::get_attacks<ROOK>(occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK])) |
           (bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]));
}

// returns whether a square is attacked by opposing side
template <Colour US> bool Position::is_attacked(const Square sq) const
{
    return (pc_bb[make_piece(PAWN, ~US)] & bitboards::PAWN_ATTACKS[US][sq]) || (pc_bb[make_piece(KNIGHT, ~US)] & bitboards::KNIGHT_ATTACKS[sq]) ||
           (pc_bb[make_piece(BISHOP, ~US)] & bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq)) ||
           (pc_bb[make_piece(ROOK, ~US)] & bitboards::get_attacks<ROOK>(occ_bb[BOTH], sq)) ||
           (pc_bb[make_piece(QUEEN, ~US)] & bitboards::get_attacks<QUEEN>(occ_bb[BOTH], sq)) ||
           (pc_bb[make_piece(KING, ~US)] & bitboards::KING_ATTACKS[sq]);
}

template <Colour US> bool Position::is_insufficient() const
{
    return std::popcount(pc_bb[make_piece(PAWN, US)]) == 0 && std::popcount(pc_bb[make_piece(ROOK, US)]) == 0 &&
           std::popcount(pc_bb[make_piece(QUEEN, US)]) == 0 && std::popcount(pc_bb[make_piece(KNIGHT, US)]) < 3 &&
           std::popcount(pc_bb[make_piece(BISHOP, US)]) + std::popcount(pc_bb[make_piece(KNIGHT, US)]) < 2;
}

inline bool Position::is_king_in_check() const
{
    return side == WHITE ? is_attacked<WHITE>(bitboards::lsb(pc_bb[W_KING])) : is_attacked<BLACK>(bitboards::lsb(pc_bb[B_KING]));
}

inline bool Position::is_stm_major() const
{
    return pc_bb[make_piece(KNIGHT, side)] | pc_bb[make_piece(BISHOP, side)] | pc_bb[make_piece(ROOK, side)] | pc_bb[make_piece(QUEEN, side)];
}

inline bool Position::is_draw_50() const { return bs->hmc >= 100; }

inline bool Position::is_draw() const { return is_repeat() || is_material_draw() || is_draw_50(); }

inline bool Position::is_material_draw() const { return is_insufficient<WHITE>() && is_insufficient<BLACK>(); }

} // namespace clovis