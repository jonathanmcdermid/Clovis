#include "tt.h"

using namespace std;

namespace clovis {

	TTable tt; // global transposition table

	TTable::TTable() : ht(nullptr), pt(nullptr) {
		resize(132);
		pt = make_unique<PTEntry[]>(pt_size);
	}

	void TTable::resize(const size_t mb) {
		tt_size = mb * 1024 * 1024 / (sizeof(TTBucket));
		ht = make_unique<TTBucket[]>(tt_size);
	}

	// empty transposition table
	void TTable::clear() {
		ht = make_unique<TTBucket[]>(tt_size);
		pt = make_unique<PTEntry[]>(pt_size);
	}

	// probe the table to see if an entry exists
	TTEntry TTable::probe(const Key key) {

		auto& [e1, e2] = ht[hash_index(key)];

		if (e1.key == key)
			return e1;
		if (e1.depth > 0)
			--e1.depth;

		return e2;
	}

	void TTable::new_entry(const Key key, const uint8_t depth, const int16_t eval, const HashFlag flags, const Move move) {
		TTBucket& bucket = ht[hash_index(key)];
		bucket[bucket.e1.depth > depth] = TTEntry(key, depth, flags, eval, move);
	}

} // namespace clovis
