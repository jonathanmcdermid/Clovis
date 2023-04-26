#include "tt.h"

using namespace std;

namespace Clovis {

	TTable tt; // global transpotision table

	TTable::TTable() : ht(nullptr), pt(nullptr) {
		resize(132);
		pt = make_unique<PTEntry[]>(pt_size);
	}

	void TTable::resize(size_t mb) {
		tt_size = mb * 1024 * 1024 / (sizeof(TTBucket));
		ht = make_unique<TTBucket[]>(tt_size);
	}

	// empty transposition table
	void TTable::clear() {
		ht.reset(new TTBucket[tt_size]);
    		pt.reset(new PTEntry[pt_size]);
	}

	// probe the table to see if an entry exists
	TTEntry* TTable::probe(Key key) {

		TTBucket& b = ht[hash_index(key)];

		if (b.e1.key == key)
			return &b.e1;
		if (b.e1.depth > 0)
			--b.e1.depth;

		return &b.e2;
	}

	void TTable::new_entry(Key key, int depth, int eval, HashFlag flags, Move move) {
		TTBucket& b = ht[hash_index(key)];
		(b.e1.depth <= depth)
			? b.e1 = TTEntry(key, depth, flags, eval, move)
			: b.e2 = TTEntry(key, depth, flags, eval, move);
	}

} // namespace Clovis
