#include "tt.h"

#include <bit>

namespace clovis {

TranspositionTable tt; // global transposition table

void TranspositionTable::resize(const int mb)
{
    table_size = std::bit_floor(static_cast<size_t>(mb) * 1024 * 1024 / sizeof(TTBucket));
    ht = std::make_unique<std::vector<TTBucket>>(table_size);
}

void TranspositionTable::clear()
{
    ht = std::make_unique<std::vector<TTBucket>>(table_size);
    pt = std::make_unique<std::vector<PTEntry>>(PAWN_TABLE_SIZE);
}

TTEntry TranspositionTable::probe(const Key key)
{
    auto& [e1, e2] = (*ht)[hash_index(key)];

    if (e1.key == key) { return e1; }
    if (e1.depth > 0) { --e1.depth; }

    return e2;
}

void TranspositionTable::new_entry(const Key key, const int depth, const int eval, const HashFlag flags, const Move move)
{
    TTBucket& bucket = (*ht)[hash_index(key)];
    bucket[bucket.e1.depth > depth] = TTEntry(key, depth, flags, eval, move);
}

} // namespace clovis
