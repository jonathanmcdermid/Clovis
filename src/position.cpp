#include <memory>
#include <sstream>
#include <iostream>

#include "position.h"

namespace Clovis {

    const extern std::string piece_str = " PNBRQK  pnbrqk";

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
            else if ((index = piece_str.find(token)) != piece_str.size()) {
                put_piece(Piece(index), sq);
                ++sq;
            }
        }
        ss >> token;
        side = (token == 'w' ? WHITE : BLACK);
        ss >> token;
        while ((ss >> token) && !isspace(token)) {
            if (token == 'K') bs->castle |= WHITE_KS;
            else if (token == 'Q') bs->castle |= WHITE_QS;
            else if (token == 'k') bs->castle |= BLACK_KS;
            else if (token == 'q') bs->castle |= BLACK_QS;
            else continue;
        }
        ss >> token;
        if (token != '-') {
            File f = File(token - 'a');
            ss >> token;
            Rank r = Rank(token - '1');
            bs->enpassant = make_square(f, r);
        }
        else bs->enpassant = SQ_NONE;
        ss >> std::skipws >> bs->hmc >> bs->fmc;
	}

    // returns whether or not a square is attacked by a particular side
    bool Position::is_attacked (Square sq, Colour s) const
    {
        return ((piece_bitboard[make_piece(PAWN, s)] & Bitboards::pawn_attacks[!s][sq]) ||
            (piece_bitboard[make_piece(KNIGHT, s)] & Bitboards::knight_attacks[sq]) ||
            (piece_bitboard[make_piece(BISHOP, s)] & Bitboards::get_bishop_attacks(occ_bitboard[BOTH], sq)) ||
            (piece_bitboard[make_piece(ROOK, s)] & Bitboards::get_rook_attacks(occ_bitboard[BOTH], sq)) ||
            (piece_bitboard[make_piece(QUEEN, s)] & Bitboards::get_queen_attacks(occ_bitboard[BOTH], sq)) ||
            (piece_bitboard[make_piece(KING, s)] & Bitboards::king_attacks[sq]));
    }

    // updates bitboards to represent a new piece on a square
    // does not remove info if a piece was already on that square
	void Position::put_piece(Piece pc, Square sq) 
	{
		set_bit(piece_bitboard[pc], sq);
		set_bit(occ_bitboard[get_side(pc)], sq);
        set_bit(occ_bitboard[BOTH], sq);
        piece_board[sq] = pc;
	}

    // updates bitboards to represent a piece being removed from a square
    void Position::remove_piece(Square sq)
    {
        pop_bit(piece_bitboard[piece_board[sq]], sq);
        pop_bit(occ_bitboard[get_side(piece_board[sq])], sq);
        pop_bit(occ_bitboard[BOTH], sq);
        piece_board[sq] = NO_PIECE;
    }

    // executes a move and updates the position
    bool Position::do_move(Move m, bool only_captures) 
    {
        if (only_captures == false || move_capture(m))
        {
            BoardState* bs_new = new BoardState;
            assert(bs_new != bs);
            // copy old boardstate info to new boardstate and update clocks
            bs_new->enpassant = bs->enpassant;
            bs_new->castle = bs->castle;
            bs_new->hmc = bs->hmc + 1;
            bs_new->fmc = bs->fmc + (side == BLACK);
            bs_new->prev = bs;
            // position now refers to new boardstate
            bs = bs_new;

            // null moves
            if (m == MOVE_NULL)
            {
                bs->enpassant = SQ_NONE;
                bs->captured_piece = NO_PIECE;
                goto nullmove;
            }

            Square src = move_from_sq(m);
            Square tar = move_to_sq(m);
            Piece piece = move_piece_type(m);

            assert(get_side(move_piece_type(m)) == side);
            assert(get_side(piece_board[src]) == side);
            assert(piece_board[tar] == NO_PIECE || get_side(piece_board[tar]) != side);
            assert(piece_type(piece_board[tar]) != KING);

            // move piece
            bs->captured_piece = piece_board[tar];

            remove_piece(tar); // only necessary for caps
            put_piece(piece, tar);
            remove_piece(src);

            if (move_castling(m))
            {
                Square rt;
                Square rf;
                if (king_side_castle(src, tar))
                {
                    rt = make_square(FILE_F, rank_of(src));
                    rf = rt + 2 * EAST;
                }
                else
                {
                    rt = make_square(FILE_D, rank_of(src));
                    rf = rt + 3 * WEST;
                }
                remove_piece(rf);
                put_piece(make_piece(ROOK, side), rt);
            }

            // update castling rights
            bs->castle &= castling_rights[src];
            bs->castle &= castling_rights[tar];

            if (move_capture(m))
            {
                if (move_enpassant(m))
                {
                    bs->captured_piece = make_piece(PAWN, other_side(side));
                    remove_piece(tar - pawn_push(side));
                }
                bs->hmc = 0;
            }

            bs->enpassant = SQ_NONE;

            if (piece_type(piece) == PAWN)
            {
                if (move_double(m))
                {
                    bs->enpassant = tar - pawn_push(side);
                }
                else if (move_promotion_type(m))
                {
                    remove_piece(tar);
                    put_piece(move_promotion_type(m), tar);
                }
                bs->hmc = 0;
            }

        nullmove:

            side = other_side(side);

            // move gen doesnt check for suicidal king, so we check here
            if (is_king_in_check(other_side(side)))
            {
                undo_move(m);
                return false;
            }
            return true;
        }
        return false;
    }

    // reverts a move and rolls back the position
    void Position::undo_move(Move m)
    {
        side = other_side(side);

        // null moves
        if (m == MOVE_NULL)
            goto nullmove;

        Square src = move_from_sq(m);
        Square tar = move_to_sq(m);

        put_piece(move_piece_type(m), src);
        remove_piece(tar);

        if (move_castling(m))
        {
            Square rt;
            Square rf;
            if (king_side_castle(src, tar))
            {
                rt = make_square(FILE_F, rank_of(src));
                rf = rt + 2 * EAST;
            }
            else
            {
                rt = make_square(FILE_D, rank_of(src));
                rf = rt + 3 * WEST;
            }
            remove_piece(rt);
            put_piece(make_piece(ROOK, side), rf);
        }

        if (move_capture(m))
        {
            if (move_enpassant(m))
            {
                put_piece(bs->captured_piece, tar - pawn_push(side));
            }
            else
            {
                put_piece(bs->captured_piece, tar);
            }
        }

    nullmove:

        assert(bs->prev != NULL);
        BoardState* temp = bs;
        bs = bs->prev;
        delete temp;
    }

    // prints the current position
    void Position::print_position()
    {
        std::cout << "+---+---+---+---+---+---+---+---+\n";
        for (Rank r = RANK_8; r >= 0; --r)
        {
            for (File f = FILE_A; f < FILE_NB; ++f)
            {
                Square sq = make_square(f, r);

                int bb_piece;
                for (bb_piece = W_PAWN; bb_piece <= B_KING; ++bb_piece)
                    if (get_bit(piece_bitboard[bb_piece], sq))
                        break;

                std::cout << "| " << ((bb_piece > B_KING) ? ' ' : piece_str[bb_piece]) << " ";
            }
            std::cout << "|" + std::to_string(1 + r) + "\n" + "+---+---+---+---+---+---+---+---+\n";
        }
        std::cout << "  a   b   c   d   e   f   g   h\n"
            << "Side:\t\t" << (side == WHITE ? "white" : "black") << "\n"
            << "Enpassant:\t" << ((bs->enpassant != SQ_NONE) ? sq2str(bs->enpassant) : "none") << "\n"
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
    void Position::print_attacked_squares(Colour s)
    {
        std::cout << "+---+---+---+---+---+---+---+---+\n";
        for (Rank r = RANK_8; r >= RANK_1; --r)
        {
            for (File f = FILE_A; f <= FILE_H; ++f)
            {
                std::cout << (is_attacked(make_square(f, r), s) ? "| X " : "|   ");
            }
            std::cout << "|" + std::to_string(1 + r) + "\n" + "+---+---+---+---+---+---+---+---+\n";
        }
    }

    std::string sq2str(Square s) {
        return std::string{ char('a' + file_of(s)), char('1' + rank_of(s)) };
    }

} // namespace Clovis