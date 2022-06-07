#pragma once

#include <chrono>

namespace Clovis {

	typedef std::chrono::milliseconds::rep TimePoint;

	class TimeManager {
	public:
		void set() { start_time = now(); }
		inline TimePoint get_time_elapsed();
		inline TimePoint now();
	private:
		TimePoint start_time;
	};

	inline TimePoint TimeManager::get_time_elapsed() {
		return now() - start_time;
	}

	inline TimePoint TimeManager::now() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

} // namespace Clovis