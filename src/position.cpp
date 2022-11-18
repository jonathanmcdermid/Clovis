#include "position.h"

namespace Clovis {

    const extern std::string piece_str = " PNBRQK  pnbrqk";

    constexpr int game_phase_inc[15] = { 0, 0, 1, 1, 2, 4, 0, 0, 0, 0, 1, 1, 2, 4, 0 };

    constexpr int position_size = sizeof(Position);

    // castling rights lookup table
    const int castling_rights[SQ_N] = {
        13, 15, 15, 15, 12, 15, 15, 14,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
         7, 15, 15, 15,  3, 15, 15, 11
    };

    namespace Zobrist {

        Key piece_square[15][SQ_N];
        Key enpassant[SQ_N];
        Key castling[16];
        Key side;

    } // namespace Zobrist

    void Position::init_position()
    {
        for (int i = NO_PIECE; i <= B_KING; ++i)
            for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
                Zobrist::piece_square[i][sq] = Random::random_U64();
        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
            Zobrist::enpassant[sq] = Random::random_U64();
        for (int i = 0; i < 16; ++i)
            Zobrist::castling[i] = Random::random_U64();
        Zobrist::side = Random::random_U64();
    }

    // sets position to the state specified by FEN string
	void Position::set(const char* fen)
	{
        memset(this, 0, position_size);
        bs = new BoardState;

        std::istringstream ss(fen);
        unsigned char token;

        int index;
        Square sq = A8;

        ss >> std::noskipws;

        while ((ss >> token) && !isspace(token))
        {
            if (isdigit(token))
                sq = sq + (token - '0') * EAST;
            else if (token == '/')
                sq = sq + 2 * SOUTH;
            else if ((index = piece_str.find(token)) != -1) {
                put_piece(Piece(index), sq);
                bs->game_phase += game_phase_inc[Piece(index)];
                ++sq;
            }
        }
        ss >> token;
        side = (token == 'w' ? WHITE : BLACK);
        ss >> token;
        while ((ss >> token) && !isspace(token)) 
        {
            if (token == 'K') bs->castle |= WHITE_KS;
            else if (token == 'Q') bs->castle |= WHITE_QS;
            else if (token == 'k') bs->castle |= BLACK_KS;
            else if (token == 'q') bs->castle |= BLACK_QS;
            else continue;
        }
        ss >> token;
        if (token != '-') 
        {
            File f = File(token - 'a');
            ss >> token;
            Rank r = Rank(token - '1');
            bs->enpassant = make_square(f, r);
        }
        else bs->enpassant = SQ_NONE;
        ss >> std::skipws >> bs->hmc >> bs->fmc;
        bs->key = make_key();
        bs->pkey = make_pawn_key();
        bs->ply_null = 0;
	}

    Key Position::make_key()
    {
        Key k = 0ULL;

        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) 
        {
            Piece p = piece_board[sq];
            if(p != NO_PIECE)
                k ^= Zobrist::piece_square[p][sq];
        }

        if (bs->enpassant != SQ_NONE)
            k ^= Zobrist::enpassant[bs->enpassant];

        if (side == BLACK)
            k ^= Zobrist::side;

        k ^= Zobrist::castling[bs->castle];

