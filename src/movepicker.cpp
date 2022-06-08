#include "movepicker.h"

namespace Clovis {

	namespace MovePick {

		// return the next ordered move
		ScoredMove MovePicker::get_next()
		{
			if (curr == last)
				return ScoredMove();
			return *curr++;
		}

		void MovePicker::sm_sort() {
			std::sort(curr, curr + (last - curr), sm_score_comp);
		}

		inline static bool sm_score_comp(ScoredMove const& lhs, ScoredMove const& rhs) {
			return lhs.score > rhs.score;
		}

	} // namespace MovePick

} // namespace Clovis
