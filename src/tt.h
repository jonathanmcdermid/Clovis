#pragma once

#include <cstdint>
#include <vector>

#include "evaluate.h"
#include "types.h"

namespace Clovis {

    constexpr size_t tt_size = 2097152;
    constexpr size_t n_p_entries = 2097152;

    enum HashFlag : uint8_t {
        HASH_NONE,
        HASH_ALPHA,
        HASH_BETA,
        HASH_EXACT,
    };

	struct TTEntry {
        TTEntry(Key k = 0ULL, int d = 0, int e = 0, HashFlag f = HASH_NONE, Move m = MOVE_NONE) : key(k), depth(d), eval(e), flags(f), move(m) {};
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
    public:
        TTEntry e1;
        TTEntry e2;
    };

    struct PTEntry {
        PTEntry(Key k = 0ULL, Score s = Score(0,0)) {
            key = k;
            eval = s;
        }
        void operator=(const PTEntry& rhs) {
            key = rhs.key;
            eval = rhs.eval;
        }
        Key key;
        Score eval;
    };

	class TTable {
    public:
        TTable();
        void clear();
        void new_entry(Key key, int d, int e, HashFlag f, Move m);
        TTEntry* probe(Key key, bool& found);
        void new_pawn_entry(Key key, Score s) { pt[pawn_hash_index(key)] = PTEntry(key, s); }
        PTEntry* probe_pawn(Key key, bool& found);
    private:
        int hash_index(Key key) const;
        int pawn_hash_index(Key key) const;
        Bucket* ht;
        PTEntry* pt;
    };

    inline int TTable::hash_index(Key key) const {
        return key & (tt_size - 1ULL);
    }

    inline int TTable::pawn_hash_index(Key key) const {
        return key & (n_p_entries - 1ULL);
    }

} // Clovis