        return k;
    }

    Key Position::make_pawn_key()
    {
        Key k = 0ULL;

        for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
        {
            Piece p = piece_board[sq];
            if (piece_type(p) == PAWN)
                k ^= Zobrist::piece_square[p][sq];
        }

        return k;
    }

    // returns whether or not a square is attacked by a particular side
    bool Position::is_attacked (Square sq, Colour s) const
    {
        return ((piece_bitboard[make_piece(PAWN, s)] & Bitboards::pawn_attacks[!s][sq]) 
            || (piece_bitboard[make_piece(KNIGHT, s)] & Bitboards::knight_attacks[sq]) 
            || (piece_bitboard[make_piece(BISHOP, s)] & Bitboards::get_bishop_attacks(occ_bitboard[BOTH], sq)) 
            || (piece_bitboard[make_piece(ROOK, s)] & Bitboards::get_rook_attacks(occ_bitboard[BOTH], sq)) 
            || (piece_bitboard[make_piece(QUEEN, s)] & Bitboards::get_queen_attacks(occ_bitboard[BOTH], sq)) 
            || (piece_bitboard[make_piece(KING, s)] & Bitboards::king_attacks[sq]));
    }

    Bitboard Position::attackers_to(Square sq, Bitboard occupied) const 
    {
        return  (Bitboards::pawn_attacks[WHITE][sq] & piece_bitboard[W_PAWN]) 
            | (Bitboards::pawn_attacks[BLACK][sq] & piece_bitboard[B_PAWN]) 
            | (Bitboards::knight_attacks[sq] & (piece_bitboard[W_KNIGHT] | piece_bitboard[B_KNIGHT])) 
            | (Bitboards::get_rook_attacks(occupied, sq) 
                & (piece_bitboard[W_ROOK] | piece_bitboard[B_ROOK] | piece_bitboard[W_QUEEN] | piece_bitboard[B_QUEEN])) 
            | (Bitboards::get_bishop_attacks(occupied, sq) 
                & (piece_bitboard[W_BISHOP] | piece_bitboard[B_BISHOP] | piece_bitboard[W_QUEEN] | piece_bitboard[B_QUEEN])) 
            | (Bitboards::king_attacks[sq] & (piece_bitboard[W_KING] | piece_bitboard[B_KING]));
    }

    bool Position::see(Move move) const
    {
        return true;
    }

    // updates bitboards to represent a new piece on a square
    // does not remove info if a piece was already on that square
	void Position::put_piece(Piece pc, Square sq) 
	{
        assert(!(piece_bitboard[pc] & sq));
        assert(!(occ_bitboard[get_side(pc)] & sq));
        assert(!(occ_bitboard[BOTH] & sq));

        piece_bitboard[pc] |= sq;
        occ_bitboard[get_side(pc)] |= sq;
        occ_bitboard[BOTH] |= sq;
        piece_board[sq] = pc;
	}

    // updates bitboards to represent a piece being removed from a square
    void Position::remove_piece(Square sq)
    {
        Piece pc = piece_board[sq];

        assert(piece_bitboard[pc] & sq);
        assert(occ_bitboard[get_side(pc)] & sq);
        assert(occ_bitboard[BOTH] & sq);

        piece_bitboard[pc] ^= sq;
        occ_bitboard[get_side(pc)] ^= sq;
        occ_bitboard[BOTH] ^= sq;
        piece_board[sq] = NO_PIECE;
    }

    bool Position::move_is_ok(Move move) const
    {
        return get_side(move_piece_type(move)) == side
            && get_side(piece_board[move_from_sq(move)]) == side
            && (piece_board[move_to_sq(move)] == NO_PIECE || get_side(piece_board[move_to_sq(move)]) != side)
            && piece_type(piece_board[move_to_sq(move)]) != KING;
    }

    // executes a move and updates the position
    bool Position::do_move(Move move)
    {
        BoardState* bs_new = new BoardState;
        assert(bs_new != bs);
        // copy old boardstate info to new boardstate and update clocks
        bs_new->enpassant = bs->enpassant;
        bs_new->castle = bs->castle;
        bs_new->hmc = bs->hmc + 1;
        bs_new->fmc = bs->fmc + (side == BLACK);
        bs_new->ply_null = bs->ply_null + 1;
        bs_new->key = bs->key;
        bs_new->pkey = bs->pkey;
        bs_new->game_phase = bs->game_phase;
        bs_new->prev = bs;
        // position now refers to new boardstate
        bs = bs_new;

        Square src = move_from_sq(move);
        Square tar = move_to_sq(move);
        Piece piece = move_piece_type(move);

        // null moves
        if (move == MOVE_NULL)
        {
            if (bs->enpassant != SQ_NONE)
            {
                bs->key ^= Zobrist::enpassant[bs->enpassant];
                bs->enpassant = SQ_NONE;
            }
            bs->captured_piece = NO_PIECE;
            bs->ply_null = 0;
            goto nullmove;
        }

        assert(get_side(move_piece_type(move)) == side);
        assert(get_side(piece_board[src]) == side);
        assert(piece_board[tar] == NO_PIECE || get_side(piece_board[tar]) != side);
        assert(piece_type(piece_board[tar]) != KING);

        // move piece
        bs->captured_piece = piece_board[tar];

        if (move_castling(move))
        {
            Square rt = castle_rook_to(tar);
            Square rf = castle_rook_from(tar);
            bs->key ^= Zobrist::piece_square[piece_board[rf]][rf];
            bs->key ^= Zobrist::piece_square[piece_board[rf]][rt];
            remove_piece(rf);
            put_piece(make_piece(ROOK, side), rt);
        }

        // update castling rights
        bs->key ^= Zobrist::castling[bs->castle];
        bs->castle &= castling_rights[src];
        bs->castle &= castling_rights[tar];
        bs->key ^= Zobrist::castling[bs->castle];

        if (move_capture(move))
        {
            if (move_enpassant(move))
            {
                Square victim_sq = tar - pawn_push(side);
                bs->captured_piece = make_piece(PAWN, other_side(side));
                bs->key ^= Zobrist::piece_square[bs->captured_piece][victim_sq];
                bs->pkey ^= Zobrist::piece_square[bs->captured_piece][victim_sq];
                remove_piece(victim_sq);
            }
            else
            {
                if (piece_type(bs->captured_piece) == PAWN)
                    bs->pkey ^= Zobrist::piece_square[bs->captured_piece][tar];
                bs->key ^= Zobrist::piece_square[piece_board[tar]][tar];
                remove_piece(tar);
            }
            bs->game_phase -= game_phase_inc[bs->captured_piece];
            bs->hmc = 0;
        }

        put_piece(piece, tar);
        bs->key ^= Zobrist::piece_square[piece_board[src]][src];
        bs->key ^= Zobrist::piece_square[piece_board[src]][tar];
        remove_piece(src);

        if (bs->enpassant != SQ_NONE)
        {
            bs->key ^= Zobrist::enpassant[bs->enpassant];
            bs->enpassant = SQ_NONE;
        }

        if (piece_type(piece) == PAWN)
        {
            if (move_double(move))
            {
                bs->enpassant = tar - pawn_push(side);
                bs->key ^= Zobrist::enpassant[bs->enpassant];
            }
            else if (move_promotion_type(move))
            {
                bs->key ^= Zobrist::piece_square[piece_board[tar]][tar];
                bs->pkey ^= Zobrist::piece_square[piece][tar];
                remove_piece(tar);
                put_piece(move_promotion_type(move), tar);
                bs->key ^= Zobrist::piece_square[piece_board[tar]][tar];
                bs->game_phase -= game_phase_inc[W_PAWN];
                bs->game_phase += game_phase_inc[move_promotion_type(move)];
            }
            bs->pkey ^= Zobrist::piece_square[piece][src];
            bs->pkey ^= Zobrist::piece_square[piece][tar];
            bs->hmc = 0;
        }

    nullmove:

        side = other_side(side);
        bs->key ^= Zobrist::side;

        //Key checkhash = make_pawn_key();
        //
        //if (checkhash != bs->pkey)
        //{
        //    std::cout << UCI::move2str(m) << "\n";
        //    print_position();
        //    undo_move(m);
        //    print_position();
        //    U64 sss = bs->pkey ^ checkhash;
        //    side = side;
        //}
        //
        //checkhash = make_key();
        //
        //if (checkhash != bs->key)
        //{
        //    std::cout << UCI::move2str(m) << "\n";
        //    print_position();
        //    undo_move(m);
        //    print_position();
        //    U64 sss = bs->key ^ checkhash;
        //    side = side;
        //}

        // move gen doesnt check for suicidal king, so we check here
        if (is_king_in_check(other_side(side)))
        {
            undo_move(move);
            return false;
        }
        return true;
    }

    // reverts a move and rolls back the position
    void Position::undo_move(Move move)
    {
        side = other_side(side);

        Square src = move_from_sq(move);
        Square tar = move_to_sq(move);

        // null moves
        if (move == MOVE_NULL)
            goto nullmove;

        put_piece(move_piece_type(move), src);
        remove_piece(tar);

        if (move_castling(move))
        {
            Square rt = castle_rook_to(tar);
            Square rf = castle_rook_from(tar);
            remove_piece(rt);
            put_piece(make_piece(ROOK, side), rf);
        }

        if (move_capture(move))
        {
            if (move_enpassant(move))
                put_piece(bs->captured_piece, tar - pawn_push(side));
            else
                put_piece(bs->captured_piece, tar);
        }

    nullmove:

        assert(bs->prev);
        BoardState* temp = bs;
        bs = bs->prev;
        delete temp;
    }

    bool Position::is_repeat() const {

        BoardState* temp = bs;
       
        for (int end = std::min(bs->hmc, bs->ply_null); end >= 4; end -= 4)
        {
            assert(temp->prev->prev->prev->prev);

            temp = temp->prev->prev->prev->prev;

            if (temp->key == bs->key)
                return true;
        }
        return false;
    }

    // prints the current position
    void Position::print_position() const
    {
        std::cout << "+---+---+---+---+---+---+---+---+\n";
        for (Rank r = RANK_8; r >= 0; --r)
        {
            for (File f = FILE_A; f < FILE_N; ++f)
            {
                Square sq = make_square(f, r);

                int bb_piece;
                for (bb_piece = W_PAWN; bb_piece <= B_KING; ++bb_piece)
                    if (piece_bitboard[bb_piece] & sq)
                        break;

                std::cout << "| " << ((bb_piece > B_KING) ? ' ' : piece_str[bb_piece]) << " ";
            }
            std::cout << "|" + std::to_string(1 + r) + "\n" + "+---+---+---+---+---+---+---+---+\n";
        }
        std::cout << "  a   b   c   d   e   f   g   h\n"
            << "Side:\t\t" 
            << (side == WHITE ? "white" : "black") << "\n"
            << "Enpassant:\t" 
            << ((bs->enpassant != SQ_NONE) ? sq2str(bs->enpassant) : "none") << "\n"
            << "Castling:\t"
            << ((bs->castle & WHITE_KS) ? 'K' : '-')
            << ((bs->castle & WHITE_QS) ? 'Q' : '-')
            << ((bs->castle & BLACK_KS) ? 'k' : '-')
            << ((bs->castle & BLACK_QS) ? 'q' : '-') << '\n'
            << "Halfmove:\t" << bs->hmc << '\n'
            << "Fullmove:\t" << bs->fmc << '\n';
    }

    // prints the bitboards for this position
    void Position::print_bitboards() 
    {
        // there is a gap in value between W_KING AND B_PAWN
        // therefore some of the bitboards printed are empty
        std::cout << "Printing Bitboards\n";
        for (auto it : piece_bitboard)
            Bitboards::print_bitboard(it);
        for (auto it : occ_bitboard)
            Bitboards::print_bitboard(it);
    }

    // prints the attacked squares for a given side
    void Position::print_attacked_squares(Colour side)
    {
        std::cout << "+---+---+---+---+---+---+---+---+\n";
        for (Rank r = RANK_8; r >= RANK_1; --r)
        {
            for (File f = FILE_A; f <= FILE_H; ++f)
            {
                std::cout << (is_attacked(make_square(f, r), side) ? "| X " : "|   ");
            }
            std::cout << "|" + std::to_string(1 + r) + "\n" + "+---+---+---+---+---+---+---+---+\n";
        }
    }

} // namespace Clovis