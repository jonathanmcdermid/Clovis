#include "tt.h"

namespace Clovis {

    // empty transposition table
    void TTable::clear()
    {
        for (unsigned i = 0; i < ht.size(); ++i)
            ht[i] = TTEntry();
        for (unsigned i = 0; i < pt.size(); ++i)
            pt[i] = PTEntry();
    }

    // set the size of the table
    void TTable::set_size(int bytes) 
    {
        unsigned i;
        for (i = 31; i > 0; --i) {
            if (bytes & 1 << i)
                break;
        }
        ht.resize(1 << (i - 5));
        pt.resize(1 << (i - 6));
    }

    // probe the table to see if an entry exists
    TTEntry* TTable::probe(Key key, bool& found) 
    {
        found = (ht[key % ht.size()].key == key);
        return &ht[key % ht.size()];
    }

    // probe the pawn table to see if an entry exists
    PTEntry* TTable::probe_pawn(Key key, bool& found)
    {
        found = (pt[key % pt.size()].key == key);
        return &pt[key % pt.size()];
    }

} // namespace Clovis