#include "tt.h"

#include <bit>

namespace clovis {

TTable tt; // global transposition table

void TTable::resize(const int mb)
{
    tt_size = std::bit_floor(static_cast<size_t>(mb) * 1024 * 1024 / sizeof(TTBucket));
    ht = std::make_unique<TTBucket[]>(tt_size);
}

void TTable::clear()
{
    ht = std::make_unique<TTBucket[]>(tt_size);
    pt = std::make_unique<PTEntry[]>(pt_size);
}

TTEntry TTable::probe(const Key key)
{
    auto& [e1, e2] = ht[hash_index(key)];

    if (e1.key == key) { return e1; }
    if (e1.depth > 0) { --e1.depth; }

    return e2;
}

void TTable::new_entry(const Key key, const int depth, const int eval, const HashFlag flags, const Move move)
{
    TTBucket& bucket = ht[hash_index(key)];
    bucket[bucket.e1.depth > depth] = TTEntry(key, depth, flags, eval, move);
}

} // namespace clovis
