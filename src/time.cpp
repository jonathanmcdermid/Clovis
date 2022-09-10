#include "time.h"

using namespace std;

namespace Clovis {

	TimeManager tm; // global time manager

	void TimeManager::set() { 
		start_time = now(); 
	}
}