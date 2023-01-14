#pragma once

#include <chrono>

using namespace std::chrono;

namespace Clovis {

	typedef milliseconds::rep TimePoint;

	class TimeManager {
	public:
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
		return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
	}

	inline void TimeManager::set() { 
		start_time = now(); 
	}

	extern TimeManager tm;

} // namespace Clovis
