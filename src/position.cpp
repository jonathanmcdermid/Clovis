#include "position.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>

namespace clovis {

#if defined(__GNUC__)
constexpr std::array<std::string_view, 15> PIECE_SYMBOLS = {"·", "♙", "♘", "♗", "♖", "♕", "♔", "", "", "♟︎", "♞", "♝", "♜", "♛", "♚"};
#else
constexpr std::array<std::string_view, 15> PIECE_SYMBOLS = {".", "P", "N", "B", "R", "Q", "K", "", "", "p", "n", "b", "r", "q", "k"};
#endif

constexpr auto CASTLING_RIGHTS = [] {
    std::array<int, SQ_N> arr{};

    arr.fill(ALL_CASTLING);

    for (const Colour c : {WHITE, BLACK})
    {
        arr[relative_square(c, E1)] &= ~(ks_castle_rights(c) | qs_castle_rights(c));
        arr[relative_square(c, A1)] &= ~qs_castle_rights(c);
        arr[relative_square(c, H1)] &= ~ks_castle_rights(c);
    }

    return arr;
}();

namespace zobrist {

constexpr uint64_t xor_shift(uint64_t state)
{
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 0x2545F4914F6CDD1DULL;
}

constexpr uint64_t ZOBRIST_SEED = 0xB1FACE5ULL;

constexpr auto ZOBRIST_PSQT = [] {
    std::array<std::array<Key, SQ_N>, 15> arr{};
    uint64_t s = ZOBRIST_SEED;

    for (int i = NO_PIECE; i <= B_KING; ++i)
    {
        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) { arr[i][sq] = xor_shift(s++); }
    }

    return arr;
}();

constexpr auto ZOBRIST_EN_PASSANT = [] {
    std::array<Key, SQ_N> arr{};
    uint64_t s = ZOBRIST_SEED + 15ULL * SQ_N;

    for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) { arr[sq] = xor_shift(s++); }

    return arr;
}();

constexpr auto ZOBRIST_CASTLING = [] {
    std::array<Key, 16> arr{};
    uint64_t s = ZOBRIST_SEED + 16ULL * SQ_N;

    for (int i = 0; i < 16; ++i) { arr[i] = xor_shift(s++); }

    return arr;
}();

constexpr Key ZOBRIST_COLOUR = xor_shift(ZOBRIST_SEED + 16ULL * SQ_N + 16);

} // namespace zobrist

// returns the square that pins a piece if it exists
template <Colour US> Square Position::get_pinner(const Square sq) const
{
    if (const Square ksq = bitboards::lsb(pc_bb[make_piece(KING, US)]); bitboards::get_attacks<QUEEN>(ksq) & sq)
    {
        Bitboard candidates =
            ((bitboards::get_attacks<ROOK>(occ_bb[BOTH] ^ sq, ksq) & (pc_bb[make_piece(QUEEN, ~US)] | pc_bb[make_piece(ROOK, ~US)])) |
             (bitboards::get_attacks<BISHOP>(occ_bb[BOTH] ^ sq, ksq) & (pc_bb[make_piece(QUEEN, ~US)] | pc_bb[make_piece(BISHOP, ~US)])));

        while (candidates)
        {
            if (const Square candidate = bitboards::pop_lsb(candidates); bitboards::between_squares(ksq, candidate) & sq) { return candidate; }
        }
    }

    return SQ_NONE;
}

// explicit template instantiations
template Square Position::get_pinner<WHITE>(Square sq) const;
template Square Position::get_pinner<BLACK>(Square sq) const;

// returns if a square is in danger of a discovery attack by a rook or bishop
template <Colour US> bool Position::is_discovery_threat(const Square sq) const
{
    // pawn is immobile if it attacks no enemies and is blocked by a piece
    // we don't have to worry about shift because discovery pawns will never be on outer files
    Bitboard their_immobile_pawns = (bitboards::shift<pawn_push(US)>(occ_bb[BOTH]) & pc_bb[make_piece(PAWN, ~US)]) &
                                    ~(bitboards::shift<pawn_push(US) + EAST>(occ_bb[US]) | bitboards::shift<pawn_push(US) + WEST>(occ_bb[US]));

    if (side == ~US && bs->en_passant != SQ_NONE) { their_immobile_pawns &= ~bitboards::PAWN_ATTACKS[US][bs->en_passant]; }

    Bitboard candidates =
        ((bitboards::get_attacks<ROOK>(pc_bb[W_PAWN] | pc_bb[B_PAWN], sq) & (pc_bb[make_piece(ROOK, ~US)])) |
         (bitboards::get_attacks<BISHOP>(pc_bb[make_piece(PAWN, US)] | their_immobile_pawns, sq) & (pc_bb[make_piece(BISHOP, ~US)])));

    const Bitboard occupancy = occ_bb[BOTH] ^ candidates;

    while (candidates)
    {
        if (std::popcount(bitboards::between_squares(sq, bitboards::pop_lsb(candidates)) & occupancy) == 1) { return true; }
    }

    return false;
}

