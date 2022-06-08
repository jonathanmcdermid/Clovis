#pragma once

#include "UCI.h"
#include "position.h"
#include "evaluate.h"
#include "movelist.h"
#include "movepicker.h"
#include "time.h"

namespace Clovis {

    namespace Search {

        struct SearchLimits {
        public:

            SearchLimits() {
                time[WHITE] = time[BLACK] = 5000;
                inc[WHITE] = inc[BLACK] = move_time = start_time = 0;
                moves_left = depth = mate = perft = 0;
                infinite = false;
                nodes = 0;
            }

            unsigned time[COLOUR_N], inc[COLOUR_N], move_time, start_time;
            unsigned moves_left, depth, mate, perft;
            bool infinite;
            U32 nodes;
        };

        Move start_search(Position& pos, SearchLimits& lim);
        int negamax(Position& pos, int alpha, int beta, int depth, int ply);
        int quiescent(Position& pos, int alpha, int beta, int ply);
        void init_search();
        void clear();

        extern SearchLimits limits;

    } // namespace Search

} // namespace Clovis