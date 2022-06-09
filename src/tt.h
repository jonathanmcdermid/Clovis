#pragma once

#include "types.h"
#include <vector>

namespace Clovis {

    enum HashFlag : int {
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
        Key key;
        int depth;
        HashFlag flags;
        int eval;
        Move move;
	};

	class TTable {
    public:
        void setSize(int bytes);
        void clear();
        void newEntry(Key key, int d, int e, HashFlag f, Move m) { ht[key % ht.size()] = TTEntry(key, d, e, f, m); }
        void newEntry(Key key, TTEntry& h) { ht[key % ht.size()] = h; }
        TTEntry getEntry(Key key) const { return ht[key % ht.size()]; }
        Key getKey(Key key) const { return ht[key % ht.size()].key; }
        Move getMove(Key key) const { return ht[key % ht.size()].move; }
        int getDepth(Key key) const { return ht[key % ht.size()].depth; }
        int getEval(Key key) const { return ht[key % ht.size()].eval; }
        int getFlags(Key key) const { return ht[key % ht.size()].flags; }
        TTEntry* probe(Key key, bool& found);
    private:
        std::vector<TTEntry> ht;
	};

} // Clovis