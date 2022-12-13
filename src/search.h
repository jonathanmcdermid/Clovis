#pragma once

#include <math.h>

#include "uci.h"
#include "position.h"
#include "evaluate.h"
#include "movelist.h"
#include "movepicker.h"
#include "time.h"
#include "tt.h"

namespace Clovis {

    namespace Search {

		extern int iid_depth[2];
		extern int iid_factor[2];
		extern int iid_divisor[2];
		extern int iid_reduction[2];

		extern int asp_depth;
		extern int delta;

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
            U64 nodes;
        };

		struct Line {
            Line() : last(moves) {}
            int move_count() const { return last - moves; }
            const Move* begin() const { return moves; }
            const Move* end() const { return last; }
            Move moves[MAX_PLY] = { MOVE_NONE }, *last;
        };

        void start_search(Position& pos, SearchLimits& lim, Move& best_move, Move& ponder_move, int& score, U64& nodes);
        int negamax(Position& pos, int alpha, int beta, int depth, int ply, bool is_null, Move prev_move, U64& nodes, Line& pline);
        int quiescent(Position& pos, int alpha, int beta, U64& nodes);
        void init_search();
        void init_values();
        void clear();

        extern SearchLimits limits;

    } // namespace Search

} // namespace Clovis
