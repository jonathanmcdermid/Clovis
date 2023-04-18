#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

#include "types.h"

namespace Clovis {

	constexpr size_t pt_size = 131072;

	struct TTEntry {
		TTEntry() : key(0ULL), depth(0), flags(HASH_NONE), eval(0), move(MOVE_NONE) {}
		TTEntry(Key k, int d, HashFlag f, int e, Move m) : key(k), depth(d), flags(f), eval(e), move(m) {};
		Key key;               // 8 bytes
		uint8_t depth, flags;  // 1 byte x 2
		short eval;            // 2 bytes
		Move move;             // 4 bytes
	};

	struct Bucket {
		TTEntry e1, e2;
	};
	
	struct PTEntry {

		PTEntry() : score(), key(0ULL), pawn_attacks{ 0ULL }, passers{ 0ULL }, 
			potential_pawn_attacks{ 0ULL }, ksq{ SQ_NONE }, weight{ 0 } {}

		void clear() {
			score = Score();
			key = 0ULL;
			for (int i = 0; i < COLOUR_N; ++i) {
				pawn_attacks[i] = 0ULL;
				passers[i] = 0ULL;
				potential_pawn_attacks[i] = 0ULL;
				ksq[i] = SQ_NONE;
				weight[i] = 0;
			}
		}

		Score score;
		Key key;
		Bitboard pawn_attacks[COLOUR_N], passers[COLOUR_N], potential_pawn_attacks[COLOUR_N];
		Square ksq[COLOUR_N];
		short weight[COLOUR_N];
	};

	class TTable {

	public:
		TTable();
		~TTable();
		void resize(size_t mb);
		void clear();

		void new_entry(Key key, int depth, int eval, HashFlag flags, Move move);
		void new_pawn_entry(const PTEntry& pte);
		PTEntry probe_pawn(Key key);
		TTEntry* probe(Key key);

	private:
		int hash_index(Key key) const;
		int pawn_hash_index(Key key) const;
		Bucket* ht;
		PTEntry* pt;
		size_t tt_size;
	};

	inline int TTable::hash_index(Key key) const {
		return key & (tt_size - 1ULL);
	}

	inline int TTable::pawn_hash_index(Key key) const {
		return key & (pt_size - 1ULL);
	}

	// probe the pawn table to see if an entry exists
	inline PTEntry TTable::probe_pawn(Key key) {
		return pt[pawn_hash_index(key)];
	}

	inline void TTable::new_pawn_entry(const PTEntry& pte) { 
		pt[pawn_hash_index(pte.key)] = pte; 
	}

	extern TTable tt;

} // Clovis
