#pragma once

#include <chrono>

namespace Clovis {

	typedef std::chrono::milliseconds::rep TimePoint;

	class TimeManager {
	public:
		constexpr TimeManager(): start_time(0) {}
		inline void set();
		inline TimePoint get_time_elapsed();
		inline TimePoint now();
	private:
		TimePoint start_time;
	};

	inline TimePoint TimeManager::get_time_elapsed() {
		return now() - start_time;
	}

	inline TimePoint TimeManager::now() {
		return std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}

	inline void TimeManager::set() { 
		start_time = now(); 
	}

	extern TimeManager tm;

} // namespace Clovis