// explicit template instantiations
template bool Position::is_discovery_threat<WHITE>(Square sq) const;
template bool Position::is_discovery_threat<BLACK>(Square sq) const;

std::string Position::get_fen() const
{
    std::stringstream fen;

    for (Rank r = RANK_8; r >= RANK_1; --r)
    {
        int empty_count = 0;

        for (File f = FILE_A; f <= FILE_H; ++f)
        {
            if (const Square sq = make_square(f, r); pc_table[sq] == NO_PIECE) { ++empty_count; }
            else
            {
                if (empty_count)
                {
                    fen << empty_count;
                    empty_count = 0;
                }
                fen.put(PIECE_STR[pc_table[sq]]);
            }
        }
        if (empty_count) { fen << empty_count; }
        if (r != RANK_1) { fen.put('/'); }
    }

    fen << ((side == WHITE) ? " w " : " b ");

    if (bs->castle == NO_CASTLING) { fen.put('-'); }
    else
    {
        if (bs->castle & WHITE_KS) { fen.put('K'); }
        if (bs->castle & WHITE_QS) { fen.put('Q'); }
        if (bs->castle & BLACK_KS) { fen.put('k'); }
        if (bs->castle & BLACK_QS) { fen.put('q'); }
    }

    fen << " " + (bs->en_passant == SQ_NONE ? "-" : sq2str(bs->en_passant)) << " " << std::to_string(bs->hmc) << " " << std::to_string(bs->fmc);

    return fen.str();
}

// sets position to the state specified by FEN std::string
void Position::set(const char* fen)
{
    reset();
    bs = std::make_unique<BoardState>();

    std::istringstream ss(fen);
    char token = '\0';
    Square sq = A8;

    ss >> std::noskipws;

    while ((ss >> token) && !isspace(token))
    {
        if (isdigit(token)) { sq += (token - '0') * EAST; }
        else if (token == '/') { sq += 2 * SOUTH; }
        else if (size_t index = PIECE_STR.find(token); index != std::string::npos)
        {
            put_piece(static_cast<Piece>(index), sq);
            put_piece(static_cast<Piece>(index), sq);
            bs->game_phase += GAME_PHASE_INCREMENT[static_cast<Piece>(index)];
            sq += EAST;
        }
    }

    ss >> token;
    side = (token == 'w' ? WHITE : BLACK);
    ss >> token;

    while ((ss >> token) && !isspace(token))
    {
        switch (token)
        {
        case 'K': bs->castle |= WHITE_KS; break;
        case 'Q': bs->castle |= WHITE_QS; break;
        case 'k': bs->castle |= BLACK_KS; break;
        case 'q': bs->castle |= BLACK_QS; break;
        default: break;
        }
    }

    ss >> token;

    if (token != '-')
    {
        const auto f = static_cast<File>(token - 'a');
        ss >> token;
        const auto r = static_cast<Rank>(token - '1');
        bs->en_passant = make_square(f, r);
    }
    else { bs->en_passant = SQ_NONE; }

    ss >> std::skipws >> bs->hmc >> bs->fmc;
    bs->key = make_key();
    bs->pawn_key = make_pawn_key();
    bs->ply_null = 0;
}

void Position::reset()
{
    side = WHITE;
    bs.reset();
    pc_table.fill(NO_PIECE);
    pc_bb.fill(0ULL);
    occ_bb.fill(0ULL);
}

Key Position::make_key() const
{
    Key k = 0ULL;

    for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
    {
        if (pc_table[sq] != NO_PIECE) { k ^= zobrist::ZOBRIST_PSQT[pc_table[sq]][sq]; }
    }

    if (bs->en_passant != SQ_NONE) { k ^= zobrist::ZOBRIST_EN_PASSANT[bs->en_passant]; }

    if (side == BLACK) { k ^= zobrist::ZOBRIST_COLOUR; }

    k ^= zobrist::ZOBRIST_CASTLING[bs->castle];

    return k;
}

Key Position::make_pawn_key() const
{
    Key k = 0ULL;

    for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
    {
        if (piece_type(pc_table[sq]) == PAWN || piece_type(pc_table[sq]) == KING) { k ^= zobrist::ZOBRIST_PSQT[pc_table[sq]][sq]; }
    }

    return k;
}

