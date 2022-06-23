#include "tt.h"

namespace Clovis {

    // empty transposition table
    void TTable::clear()
    {
        for (int i = 0; i < n_entries; ++i)
            ht[i] = TTEntry();
        for (int i = 0; i < n_p_entries; ++i)
            pt[i] = PTEntry();
    }

    // set the size of the table
    void TTable::set_size(int bytes) 
    {
        unsigned i;
        for (i = 31; i > 0; --i) 
            if (bytes & 1 << i)
                break;
        n_entries = (1 << (i - 5));
        n_p_entries = (1 << (i - 6));
        ht.resize(n_entries);
        pt.resize(n_p_entries);
    }

    // probe the table to see if an entry exists
    TTEntry* TTable::probe(Key key, bool& found) 
    {
        found = (ht[key % n_entries].key == key);
        return &ht[key % n_entries];
    }

    // probe the pawn table to see if an entry exists
    PTEntry* TTable::probe_pawn(Key key, bool& found)
    {
        found = (pt[key % n_p_entries].key == key);
        return &pt[key % n_p_entries];
    }

} // namespace Clovis