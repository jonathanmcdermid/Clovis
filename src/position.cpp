#include "position.h"

using namespace std;

namespace Clovis {

	const extern string piece_str = " PNBRQK  pnbrqk";

	// castling rights lookup table
	constexpr int castling_rights[SQ_N] = {
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

		void init_zobrist()
		{
			for (int i = NO_PIECE; i <= B_KING; ++i)
				for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
  					piece_square[i][sq] = Random::random_U64();
			for (Square sq = SQ_ZERO; sq < SQ_N; ++sq)
				enpassant[sq] = Random::random_U64();
			for (int i = 0; i < 16; ++i)
				castling[i] = Random::random_U64();
			side = Random::random_U64();
		}

		Key piece_square[15][SQ_N];
		Key enpassant[SQ_N];
		Key castling[16];
		Key side;

	} // namespace Zobrist
	
	// returns if a square is in danger of a discovery attack by a rook or bishop
	template<Colour US>
	bool Position::discovery_threat(Square sq) const 
	{
		// pawn is moveless if it attacks no enemies and is blocked by a piece
		// we dont have to worry about shift because discovery pawns will never be on outer files
		Bitboard their_moveless_pawns = 
		 (shift<pawn_push(US)>(occ_bb[BOTH]) & pc_bb[make_piece(PAWN, ~US)]) & 
		~(shift<pawn_push(US) + EAST>(occ_bb[US]) | shift<pawn_push(US) + WEST>(occ_bb[US]));

		if (side == ~US && bs->enpassant != SQ_NONE)
			their_moveless_pawns &= ~(shift<pawn_push(US) + EAST>(sqbb(bs->enpassant)) | shift<pawn_push(US) + WEST>(sqbb(bs->enpassant)));

		Bitboard candidates = 
		 ((Bitboards::get_attacks<ROOK>(pc_bb[W_PAWN] | pc_bb[B_PAWN], sq) & (pc_bb[make_piece(ROOK, ~US)])) 
		| (Bitboards::get_attacks<BISHOP>(pc_bb[make_piece(PAWN, US)] | their_moveless_pawns, sq) & (pc_bb[make_piece(BISHOP, ~US)])));
        
		Bitboard occupancy = occ_bb[BOTH] ^ candidates;

		while (candidates)
			if (popcnt(between_squares(sq, pop_lsb(candidates)) & occupancy) == 1)
				return true;

		return false;
	}
	
	// explicit template instantiations
	template bool Position::discovery_threat<WHITE>(Square sq) const;
	template bool Position::discovery_threat<BLACK>(Square sq) const;

	// sets position to the state specified by FEN string
	void Position::set(const char* fen)
	{
		memset(this, 0, sizeof(Position));
		bs = new BoardState;

		istringstream ss(fen);
		unsigned char token;

		int index;
		Square sq = A8;

		ss >> noskipws;

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
			if      (token == 'K') bs->castle |= WHITE_KS;
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

		ss >> skipws >> bs->hmc >> bs->fmc;
		bs->key = make_key();
		bs->pkey = make_pawn_key();
		bs->ply_null = 0;
	}

	Key Position::make_key()
	{
		Key k = 0ULL;

		for (Square sq = SQ_ZERO; sq < SQ_N; ++sq) 
		{
			Piece p = pc_table[sq];

			if (p != NO_PIECE)
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
			Piece p = pc_table[sq];
			if (piece_type(p) == PAWN || piece_type(p) == KING)
				k ^= Zobrist::piece_square[p][sq];
		}

		return k;
	}

