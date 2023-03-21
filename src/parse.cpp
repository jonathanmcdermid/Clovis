#include "parse.h"
#include "movepicker.h"

namespace Clovis {

	namespace Parse {

		Move parse(const Position& pos, string move)
		{
			Move res = MOVE_NONE;

			if (move[move.length() - 1] == '+' || move[move.length() - 1] == '#')
				move = move.substr(0, move.length() - 1);

			if (move == "O-O-O")
			{
				res = (pos.side == WHITE)
					? encode_move(E1, C1, W_KING, NO_PIECE, 0, 0, 0, 1) 
					: encode_move(E8, C8, B_KING, NO_PIECE, 0, 0, 0, 1);
			}
			else if (move == "O-O")
			{
				res = (pos.side == WHITE)
					? encode_move(E1, G1, W_KING, NO_PIECE, 0, 0, 0, 1)
					: encode_move(E8, G8, B_KING, NO_PIECE, 0, 0, 0, 1);
			}
			else if (islower(move[0]))
			{
				Piece promotion = 
					  move[move.length() - 2] == '='
					? move[move.length() - 1] == 'Q' ? make_piece(QUEEN, pos.side)
					: move[move.length() - 1] == 'R' ? make_piece(ROOK, pos.side)
					: move[move.length() - 1] == 'B' ? make_piece(BISHOP, pos.side)
					: make_piece(KNIGHT, pos.side) : NO_PIECE;

				if (move[1] == 'x')
				{
					Square to = str2sq(move.substr(2, 2));
					Square from = make_square(File(move[0] - 'a'), rank_of(to - pawn_push(pos.side)));
					res = encode_move(from, to, make_piece(PAWN, pos.side), promotion, 1, 0, pos.bs->enpassant == to, 0);
				}
				else
				{
					Square to = str2sq(move);
					Square from = pos.pc_table[to - pawn_push(pos.side)] == NO_PIECE 
						? to - 2 * pawn_push(pos.side) 
						: to - pawn_push(pos.side);
					res = encode_move(from, to, make_piece(PAWN, pos.side), promotion, 0, abs(rank_of(to) - rank_of(from)) == 2, 0, 0);
				}
			}
			else
			{
				Piece piece = make_piece(
					  move[0] == 'K' ? KING
					: move[0] == 'Q' ? QUEEN
					: move[0] == 'R' ? ROOK
					: move[0] == 'B' ? BISHOP
					: KNIGHT, pos.side);

				if (move[1] == 'x')
				{
					Square to = str2sq(move.substr(2, 2));
					Bitboard bb = Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
					Square from = pop_lsb(bb);
					if (bb)
					{
						// one of the pieces that attacks this square is pinned
						if (pos.side == WHITE)
							while (pos.get_pinner<WHITE>(from) != SQ_NONE)
								from = pop_lsb(bb);
						else
							while (pos.get_pinner<BLACK>(from) != SQ_NONE)
								from = pop_lsb(bb);
					}
					res = encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0);
				}
				else if (move[2] == 'x')
				{
					Square to = str2sq(move.substr(3, 2));
					Bitboard bb = Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
					Square from = pop_lsb(bb);
					if (isdigit(move[1]))
						while (rank_of(from) != Rank(move[1] - '1'))
							from = pop_lsb(bb);
					else
						while (file_of(from) != File(move[1] - 'a'))
							from = pop_lsb(bb);
					res = encode_move(from, to, piece, NO_PIECE, 1, 0, 0, 0);
				}
				else if (move.length() == 3)
				{
					Square to = str2sq(move.substr(1, 2));
					Bitboard bb = Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
					Square from = pop_lsb(bb);
					if (bb)
					{
						// one of the pieces that attacks this square is pinned
						if (pos.side == WHITE)
							while (pos.get_pinner<WHITE>(from) != SQ_NONE)
								from = pop_lsb(bb);
						else
							while (pos.get_pinner<BLACK>(from) != SQ_NONE)
								from = pop_lsb(bb);
					}
					res = encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0);
				}
				else if (move.length() == 4)
				{
					Square to = str2sq(move.substr(2, 2));
					Bitboard bb = Bitboards::get_attacks(piece_type(piece), pos.occ_bb[BOTH], to) & pos.pc_bb[piece];
					Square from = pop_lsb(bb);
					if (isdigit(move[1]))
						while (rank_of(from) != Rank(move[1] - '1'))
							from = pop_lsb(bb);
					else
						while (file_of(from) != File(move[1] - 'a'))
							from = pop_lsb(bb);
					res = encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0);
				}
				else if (move.length() == 5)
				{
					Square to = str2sq(move.substr(3, 2));
					Square from = str2sq(move.substr(1, 2));
					res = encode_move(from, to, piece, NO_PIECE, 0, 0, 0, 0);
				}
			}
			
			MoveGen::MoveList ml = MoveGen::MoveList(pos);
			ml.remove_illegal(pos);

			for (const auto& m : ml)
				if (m == res)
					return res;

			exit(EXIT_FAILURE);
		}

		void generate_dataset()
		{
			string i_file_name = "src/games.pgn";
			string o_file_name = "src/tuner.epd";

			ifstream ifs;
			ofstream ofs;

			ifs.open(i_file_name.c_str(), ifstream::in);
			ofs.open(o_file_name.c_str(), ofstream::out);

			string line, result, fen;
			
			while (!ifs.eof())
			{
				while (getline(ifs, line))
				{
					if (line.find("Result") != string::npos)
					{
						size_t start = line.find("\"") + 1;
						size_t end   = line.rfind("\"");
						result = line.substr(start, end - start);
						break;
					}
				}

				while (getline(ifs, line))
				{
					if (line.find("FEN") != string::npos)
					{
						size_t start = line.find("\"") + 1;
						size_t end = line.rfind("\"");
						fen = line.substr(start, end - start);
						break;
					}
				}

				Position pos(fen.c_str());
				int fmc = 1;
				Colour turn = pos.side;
				bool live = true;
				vector<Key> keys;

				while (live && getline(ifs, line))
				{
					istringstream ss(line);
					while (line.find(to_string(fmc) + ". ") != string::npos || turn == BLACK || line.find(result) != string::npos)
					{
						string token;
						ss >> skipws >> token;
						if (token.empty())
							break;
						if (token == result)
						{
							live = false;
							break;
						}
						if (token.find(".") == string::npos)
						{
							pos.do_move(parse(pos, token));

							if (fmc > 16 
							&& token[token.length() - 1] != '#' 
							&& token[token.length() - 1] != '+'
							&& Random::random_U64() % 5 == 0)
							{
								Search::SearchLimits limits;
								limits.depth = 1;
								int score;
								U64 nodes;
								Search::Line pline;
								Search::start_search(pos, limits, score, nodes, pline);

								if (score < MIN_CHECKMATE_SCORE && score > -MIN_CHECKMATE_SCORE)
								{
									for (const auto& it : pline)
										pos.do_move(it);

									if (find(keys.begin(), keys.end(), pos.bs->key) == keys.end())
									{
										keys.push_back(pos.bs->key);
										ofs << pos.get_fen() + " \"" + result + "\";" << endl;
									}

									for (Move* m = pline.last - 1; m >= pline.moves; --m)
										pos.undo_move(*m);
								}
							}

							if ((turn = ~turn) == WHITE)
								++fmc;
						}
					}
				}
			}

			ifs.close();
			ofs.close();
		}

	} // namespace Parse

} // namespace Clovis
