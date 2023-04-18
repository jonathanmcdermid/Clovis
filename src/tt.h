#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

#include "types.h"

namespace Clovis {

	constexpr size_t pt_size = 131072;

	struct KingZone {
		constexpr KingZone() : outer_ring(0ULL), inner_ring(0ULL) {}
		constexpr KingZone(Bitboard outer, Bitboard inner) : outer_ring(outer), inner_ring(inner) {}
		Bitboard outer_ring, inner_ring;
	};

	struct TTEntry {
		constexpr TTEntry() : key(0ULL), depth(0), flags(HASH_NONE), eval(0), move(MOVE_NONE) {}
		constexpr TTEntry(Key k, int d, HashFlag f, int e, Move m) : key(k), depth(d), flags(f), eval(e), move(m) {};
		Key key;               // 8 bytes
		uint8_t depth, flags;  // 1 byte x 2
		short eval;            // 2 bytes
		Move move;             // 4 bytes
	};

	struct Bucket {
		TTEntry e1, e2;
	};
	
	struct PTEntry {

		constexpr PTEntry() : score(), key(0ULL), pawn_attacks{ 0ULL }, passers{ 0ULL }, 
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

		constexpr void new_entry(Key key, int depth, int eval, HashFlag flags, Move move);
		constexpr void new_pawn_entry(const PTEntry& pte);
		constexpr PTEntry probe_pawn(Key key);
		constexpr TTEntry* probe(Key key);

	private:
		constexpr int hash_index(Key key) const;
		constexpr int pawn_hash_index(Key key) const;
		Bucket* ht = nullptr;
		PTEntry* pt = nullptr;
		size_t tt_size;
	};

	constexpr int TTable::hash_index(Key key) const {
		return key & (tt_size - 1ULL);
	}

	constexpr int TTable::pawn_hash_index(Key key) const {
		return key & (pt_size - 1ULL);
	}

	// probe the table to see if an entry exists
	constexpr TTEntry* TTable::probe(Key key) {

		Bucket* b = ht + hash_index(key);

		if (b->e1.key == key)
			return &b->e1;
		if (b->e1.depth > 0)
			--b->e1.depth;

		return (b->e2.key == key) ? &b->e2 : nullptr;
	}

	constexpr void TTable::new_entry(Key key, int depth, int eval, HashFlag flags, Move move) {
		Bucket* b = ht + hash_index(key);
		(b->e1.depth <= depth)
			? b->e1 = TTEntry(key, depth, flags, eval, move)
			: b->e2 = TTEntry(key, depth, flags, eval, move);
	}

	// probe the pawn table to see if an entry exists
	constexpr PTEntry TTable::probe_pawn(Key key) {
		return pt[pawn_hash_index(key)];
	}

	constexpr void TTable::new_pawn_entry(const PTEntry& pte) { 
		pt[pawn_hash_index(pte.key)] = pte; 
	}

	extern TTable tt;

} // Clovis
