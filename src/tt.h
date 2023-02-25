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
		Bitboard outer_ring;
		Bitboard inner_ring;
	};

	struct TTEntry {
		TTEntry(Key k = 0ULL, int d = 0, HashFlag f = HASH_NONE, int e = 0, Move m = MOVE_NONE) : key(k), depth(d), flags(f), eval(e), move(m) {};
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
		void clear() { *this = {}; }
		Score score;
		Key key = 0ULL;
		Bitboard pawn_attacks[COLOUR_N] = {0ULL};
		Bitboard passers[COLOUR_N] = {0ULL};
		Bitboard potential_pawn_attacks[COLOUR_N] = {0ULL};
		Bitboard attacked_twice[COLOUR_N] = {0ULL};
		Square ksq[COLOUR_N] = {SQ_NONE};
		short weight[COLOUR_N] = {0};
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