int Position::see(const Move move) const
{
    // this function will not work for quiet moves that arent promotions because of how we update attackers bitboard
    assert(move_capture(move) || move_promotion_type(move));
    std::array<int, 32> gain{};
    int d = 0;
    Square from = move_from_sq(move);
    const Square to = move_to_sq(move);
    Bitboard occ = occ_bb[BOTH];
    Bitboard attackers = attackers_to(to);
    Colour stm = side;

    if (move_promotion_type(move))
    {
        gain[0] = PIECE_VALUE[pc_table[to]] + PIECE_VALUE[move_promotion_type(move)] - PIECE_VALUE[PAWN];
        stm = ~stm;
        d = 1;
        gain[1] = PIECE_VALUE[move_promotion_type(move)] - gain[0];
        attackers &= ~from; // no ^= as this may be a quiet move
        occ ^= from;
        attackers |= consider_xray(occ, to, move_capture(move) ? BISHOP : ROOK);
        from = get_smallest_attacker(attackers, stm);
    }
    else if (move_en_passant(move))
    {
        gain[0] = PIECE_VALUE[PAWN];
        stm = ~stm;
        d = 1;
        gain[1] = 0;
        attackers ^= from;
        occ ^= from;
        occ ^= to - pawn_push(side);
        attackers |= consider_xray(occ, to, QUEEN);
        from = get_smallest_attacker(attackers, stm);
    }
    else { gain[0] = PIECE_VALUE[pc_table[to]]; }

    while (from != SQ_NONE)
    {
        stm = ~stm;
        d++;
        assert(d < 32);
        gain[d] = PIECE_VALUE[pc_table[from]] - gain[d - 1];
        attackers ^= from;
        occ ^= from;
        attackers |= consider_xray(occ, to, piece_type(pc_table[from]));
        from = get_smallest_attacker(attackers, stm);
    }

    while (--d) { gain[d - 1] = -std::max(-gain[d - 1], gain[d]); }

    return gain[0];
}

// updates bitboards to represent a new piece on a square
// does not remove info if a piece was already on that square
void Position::put_piece(const Piece pc, const Square sq)
{
    assert(!(sq & pc_bb[pc]));
    assert(!(sq & occ_bb[piece_colour(pc)]));
    assert(!(sq & occ_bb[BOTH]));

    pc_bb[pc] |= sq;
    occ_bb[piece_colour(pc)] |= sq;
    occ_bb[BOTH] |= sq;
    pc_table[sq] = pc;
}

// updates bitboards to replace an old piece with a new piece on a square
void Position::replace_piece(const Piece pc, const Square sq)
{
    assert(!(sq & occ_bb[piece_colour(pc)]));
    assert((sq & occ_bb[~piece_colour(pc)]));
    assert((sq & occ_bb[BOTH]));

    pc_bb[pc_table[sq]] ^= sq;
    pc_bb[pc] |= sq;
    occ_bb[~piece_colour(pc)] ^= sq;
    occ_bb[piece_colour(pc)] |= sq;
    pc_table[sq] = pc;
}

// updates bitboards to represent a piece being removed from a square
void Position::remove_piece(const Square sq)
{
    const Piece pc = pc_table[sq];

    assert(sq & pc_bb[pc]);
    assert(sq & occ_bb[piece_colour(pc)]);
    assert(sq & occ_bb[BOTH]);

    pc_bb[pc] ^= sq;
    occ_bb[piece_colour(pc)] ^= sq;
    occ_bb[BOTH] ^= sq;
    pc_table[sq] = NO_PIECE;
}

template <bool NM> void Position::new_board_state()
{
    auto bs_new = std::make_unique<BoardState>();
    bs_new->en_passant = bs->en_passant;
    bs_new->castle = bs->castle;
    bs_new->hmc = NM ? bs->hmc : bs->hmc + 1; // TODO: does a null move increase HMC?
    bs_new->fmc = bs->fmc + (side == BLACK);
    bs_new->ply_null = NM ? 0 : bs->ply_null + 1;
    bs_new->key = bs->key ^ zobrist::ZOBRIST_COLOUR;
    bs_new->pawn_key = bs->pawn_key;
    bs_new->game_phase = bs->game_phase;
    bs_new->prev = std::move(bs);
    // position now refers to new board state
    bs = std::move(bs_new);

    if (bs->en_passant != SQ_NONE)
    {
        bs->key ^= zobrist::ZOBRIST_EN_PASSANT[bs->en_passant];
        bs->en_passant = SQ_NONE;
    }
}

