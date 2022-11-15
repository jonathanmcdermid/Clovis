#include "movelist.h"

using namespace std;

namespace Clovis {

    namespace MoveGen {

        void MoveList::print()
        {
            cout << "\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\n\n";

            int move_count = 0;
            for (Move* move = moves; move != last; ++move, ++move_count)
            {
                cout << sq2str(move_from_sq(*move))
                    << sq2str(move_to_sq(*move))
                    << piece_str[move_promotion_type(*move)] << '\t'
                    << piece_str[move_piece_type(*move)] << '\t'
                    << int(move_capture(*move)) << '\t'
                    << int(move_double(*move)) << '\t'
                    << int(move_enpassant(*move)) << '\t'
                    << int(move_castling(*move)) << '\n';

            }
            cout << "\n\nTotal move count:" << move_count;
        }

    } // namespace MoveGen

} // namespace Clovis
