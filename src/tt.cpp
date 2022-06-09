#include "tt.h"

namespace Clovis {

    // empty transposition table
    void TTable::clear()
    {
        for (unsigned i = 0; i < ht.size(); ++i)
            ht[i] = TTEntry();
    }

    // set the size of the table
    void TTable::setSize(int bytes) 
    {
        unsigned i;
        for (i = 31; i > 0; --i) {
            if (bytes & 1 << i)
                break;
        }
        ht.resize(1 << (i - 5));
    }

    // probe the table to see if an entry exists
    TTEntry* TTable::probe(U64 key, bool& found) 
    {
        found = (ht[key % ht.size()].key == key);
        return &ht[key % ht.size()];
    }

} // namespace Clovis