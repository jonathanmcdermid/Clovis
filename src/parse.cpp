#include "parse.h"
#include "movepicker.h"

using namespace std;

namespace Clovis {

	namespace Parse {

		Move parse(const Position& pos, string move) {
			if (move[move.length() - 1] == '+' || move[move.length() - 1] == '#')
				move = move.substr(0, move.length() - 1);

			if (move.find("O-O") != string::npos)
				return encode_move(relative_square(pos.side, E1), 
					relative_square(pos.side, move == "O-O" ? G1 : C1), 
					make_piece(KING, pos.side), NO_PIECE, 0, 0, 0, 1);
			else if (islower(move[0])) { // pawn moves
				Piece promo = move[move.length() - 2] == '=' 
					? make_piece((PieceType) piece_str.find(move[move.length() - 1]), pos.side) 
					: NO_PIECE;
				Square to = (promo == NO_PIECE) 
					? str2sq(move.substr(move.length() - 2))
					: str2sq(move.substr(move.length() - 4, 2));
				Square from = (move[1] == 'x')
					? make_square(File(move[0] - 'a'), rank_of(to - pawn_push(pos.side)))
					: pos.pc_table[to - pawn_push(pos.side)] == NO_PIECE 
					? to - 2 * pawn_push(pos.side) 
					: to - pawn_push(pos.side);

				return encode_move(from, to, make_piece(PAWN, pos.side), 
					promo, move.find('x') != string::npos, 
					abs(rank_of(to) - rank_of(from)) == 2, pos.bs->enpassant == to, 0);
			} else { // major moves
				Piece piece = make_piece((PieceType) piece_str.find(move[0]), pos.side);
				Square to = str2sq(move.substr(move.length() - 2));
				Bitboard bb = Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
				Square from = pop_lsb(bb);

				if (move[1] == 'x' || move.length() == 3) {
					// one of the pieces that attacks this square is pinned
					if (bb) {
						if (pos.side == WHITE)
							while (pos.get_pinner<WHITE>(from) != SQ_NONE)
								from = pop_lsb(bb);
						else
							while (pos.get_pinner<BLACK>(from) != SQ_NONE)
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
				
				return encode_move(from, to, piece, NO_PIECE, move.find('x') != string::npos, 0, 0, 0);
			}
		}

		void generate_dataset() {

			string i_file_name = "src/games.pgn";
			string o_file_name = "src/tuner.epd";

			ifstream ifs;
			ofstream ofs;

			ifs.open(i_file_name.c_str(), ifstream::in);
			ofs.open(o_file_name.c_str(), ofstream::out);

			string line, result, fen;
			
			while (!ifs.eof()) {
				while (getline(ifs, line)) {
					if (line.find("Result") != string::npos) {
						size_t start = line.find ("\"") + 1;
						size_t end   = line.rfind("\"");
						result = line.substr(start, end - start);
						break;
					}
				}

				while (getline(ifs, line)) {
					if (line.find("FEN") != string::npos) {
						size_t start = line. find("\"") + 1;
						size_t end   = line.rfind("\"");
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
						if (token.find(".") == string::npos) {
							if (!pos.do_move(parse(pos, token)))
								exit(EXIT_FAILURE);

							if (pos.bs->fmc > 8
							&& token[token.length() - 1] != '#' 
							&& token[token.length() - 1] != '+'
							&& Random::random_U64() % 5 == 0) {
								Search::SearchLimits limits;
								limits.depth = 1;
								Search::SearchInfo info;
								Search::start_search(pos, limits, info);
								
								if (info.score < MIN_CHECKMATE_SCORE && info.score > -MIN_CHECKMATE_SCORE) {
									for (const auto& it : info.pline)
										pos.do_move(it);

									if (find(keys.begin(), keys.end(), pos.bs->key) == keys.end()) {
										int eval = Eval::evaluate<false>(pos);
										if (pos.side == BLACK)
											eval = -eval;
										if ((result == "1-0" && eval > -500)
										|| (result == "0-1" && eval < 500)
										|| (result == "1/2-1/2" && (eval > -500 && eval < 500))) {
											keys.push_back(pos.bs->key);
											ofs << pos.get_fen() + " \"" + result + "\";" << endl;
										}
									}

									for_each(make_reverse_iterator(info.pline.last),
										make_reverse_iterator(info.pline.moves.data()),
										[&](Move& m) { pos.undo_move(m); });
								}
							}
						}
					}
				} while (live && getline(ifs, line));
			}

			ifs.close();
			ofs.close();
		}

	} // namespace Parse

} // namespace Clovis
