#pragma once

#include <chrono>
#include <iomanip>
#include <climits>

#include "evaluate.h"
#include "movepicker.h"
#include "tt.h"

namespace Clovis {

	namespace Search {

		struct SearchLimits {
			constexpr SearchLimits() = default;

			std::array<int, COLOUR_N> time{ {5000, 5000} }, inc{ {0, 0} };
			int move_time{ 0 }, start_time{ 0 }, moves_left{ 20 }, depth{ 0 }, mate{ 0 }, perft{ 0 };
			bool infinite{ false };
			U64 nodes{0};
		};

		struct Line {
			constexpr Line() = default;
			int move_count()    const { return last - moves.data(); }
			const Move* begin() const { return moves.data(); }
			const Move* end()   const { return last; }

			std::array<Move, MAX_PLY> moves;
			Move* last{ moves.data() };
		};

		struct SearchInfo {
			constexpr SearchInfo() = default;

			int score{ 0 };
			U64 nodes{ 0ULL };
			Line pline;
		};

		void start_search(Position& pos, const SearchLimits& limits, SearchInfo& info);
		void clear();

	} // namespace Search

} // namespace Clovis
