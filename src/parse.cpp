#include <fstream>
#include <sstream>
#include <vector>

#include "parse.h"
#include "evaluate.h"
#include "movepicker.h"

using namespace std;

namespace clovis {

	namespace parse {

		Move parse(const Position& pos, string move) {
			if (move[move.length() - 1] == '+' || move[move.length() - 1] == '#')
				move = move.substr(0, move.length() - 1);

			if (move.find("O-O") != string::npos)
				return encode_move(relative_square(pos.side, E1),
					relative_square(pos.side, move == "O-O" ? G1 : C1),
					make_piece(KING, pos.side), NO_PIECE, false, false, false, true);
			if (islower(move[0])) { // pawn moves
				const Piece promo = move[move.length() - 2] == '='
					? make_piece(static_cast<PieceType>(piece_str.find(move[move.length() - 1])), pos.side)
					: NO_PIECE;
				const Square to = (promo == NO_PIECE)
					? str2sq(move.substr(move.length() - 2))
					: str2sq(move.substr(move.length() - 4, 2));
				const Square from = (move[1] == 'x')
					? make_square(static_cast<File>(move[0] - 'a'), rank_of(to - pawn_push(pos.side)))
					: pos.pc_table[to - pawn_push(pos.side)] == NO_PIECE
					? to - 2 * pawn_push(pos.side)
					: to - pawn_push(pos.side);

				return encode_move(from, to, make_piece(PAWN, pos.side),
					promo, move.find('x') != string::npos,
					abs(rank_of(to) - rank_of(from)) == 2, pos.bs->enpassant == to, false);
			}
			// major moves
			const Piece piece = make_piece(static_cast<PieceType>(piece_str.find(move[0])), pos.side);
			const Square to = str2sq(move.substr(move.length() - 2));
			Bitboard bb = bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
			Square from = pop_lsb(bb);

			if (move[1] == 'x' || move.length() == 3) {
				// one of the pieces that attacks this square is pinned
				if (bb) {
					if (pos.side == WHITE)
						while (pos.get_pinner<WHITE>(from).has_value())
							from = pop_lsb(bb);
					else
						while (pos.get_pinner<BLACK>(from).has_value())
							from = pop_lsb(bb);
				}
			}
			else if (move[2] == 'x' || move.length() == 4) {
				// there are multiple matching pieces that attack this square
				if (isdigit(move[1]))
					while (rank_of(from) != Rank(move[1] - '1'))
						from = pop_lsb(bb);
				else
					while (file_of(from) != File(move[1] - 'a'))
						from = pop_lsb(bb);
			}

			return encode_move(from, to, piece, NO_PIECE, move.find('x') != string::npos, false, false, false);
		}

		void generate_data() {

			ifstream ifs("src/games.pgn");
			ofstream ofs("src/tuner.epd");

			string line, result, fen;
			
			while (!ifs.eof()) {
				while (getline(ifs, line)) {
					if (line.find("Result") != string::npos) {
						size_t start = line.find ('\"') + 1, end = line.rfind('\"');
						result = line.substr(start, end - start);
						break;
					}
				}

				while (getline(ifs, line)) {
					if (line.find("FEN") != string::npos) {
						size_t start = line. find('\"') + 1, end = line.rfind('\"');
						fen = line.substr(start, end - start);
						break;
					}
				}

				Position pos(fen.c_str());

				while (getline(ifs, line))
					if (line.find(to_string(pos.bs->fmc) + "... ") != string::npos 
						|| line.find(to_string(pos.bs->fmc) + ". ") != string::npos)
						break;

				bool live = true;
				vector<Key> keys;

				do {
					istringstream ss(line);
					while (true) {
						string token;
						ss >> skipws >> token;
						if (token.empty())
							break;
						if (token == result) {
							live = false;
							break;
						}
						if (token.find('.') == string::npos) {
							if (!pos.do_move(parse(pos, token)))
								exit(EXIT_FAILURE);

							if (pos.bs->fmc > 8
							&& token[token.length() - 1] != '#' 
							&& token[token.length() - 1] != '+') {
								search::SearchLimits limits;
								limits.depth = 1;
								search::SearchInfo info;
								search::start_search(pos, limits, info);
								
								if (info.score < MIN_CHECKMATE_SCORE && info.score > -MIN_CHECKMATE_SCORE) {
									for (const auto& it : info.pline)
										pos.do_move(it);

									if (ranges::find(keys.begin(), keys.end(), pos.bs->key) == keys.end()) {
										if (const int eval = pos.side == WHITE ? eval::evaluate<false>(pos) : -eval::evaluate<false>(pos); 
										   (result == "1-0" && eval > -500)
										|| (result == "0-1" && eval < 500)
										|| (result == "1/2-1/2" && (eval > -500 && eval < 500))) {
											keys.push_back(pos.bs->key);
											ofs << pos.get_fen() + " \"" + result + "\";" << endl;
										}
									}

									for_each(make_reverse_iterator(info.pline.last),
										make_reverse_iterator(info.pline.moves.data()),
										[&](const Move& m) { pos.undo_move(m); });
								}
							}
						}
					}
				} while (live && getline(ifs, line));
			}

			ifs.close();
			ofs.close();
		}

	} // namespace parse

} // namespace clovis
