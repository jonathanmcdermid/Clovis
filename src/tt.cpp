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

	// probe the table to see if an entry exists
	TTEntry* TTable::probe(Key key) {

		Bucket* b = ht + hash_index(key);

		if (b->e1.key == key)
			return &b->e1;
		if (b->e1.depth > 0)
			--b->e1.depth;

		return (b->e2.key == key) ? &b->e2 : nullptr;
	}

	void TTable::new_entry(Key key, int depth, int eval, HashFlag flags, Move move) {
		Bucket* b = ht + hash_index(key);
		(b->e1.depth <= depth) 
		? b->e1 = TTEntry(key, depth, flags, eval, move) 
		: b->e2 = TTEntry(key, depth, flags, eval, move);
	}

	// probe the pawn table to see if an entry exists
	PTEntry TTable::probe_pawn(Key key) {
		return pt[pawn_hash_index(key)];
	}

} // namespace Clovis
