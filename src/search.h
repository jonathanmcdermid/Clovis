#pragma once

#include <math.h>

#include "UCI.h"
#include "position.h"
#include "evaluate.h"
#include "movelist.h"
#include "movepicker.h"
#include "time.h"
#include "tt.h"

namespace Clovis {

    namespace Search {

        struct SearchLimits {
        public:

            SearchLimits() {
                time[WHITE] = time[BLACK] = 5000;
                inc[WHITE] = inc[BLACK] = move_time = start_time = 0;
                depth = 0;
                mate = perft = 0;
                moves_left = 20;
                infinite = false;
                nodes = 0;
            }

            int time[COLOUR_N], inc[COLOUR_N], move_time, start_time;
            int moves_left, depth, mate, perft;
            bool infinite;
            U32 nodes;
        };

        struct Line {
            Line() : last(moves) {}
            int move_count() const { return last - moves; }
            const Move* begin() const { return moves; }
            const Move* end() const { return last; }
            Move moves[MAX_PLY] = { MOVE_NONE }, *last;
        };

        Move start_search(Position& pos, SearchLimits& lim);
        int negamax(Position& pos, int alpha, int beta, int depth, int ply, bool is_null);
        int quiescent(Position& pos, int alpha, int beta);
        void init_search();
        void init_lmr_tables();
        void clear();

        extern SearchLimits limits;

    } // namespace Search

} // namespace Clovis