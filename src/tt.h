#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

#include "types.h"

namespace Clovis {

	constexpr size_t pt_size = 131072;

	struct KingZone {
		KingZone() : outer_ring(0ULL), inner_ring(0ULL) { ; }
		constexpr KingZone(Bitboard outer, Bitboard inner) : outer_ring(outer), inner_ring(inner) { ; }
		void operator=(const KingZone& rhs) {
			this->outer_ring = rhs.outer_ring;
			this->inner_ring = rhs.inner_ring;
		}
		Bitboard outer_ring;
		Bitboard inner_ring;
	};

	struct TTEntry {
		TTEntry(Key k = 0ULL, int d = 0, HashFlag f = HASH_NONE, int e = 0, Move m = MOVE_NONE) : key(k), depth(d), flags(f), eval(e), move(m) {};
		void operator=(const TTEntry& rhs){
			key = rhs.key;
			depth = rhs.depth;
			eval = rhs.eval;
			flags = rhs.flags;
			move = rhs.move;
		};
		Key key;        // 8 bytes
		uint8_t depth;  // 1 byte
		uint8_t flags;  // 1 byte
		short eval;     // 2 bytes
		Move move;      // 4 bytes
	};

	struct Bucket {
		TTEntry e1;
		TTEntry e2;
	};

	struct PTEntry {
		PTEntry() { clear(); }
		void clear() { 
			key = 0ULL;
			score = Score();
			weight[WHITE] = 0;
			weight[BLACK] = 0;
			attacks[WHITE] = 0ULL;
			attacks[BLACK] = 0ULL;
			n_att[WHITE] = 0;
			n_att[BLACK] = 0;
			ksq[WHITE] = SQ_NONE;
			ksq[BLACK] = SQ_NONE;
		} 
		Key key;
		Score score;
		short weight[COLOUR_N];
		Bitboard attacks[COLOUR_N];
		int n_att[COLOUR_N];
		Square ksq[COLOUR_N];
	};

	class TTable {
	public:
		TTable();
		void resize(size_t mb);
		void clear();
		void new_entry(Key key, int depth, int eval, HashFlag flags, Move move);
		TTEntry* probe(Key key);
		void new_pawn_entry(PTEntry pte) { pt[pawn_hash_index(pte.key)] = pte; }
		PTEntry probe_pawn(Key key);
		~TTable();
	private:
		int hash_index(Key key) const;
		int pawn_hash_index(Key key) const;
		Bucket* ht = nullptr;
		PTEntry* pt = nullptr;
		size_t tt_size;
	};

	inline int TTable::hash_index(Key key) const {
		return key & (tt_size - 1ULL);
	}

	inline int TTable::pawn_hash_index(Key key) const {
		return key & (pt_size - 1ULL);
	}

	extern TTable tt;

} // Clovis