// executes a null move and updates the position
void Position::do_null_move()
{
    new_board_state<true>();
    side = ~side;
}

// reverts a null move and rolls back the position
void Position::undo_null_move()
{
    side = ~side;
    assert(bs->prev);
    std::unique_ptr<BoardState> temp = std::move(bs);
    bs = std::move(temp->prev);
}

// executes a move and updates the position
bool Position::do_move(const Move move)
{
    new_board_state<false>();

    const Square src = move_from_sq(move);
    const Square tar = move_to_sq(move);
    const Piece piece = move_piece_type(move);

    assert(piece_colour(move_piece_type(move)) == side);
    assert(piece_colour(pc_table[src]) == side);
    assert(pc_table[tar] == NO_PIECE || piece_colour(pc_table[tar]) != side);
    assert(piece_type(pc_table[tar]) != KING);

    // move piece
    bs->captured_piece = pc_table[tar];

    // update castling rights
    bs->key ^= zobrist::ZOBRIST_CASTLING[bs->castle];
    bs->castle &= CASTLING_RIGHTS[src];
    bs->castle &= CASTLING_RIGHTS[tar];
    bs->key ^= zobrist::ZOBRIST_CASTLING[bs->castle];

    if (move_capture(move))
    {
        if (move_en_passant(move))
        {
            const Square victim_sq = tar - pawn_push(side);
            bs->captured_piece = make_piece(PAWN, ~side);
            bs->key ^= zobrist::ZOBRIST_PSQT[bs->captured_piece][victim_sq];
            bs->pawn_key ^= zobrist::ZOBRIST_PSQT[bs->captured_piece][victim_sq];
            remove_piece(victim_sq);
            put_piece(piece, tar);
        }
        else
        {
            if (piece_type(bs->captured_piece) == PAWN) { bs->pawn_key ^= zobrist::ZOBRIST_PSQT[bs->captured_piece][tar]; }

            bs->key ^= zobrist::ZOBRIST_PSQT[pc_table[tar]][tar];
            replace_piece(piece, tar);
        }
        bs->game_phase -= GAME_PHASE_INCREMENT[bs->captured_piece];
        bs->hmc = 0;
    }
    else { put_piece(piece, tar); }

    bs->key ^= zobrist::ZOBRIST_PSQT[pc_table[src]][src];
    bs->key ^= zobrist::ZOBRIST_PSQT[pc_table[src]][tar];
    remove_piece(src);

    if (piece_type(piece) == PAWN)
    {
        if (move_double(move))
        {
            bs->en_passant = tar - pawn_push(side);
            bs->key ^= zobrist::ZOBRIST_EN_PASSANT[bs->en_passant];
        }
        else if (move_promotion_type(move))
        {
            bs->key ^= zobrist::ZOBRIST_PSQT[pc_table[tar]][tar];
            bs->pawn_key ^= zobrist::ZOBRIST_PSQT[piece][tar];
            remove_piece(tar);
            put_piece(move_promotion_type(move), tar);
            bs->key ^= zobrist::ZOBRIST_PSQT[pc_table[tar]][tar];
            bs->game_phase -= GAME_PHASE_INCREMENT[PAWN];
            bs->game_phase += GAME_PHASE_INCREMENT[move_promotion_type(move)];
        }
        bs->pawn_key ^= zobrist::ZOBRIST_PSQT[piece][src];
        bs->pawn_key ^= zobrist::ZOBRIST_PSQT[piece][tar];
        bs->hmc = 0;
    }
    else if (piece_type(piece) == KING)
    {
        bs->pawn_key ^= zobrist::ZOBRIST_PSQT[piece][src];
        bs->pawn_key ^= zobrist::ZOBRIST_PSQT[piece][tar];

        if (move_castling(move))
        {
            Square rt = SQ_ZERO;
            Square rf = SQ_ZERO;
            get_castle_rook_squares(tar, rf, rt);
            bs->key ^= zobrist::ZOBRIST_PSQT[pc_table[rf]][rf];
            bs->key ^= zobrist::ZOBRIST_PSQT[pc_table[rf]][rt];
            remove_piece(rf);
            put_piece(make_piece(ROOK, side), rt);
        }
    }

    // move gen doesn't check for suicidal king, so we check here
    const bool valid = !is_king_in_check();
    side = ~side;
    if (!valid) { undo_move(move); }
    return valid;
}

