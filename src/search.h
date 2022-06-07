#pragma once

#include "position.h"

namespace Clovis {

    namespace Search {

        struct SearchLimits {
        public:

            SearchLimits() {
                time[WHITE] = time[BLACK] = inc[WHITE] = inc[BLACK] = moveTime = startTime = 0;
                moves_left = depth = mate = perft = 0;
                infinite = false;
                nodes = 0;
            }

            unsigned time[COLOUR_N], inc[COLOUR_N], moveTime, startTime;
            unsigned moves_left, depth, mate, perft;
            bool infinite;
            U32 nodes;
        };

        void start_search(Position& pos, SearchLimits& lim);
        void init_search();
        void clear();

        extern SearchLimits limits;

    } // namespace Search

} // namespace Clovis