	Bitboard Position::attackers_to(Square sq) const 
	{
		return  (Bitboards::pawn_attacks[BLACK][sq] &  pc_bb[W_PAWN]) 
		| (Bitboards::pawn_attacks[WHITE][sq] &  pc_bb[B_PAWN])
		| (Bitboards::knight_attacks[sq]      & (pc_bb[W_KNIGHT] | pc_bb[B_KNIGHT]))
		| (Bitboards::king_attacks[sq]        & (pc_bb[W_KING]   | pc_bb[B_KING]))
		| (Bitboards::get_attacks<ROOK>  (occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK] | pc_bb[B_ROOK]))
		| (Bitboards::get_attacks<BISHOP>(occ_bb[BOTH], sq) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]));
	}

	// returns the piece type of the least valuable piece on a bitboard of attackers
	Square Position::get_smallest_attacker(Bitboard attackers, const Colour stm) const
	{
		Bitboard bb;
        
		for (Piece p = make_piece(PAWN, stm); piece_type(p) <= KING; ++p)
			if ((bb = pc_bb[p] & attackers))
				return lsb(bb);
        
		return SQ_NONE;
	}

	// updates a bitboard of attackers after a piece has moved to include possible x ray attackers
	Bitboard Position::consider_xray(Bitboard occ, Square to, PieceType pt) const
	{
		switch (pt)
		{
		case PAWN:
		case BISHOP:
			return occ & (Bitboards::get_attacks<BISHOP> (occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]));
		case ROOK:
			return occ & (Bitboards::get_attacks<ROOK>   (occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK]   | pc_bb[B_ROOK]));
		case QUEEN:
			return occ & ((Bitboards::get_attacks<BISHOP>(occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_BISHOP] | pc_bb[B_BISHOP]))
			|  (Bitboards::get_attacks<ROOK>  (occ, to) & (pc_bb[W_QUEEN] | pc_bb[B_QUEEN] | pc_bb[W_ROOK]   | pc_bb[B_ROOK])));
		default:
			return 0ULL;
		}
	}

	bool Position::see_ge(Move move, int threshold) const
	{
		// dont even bother
		if (move_promotion_type(move) || move_enpassant(move))
			return true;

		int gain[32], d = 0;

		Square from = move_from_sq(move);
		Square to = move_to_sq(move);
		Bitboard occ = occ_bb[BOTH];
		Bitboard attackers = attackers_to(to);

		Colour stm = side;

		gain[d] = piece_value[pc_table[to]];

		do {
			stm = ~stm;
			d++;
			assert(d < 32);
			gain[d] = piece_value[pc_table[from]] - gain[d - 1]; 

			if (max(-gain[d - 1], gain[d]) < threshold) 
				break;

			attackers ^= from;
			occ ^= from;
			attackers |= consider_xray(occ, to, piece_type(pc_table[from]));
			from = get_smallest_attacker(attackers, stm);
		} while (from != SQ_NONE);

		while (--d)
			gain[d - 1] = -max(-gain[d - 1], gain[d]);

		return gain[0] >= threshold;
	}

	// updates bitboards to represent a new piece on a square
	// does not remove info if a piece was already on that square
	void Position::put_piece(Piece pc, Square sq) 
	{
		assert(!(sq & pc_bb[pc]));
		assert(!(sq & occ_bb[get_side(pc)]));
		assert(!(sq & occ_bb[BOTH]));

		pc_bb[pc] |= sq;
		occ_bb[get_side(pc)] |= sq;
		occ_bb[BOTH] |= sq;
		pc_table[sq] = pc;
	}

	// updates bitboards to represent a piece being removed from a square
	void Position::remove_piece(Square sq)
	{
		Piece pc = pc_table[sq];

		assert(sq & pc_bb[pc]);
		assert(sq & occ_bb[get_side(pc)]);
		assert(sq & occ_bb[BOTH]);

		pc_bb[pc] ^= sq;
		occ_bb[get_side(pc)] ^= sq;
		occ_bb[BOTH] ^= sq;
		pc_table[sq] = NO_PIECE;
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

		if (move == MOVE_NULL)
		{
			if (bs->enpassant != SQ_NONE)
			{
				bs->key ^= Zobrist::enpassant[bs->enpassant];
				bs->enpassant = SQ_NONE;
			}
			bs->captured_piece = NO_PIECE;
			bs->ply_null = 0;
		}
		else
		{
			assert(get_side(move_piece_type(move)) == side);
			assert(get_side(pc_table[src]) == side);
			assert(pc_table[tar] == NO_PIECE || get_side(pc_table[tar]) != side);
			assert(piece_type(pc_table[tar]) != KING);

			// move piece
			bs->captured_piece = pc_table[tar];

			if (move_castling(move))
			{
				Square rt, rf;
				get_castle_rook_squares(tar, rf, rt);
				bs->key ^= Zobrist::piece_square[pc_table[rf]][rf];
				bs->key ^= Zobrist::piece_square[pc_table[rf]][rt];
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
					bs->captured_piece = make_piece(PAWN, ~side);
					bs->key  ^= Zobrist::piece_square[bs->captured_piece][victim_sq];
					bs->pkey ^= Zobrist::piece_square[bs->captured_piece][victim_sq];
					remove_piece(victim_sq);
				}
				else
				{
					if (piece_type(bs->captured_piece) == PAWN)
						bs->pkey ^= Zobrist::piece_square[bs->captured_piece][tar];

					bs->key ^= Zobrist::piece_square[pc_table[tar]][tar];
					remove_piece(tar);
				}
				bs->game_phase -= game_phase_inc[bs->captured_piece];
				bs->hmc = 0;
			}

			put_piece(piece, tar);
			bs->key ^= Zobrist::piece_square[pc_table[src]][src];
			bs->key ^= Zobrist::piece_square[pc_table[src]][tar];
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
					bs->key  ^= Zobrist::piece_square[pc_table[tar]][tar];
					bs->pkey ^= Zobrist::piece_square[piece][tar];
					remove_piece(tar);
					put_piece(move_promotion_type(move), tar);
					bs->key ^= Zobrist::piece_square[pc_table[tar]][tar];
					bs->game_phase -= game_phase_inc[PAWN];
					bs->game_phase += game_phase_inc[move_promotion_type(move)];
				}
				bs->pkey ^= Zobrist::piece_square[piece][src];
				bs->pkey ^= Zobrist::piece_square[piece][tar];
				bs->hmc = 0;
			}
			else if (piece_type(piece) == KING)
			{
				bs->pkey ^= Zobrist::piece_square[piece][src];
				bs->pkey ^= Zobrist::piece_square[piece][tar];
			}
		}

		bs->key ^= Zobrist::side;

		// move gen doesnt check for suicidal king, so we check here
		if (is_king_in_check())
		{
			side = ~side;
			undo_move(move);
			return false;
		}

		side = ~side;
		return true;
	}

	// reverts a move and rolls back the position
	void Position::undo_move(Move move)
	{
		side = ~side;

		Square src = move_from_sq(move);
		Square tar = move_to_sq(move);

		if (move != MOVE_NULL)
		{
			put_piece(move_piece_type(move), src);
			remove_piece(tar);

			if (move_castling(move))
			{
				Square rt, rf;
				get_castle_rook_squares(tar, rf, rt);
				remove_piece(rt);
				put_piece(make_piece(ROOK, side), rf);
			}

			if (move_capture(move))
				put_piece(bs->captured_piece, move_enpassant(move) ? tar - pawn_push(side) : tar);
		}

		assert(bs->prev);
		BoardState* temp = bs;
		bs = bs->prev;
		delete temp;
	}

	bool Position::is_repeat() const 
	{
		BoardState* temp = bs;
       
		for (int end = min(bs->hmc, bs->ply_null); end >= 4; end -= 4)
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
		cout << "+---+---+---+---+---+---+---+---+" << endl;

		for (Rank r = RANK_8; r >= 0; --r)
		{
			for (File f = FILE_A; f < FILE_N; ++f)
			{
				Square sq = make_square(f, r);
				int bb_piece;

				for (bb_piece = W_PAWN; bb_piece <= B_KING; ++bb_piece)
					if (pc_bb[bb_piece] & sq)
						break;

				cout << "| " << ((bb_piece > B_KING) ? ' ' : piece_str[bb_piece]) << " ";
			}
			cout << "|" << to_string(1 + r) << endl << "+---+---+---+---+---+---+---+---+" << endl;
		}

		cout << "  a   b   c   d   e   f   g   h" << endl
		<< "Side:\t\t" 
		<< (side == WHITE ? "white" : "black") << endl
		<< "Enpassant:\t" 
		<< bs->enpassant << endl
		<< "Castling:\t"
		<< ((bs->castle & WHITE_KS) ? 'K' : '-')
		<< ((bs->castle & WHITE_QS) ? 'Q' : '-')
		<< ((bs->castle & BLACK_KS) ? 'k' : '-')
		<< ((bs->castle & BLACK_QS) ? 'q' : '-') << endl
		<< "Halfmove:\t" << bs->hmc << endl
		<< "Fullmove:\t" << bs->fmc << endl;
	}

	// prints the bitboards for this position
	void Position::print_bitboards() 
	{
		// there is a gap in value between W_KING AND B_PAWN
		// therefore some of the bitboards printed are empty
		cout << "Printing Bitboards\n";
		for (auto it : pc_bb)
			Bitboards::print_bitboard(it);
		for (auto it : occ_bb)
			Bitboards::print_bitboard(it);
	}

} // namespace Clovis
