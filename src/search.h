#pragma once

#include "movepicker.h"
#include "tt.h"

namespace clovis {

	namespace search {

		struct SearchLimits {
			constexpr SearchLimits() = default;

			bool infinite{ false };
			int move_time{ 0 }, start_time{ 0 }, moves_left{ 20 }, depth{ 0 }, mate{ 0 }, perft{ 0 };
			uint64_t nodes{ 0 };
			std::array<int, COLOUR_N> time{ {5000, 5000} }, inc{ {0, 0} };
		};

		struct Line {
			constexpr Line() = default;
			[[nodiscard]] int move_count()    const { return static_cast<int>(last - moves.data()); }
			[[nodiscard]] const Move* begin() const { return moves.data(); }
			[[nodiscard]] const Move* end()   const { return last; }

			std::array<Move, MAX_PLY> moves{};
			Move* last{ moves.data() };
		};

		struct SearchInfo {
			constexpr SearchInfo() = default;

			int16_t score{ 0 };
			uint64_t nodes{ 0ULL };
			Line pline;
		};

		void start_search(Position& pos, const SearchLimits& limits, SearchInfo& info);
		void clear();

	} // namespace search

} // namespace clovis
