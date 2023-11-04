#pragma once

#include <cstdint>
#include <memory>
#include <mutex>

#include "types.h"

namespace clovis::transposition {

struct TTEntry {
    constexpr TTEntry() = default;
    TTEntry(const Key k, const int d, const HashFlag f, const int e, const Move m)
        : depth(static_cast<uint8_t>(d)),
          flags(static_cast<uint8_t>(f)),
          eval(static_cast<int16_t>(e)),
          move(m),
          key(k) {}

    uint8_t depth{0}; // 1 bytes
    uint8_t flags{0}; // 1 bytes
    int16_t eval{0};  // 2 bytes
    Move move{0};     // 4 bytes
    Key key{0ULL};    // 8 bytes
};

struct TTBucket {
    TTEntry &operator[](const bool index) { return index ? e2 : e1; }

    TTEntry e1, e2;
};

struct PTEntry {
    constexpr PTEntry() = default;

    short weight[COLOUR_N]{0};
    Score score;
    Square ksq[COLOUR_N]{SQ_NONE};
    Key key{0ULL};
    Bitboard pawn_attacks[COLOUR_N]{0ULL}, passers[COLOUR_N]{0ULL}, potential_pawn_attacks[COLOUR_N]{0ULL};
};

static size_t tt_size = 4194304;
constexpr size_t pt_size = 131072;

static std::mutex ht_mutex;
static std::mutex pt_mutex;

static std::unique_ptr<TTBucket[]> ht = std::make_unique<TTBucket[]>(tt_size);
static std::unique_ptr<PTEntry[]> pt = std::make_unique<PTEntry[]>(pt_size);

static inline void resize(const int mb) {
	std::lock_guard<std::mutex> lock(ht_mutex);
    tt_size = std::bit_floor(static_cast<size_t>(mb) * 1024 * 1024 / sizeof(TTBucket));
    ht = std::make_unique<TTBucket[]>(tt_size);
}

static inline void clear() {
    {
		std::lock_guard<std::mutex> lock(ht_mutex);
		ht = std::make_unique<TTBucket[]>(tt_size);
	}
	{
		std::lock_guard<std::mutex> lock(pt_mutex);
		pt = std::make_unique<PTEntry[]>(pt_size);
	}
}

static inline size_t hash_index(const Key key) { return key & (tt_size - 1ULL); }

static inline size_t pawn_hash_index(const Key key) { return key & (pt_size - 1ULL); }

static inline TTEntry probe(const Key key) {
	std::lock_guard<std::mutex> lock(ht_mutex);
    auto &[e1, e2] = ht[hash_index(key)];

    if (e1.key == key) return e1;
    if (e1.depth > 0) --e1.depth;

    return e2;
}

static inline PTEntry probe_pawn(const Key key) { 
	std::lock_guard<std::mutex> lock(pt_mutex);
	return pt[pawn_hash_index(key)]; 
}

static inline void new_entry(const Key key, const int depth, const int eval, const HashFlag flags, const Move move) {
	std::lock_guard<std::mutex> lock(ht_mutex);
    TTBucket &bucket = ht[hash_index(key)];
    bucket[bucket.e1.depth > depth] = TTEntry(key, depth, flags, eval, move);
}

static inline void new_pawn_entry(const PTEntry &pte) { 
	std::lock_guard<std::mutex> lock(pt_mutex);
	pt[pawn_hash_index(pte.key)] = pte; 
}

} // namespace clovis::transposition
