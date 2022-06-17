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

    struct PTEntry {
        PTEntry(Key k = 0ULL, int e = 0) {
            key = k;
            eval = e;
        }
        void operator=(const PTEntry& rhs) {
            key = rhs.key;
            eval = rhs.eval;
        }
        Key key;
        int eval;
    };

	class TTable {
    public:
        void set_size(int bytes);
        void clear();
        void new_entry(Key key, int d, int e, HashFlag f, Move m) { ht[key % ht.size()] = TTEntry(key, d, e, f, m); }
        void new_entry(Key key, TTEntry& h) { ht[key % ht.size()] = h; }
        TTEntry get_entry(Key key) const { return ht[key % ht.size()]; }
        Key get_key(Key key) const { return ht[key % ht.size()].key; }
        Move get_move(Key key) const { return ht[key % ht.size()].move; }
        int get_depth(Key key) const { return ht[key % ht.size()].depth; }
        int get_eval(Key key) const { return ht[key % ht.size()].eval; }
        int get_flags(Key key) const { return ht[key % ht.size()].flags; }
        TTEntry* probe(Key key, bool& found);
        void new_pawn_entry(Key key, int e) { pt[key % pt.size()] = PTEntry(key, e); }
        int get_pawn_eval(Key key)    const { return pt[key % pt.size()].eval; }
        Key get_pawn_key(Key key) const { return pt[key % pt.size()].key; }
    private:
        std::vector<TTEntry> ht;
        std::vector<PTEntry> pt;
	};

} // Clovis