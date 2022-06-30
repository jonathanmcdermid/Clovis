#include "tt.h"

namespace Clovis {

    TTable::TTable()
    {
        ht = new Bucket[tt_size];
        pt = new PTEntry[pt_size];
    }

    // empty transposition table
    void TTable::clear()
    {
        std::memset(static_cast<void*>(ht), 0, tt_size * sizeof(Bucket));
        std::memset(static_cast<void*>(pt), 0, pt_size * sizeof(PTEntry));
    }

    // probe the table to see if an entry exists
    TTEntry* TTable::probe(Key key, bool& found) 
    {
        Bucket* b = ht + hash_index(key);
        found = (b->e1.key == key);
        if (found)
            return &b->e1;
        if(b->e1.depth > 0)
            --b->e1.depth;
        found = (b->e2.key == key);
        return &b->e2;
    }

    void TTable::new_entry(Key key, int d, int e, HashFlag f, Move m)
    {
        Bucket* b = ht + hash_index(key);
        if(b->e1.depth <= d)
            b->e1 = TTEntry(key, d, e, f, m);
        else
            b->e2 = TTEntry(key, d, e, f, m);
    }

    // probe the pawn table to see if an entry exists
    PTEntry* TTable::probe_pawn(Key key, bool& found)
    {
        PTEntry* p = pt + pawn_hash_index(key);
        found = (p->key == key);
        return p;
    }

} // namespace Clovis