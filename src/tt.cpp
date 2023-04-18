#include "tt.h"

using namespace std;

namespace Clovis {

	TTable tt; // global transpotision table

	TTable::TTable() {
		resize(132);
		pt = new PTEntry[pt_size];
	}

	void TTable::resize(size_t mb) {
		delete ht;
		tt_size = mb * 1024 * 1024 / (sizeof(Bucket));
		ht = new Bucket[tt_size];
	}

	TTable::~TTable() {
		delete ht;
		delete pt;
	}

	// empty transposition table
	void TTable::clear() {
		memset(static_cast<void*>(ht), 0, tt_size * sizeof(Bucket));
		memset(static_cast<void*>(pt), 0, pt_size * sizeof(PTEntry));
	}

} // namespace Clovis