// reverts a move and rolls back the position
void Position::undo_move(const Move move)
{
    side = ~side;
    const Square tar = move_to_sq(move);
    put_piece(move_piece_type(move), move_from_sq(move));

    if (move_capture(move))
    {
        if (move_en_passant(move))
        {
            remove_piece(tar);
            put_piece(bs->captured_piece, tar - pawn_push(side));
        }
        else { replace_piece(bs->captured_piece, tar); }
    }
    else
    {
        if (move_castling(move))
        {
            Square rt = SQ_ZERO;
            Square rf = SQ_ZERO;
            get_castle_rook_squares(tar, rf, rt);
            remove_piece(rt);
            put_piece(make_piece(ROOK, side), rf);
        }
        remove_piece(tar);
    }

    assert(bs->prev);
    std::unique_ptr<BoardState> temp = std::move(bs);
    bs = std::move(temp->prev);
}

// returns the piece type of the least valuable piece on a bitboard of attackers
Square Position::get_smallest_attacker(const Bitboard attackers, const Colour stm) const
{
    if (attackers & occ_bb[stm])
    {
        for (PieceType pt = PAWN; pt <= KING; ++pt)
        {
            if (const Bitboard bb = pc_bb[make_piece(pt, stm)] & attackers) { return bitboards::lsb(bb); }
        }
    }

    return SQ_NONE;
}

bool Position::is_repeat() const
{
    const BoardState* temp = bs.get();

    for (int end = std::min(bs->hmc, bs->ply_null); end >= 4; end -= 4)
    {
        assert(temp->prev && temp->prev->prev && temp->prev->prev->prev && temp->prev->prev->prev->prev);
        temp = temp->prev->prev->prev->prev.get();

        if (temp->key == bs->key) { return true; }
    }

    return false;
}

// prints the current position
void Position::print_position() const
{
    for (Rank r = RANK_8; r >= RANK_1; --r)
    {
        std::cout << r + 1 << ' ';
        for (File f = FILE_A; f < FILE_N; ++f) { std::cout << PIECE_SYMBOLS[pc_table[make_square(f, r)]] << " "; }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h" << '\n' << '\n' << get_fen() << '\n' << '\n';
}

Move Position::parse(std::string move) const
{
    if (move[move.length() - 1] == '+' || move[move.length() - 1] == '#') { move = move.substr(0, move.length() - 1); }

    if (move.find("O-O") != std::string::npos)
    {
        return encode_move(relative_square(side, E1), relative_square(side, move == "O-O" ? G1 : C1), make_piece(KING, side), NO_PIECE, false, false,
                           false, true);
    }
    if (islower(move[0]))
    { // pawn moves
        const Piece promo =
            move[move.length() - 2] == '=' ? make_piece(static_cast<PieceType>(PIECE_STR.find(move[move.length() - 1])), side) : NO_PIECE;
        const Square to = (promo == NO_PIECE) ? str2sq(move.substr(move.length() - 2)) : str2sq(move.substr(move.length() - 4, 2));
        const Square from = (move[1] == 'x') ? make_square(static_cast<File>(move[0] - 'a'), rank_of(to - pawn_push(side)))
                            : pc_table[to - pawn_push(side)] == NO_PIECE ? to - 2 * pawn_push(side)
                                                                         : to - pawn_push(side);

        return encode_move(from, to, make_piece(PAWN, side), promo, move.find('x') != std::string::npos, abs(rank_of(to) - rank_of(from)) == 2,
                           bs->en_passant == to, false);
    }
    // major moves
    const Piece piece = make_piece(static_cast<PieceType>(PIECE_STR.find(move[0])), side);
    const Square to = str2sq(move.substr(move.length() - 2));
    Bitboard bb = bitboards::get_attacks(piece_type(piece), occ_bb[BOTH], to) & pc_bb[piece];
    Square from = bitboards::pop_lsb(bb);

    if (move[1] == 'x' || move.length() == 3)
    {
        // one of the pieces that attacks this square is pinned
        if (bb)
        {
            if (side == WHITE)
            {
                while (get_pinner<WHITE>(from) != SQ_NONE) { from = bitboards::pop_lsb(bb); }
            }
            else
            {
                while (get_pinner<BLACK>(from) != SQ_NONE) { from = bitboards::pop_lsb(bb); }
            }
        }
    }
    else if (move[2] == 'x' || move.length() == 4)
    {
        // there are multiple matching pieces that attack this square
        if (isdigit(move[1]))
        {
            while (rank_of(from) != static_cast<Rank>(move[1] - '1')) { from = bitboards::pop_lsb(bb); }
        }
        else
        {
            while (file_of(from) != static_cast<File>(move[1] - 'a')) { from = bitboards::pop_lsb(bb); }
        }
    }

    return encode_move(from, to, piece, NO_PIECE, move.find('x') != std::string::npos, false, false, false);
}

} // namespace clovis
