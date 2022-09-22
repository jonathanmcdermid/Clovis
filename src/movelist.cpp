#include "movelist.h"

using namespace std;

namespace Clovis {

    namespace MoveGen {

        void MoveList::print()
        {
            cout << "\nmove\tpiece\tcapture\tdouble\tenpass\tcastling\n\n";

            int move_count = 0;
            for (Move* m = moves; m != last; ++m, ++move_count)
            {
                cout << sq2str(move_from_sq(*m))
                    << sq2str(move_to_sq(*m))
                    << piece_str[move_promotion_type(*m)] << '\t'
                    << piece_str[move_piece_type(*m)] << '\t'
                    << (int) move_capture(*m) << '\t'
                    << (int) move_double(*m) << '\t'
                    << (int) move_enpassant(*m) << '\t'
                    << (int) move_castling(*m) << '\n';

            }
            cout << "\n\nTotal move count:" << move_count;
        }

    } // namespace MoveGen

} // namespace Clovis
