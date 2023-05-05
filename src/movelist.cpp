#include "movelist.h"

using namespace std;

namespace clovis {
    
	namespace move_gen {
    
		template<typename T, MoveType M, PieceType PT, Colour US>
		T* generate_majors(const Position& pos, T* moves) {

			Bitboard bb = pos.pc_bb[make_piece(PT, US)];

			Bitboard tar_bb = M == ALL_MOVES   ? ~pos.occ_bb[US] 
				        : M == QUIET_MOVES ? ~pos.occ_bb[BOTH]
					                   :  pos.occ_bb[~US];

			while (bb) {

				const Square src = pop_lsb(bb);
				Bitboard att = bitboards::get_attacks<PT>(pos.occ_bb[BOTH], src) & tar_bb;

				while (att) {
					const Square tar = pop_lsb(att);
					*moves++ = encode_move(src, tar, make_piece(PT, US), NO_PIECE, M == QUIET_MOVES ? false : pos.occ_bb[BOTH] & tar, false, false, false);
				}
			}

			return moves;
		}

		template<typename T, MoveType M, Colour US, bool TC>
		T* generate_promotions(T* moves, const Square src, const Square tar) {

			if constexpr (M != QUIET_MOVES)
				*moves++ = encode_move(src, tar, make_piece(PAWN, US), make_piece(QUEEN,  US), TC, false, false, false);
			if constexpr (M != CAPTURE_MOVES) {
				*moves++ = encode_move(src, tar, make_piece(PAWN, US), make_piece(KNIGHT, US), TC, false, false, false);
				*moves++ = encode_move(src, tar, make_piece(PAWN, US), make_piece(BISHOP, US), TC, false, false, false);
				*moves++ = encode_move(src, tar, make_piece(PAWN, US), make_piece(ROOK,   US), TC, false, false, false);
			}

			return moves;
		}

		// gen pseudo-legal moves for a position
		template<typename T, MoveType M, Colour US>
		T* generate_all(const Position& pos, T* moves) {

			constexpr bool CAPTURES = M != QUIET_MOVES;
			constexpr bool QUIETS	= M != CAPTURE_MOVES;
			
			Bitboard bb = pos.pc_bb[make_piece(PAWN, US)];

			while (bb) {

				const Square src = pop_lsb(bb);
				Square tar = src + pawn_push(US);

				if (rank_of(src) == relative_rank(US, RANK_7)) {

					Bitboard att = bitboards::pawn_attacks[US][src] & pos.occ_bb[~US];

					if (!(pos.occ_bb[BOTH] & tar))
						moves = generate_promotions<T, M, US, false>(moves, src, tar);

					while (att) {
						tar = pop_lsb(att);
						moves = generate_promotions<T, M, US, true>(moves, src, tar);
					}
				} else {
					if (QUIETS && !(pos.occ_bb[BOTH] & tar)) {
						// single push
						*moves++ = encode_move(src, tar, make_piece(PAWN, US), NO_PIECE, false, false, false, false);
						// double push
						if (rank_of(src) == relative_rank(US, RANK_2) && !(pos.occ_bb[BOTH] & (tar + pawn_push(US))))
							*moves++ = encode_move(src, tar + pawn_push(US), make_piece(PAWN, US), NO_PIECE, false, true, false, false);
 					}
					if constexpr (CAPTURES) {

						Bitboard att = bitboards::pawn_attacks[US][src] & pos.occ_bb[~US];

						while (att) {
							tar = pop_lsb(att);
							*moves++ = encode_move(src, tar, make_piece(PAWN, US), NO_PIECE, true, false, false, false);
						}

						if (bitboards::pawn_attacks[US][src] & pos.bs->enpassant)
							*moves++ = encode_move(src, pos.bs->enpassant, make_piece(PAWN, US), NO_PIECE, true, false, true, false);
					}
				}
			}

			if (QUIETS && !pos.is_attacked<US>(relative_square(US, E1))) {

				if (pos.bs->castle & ks_castle_rights(US)
				&& !(pos.occ_bb[BOTH] & (relative_square(US, F1) | relative_square(US, G1)))
				&& !pos.is_attacked<US>(relative_square(US, F1)))
					*moves++ = encode_move(relative_square(US, E1), relative_square(US, G1), make_piece(KING, US), NO_PIECE, false, false, false, true);

				if (pos.bs->castle & qs_castle_rights(US)
				&& !(pos.occ_bb[BOTH] & (relative_square(US, B1) | relative_square(US, C1) | relative_square(US, D1)))
				&& !pos.is_attacked<US>(relative_square(US, D1)))
					*moves++ = encode_move(relative_square(US, E1), relative_square(US, C1), make_piece(KING, US), NO_PIECE, false, false, false, true);
			}

			moves = generate_majors<T, M, KNIGHT, US>(pos, moves);
			moves = generate_majors<T, M, BISHOP, US>(pos, moves);
			moves = generate_majors<T, M, ROOK,   US>(pos, moves);
			moves = generate_majors<T, M, QUEEN,  US>(pos, moves);
			moves = generate_majors<T, M, KING,   US>(pos, moves);
			
			return moves;
		}

		template<typename T, MoveType M> 
		T* generate(const Position& pos, T* moves) {
			return (pos.side == WHITE)
			? generate_all<T, M, WHITE>(pos, moves)
			: generate_all<T, M, BLACK>(pos, moves);
		}

		template<typename T>
		void print_moves(const T* m, const T* end) {
			
			cout << "move\tpiece\tcapture\tdouble\tenpass\tcastle";

			if constexpr (is_same<T, ScoredMove>())
				cout << "\tscore";

			cout << endl;

			int count = 0;

			while (m != end) {
				cout << move_from_sq(*m)
				<< move_to_sq(*m)
				<< piece_str[move_promotion_type(*m)] << '\t'
				<< piece_str[move_piece_type(*m)]     << '\t'
				<< static_cast<int>(move_capture(*m))              << '\t'
				<< static_cast<int>(move_double(*m))               << '\t'
				<< static_cast<int>(move_enpassant(*m))            << '\t'
				<< static_cast<int>(move_castling(*m))             << '\t';

				if constexpr (is_same<T, ScoredMove>())
					cout << m->score;

				cout << endl;

				++m, ++count;
			}

			cout << "Total move count:" << count << endl;
		}
		
		// explicit template instantiations
		template Move* generate<Move, QUIET_MOVES>(const Position& pos, Move* moves);
		template Move* generate<Move, CAPTURE_MOVES>(const Position& pos, Move* moves);
		template Move* generate<Move, ALL_MOVES>(const Position& pos, Move* moves);
		template ScoredMove* generate<ScoredMove, QUIET_MOVES>(const Position& pos, ScoredMove* moves);
		template ScoredMove* generate<ScoredMove, CAPTURE_MOVES>(const Position& pos, ScoredMove* moves);
		template ScoredMove* generate<ScoredMove, ALL_MOVES>(const Position& pos, ScoredMove* moves);
		template void print_moves<Move>(const Move* m, const Move* end);
		template void print_moves<ScoredMove>(const ScoredMove* m, const ScoredMove* end);
    
	} // namespace move_gen

} // namespace clovis
