#pragma once

#include <cstdint>
#include <memory>

#include "types.h"

namespace clovis {

struct TTEntry
{
    constexpr TTEntry() = default;
    TTEntry(const Key k, const int d, const HashFlag f, const int e, const Move m)
        : depth(static_cast<uint8_t>(d)), flags(static_cast<uint8_t>(f)), eval(static_cast<int16_t>(e)), move(m), key(k)
    {
    }

    uint8_t depth{0}; // 1 bytes
    uint8_t flags{0}; // 1 bytes
    int16_t eval{0};  // 2 bytes
    Move move{0};     // 4 bytes
    Key key{0ULL};    // 8 bytes
};

struct TTBucket
{
    TTEntry& operator[](const bool index) { return index ? e2 : e1; }

    TTEntry e1;
    TTEntry e2;
};

struct PTEntry
{
    constexpr PTEntry() = default;

    std::array<short, COLOUR_N> weight{0};
    Score score;
    std::array<Square, COLOUR_N> ksq{SQ_NONE};
    Key key{0ULL};
    std::array<Bitboard, COLOUR_N> pawn_attacks{0ULL};
    std::array<Bitboard, COLOUR_N> passers{0ULL};
    std::array<Bitboard, COLOUR_N> potential_pawn_attacks{0ULL};
};

class TTable
{
  public:
    TTable() : ht(std::make_unique<TTBucket[]>(tt_size)), pt(std::make_unique<PTEntry[]>(pt_size)) {}
    void resize(int mb);
    void clear();

    void new_entry(Key key, int depth, int eval, HashFlag flags, Move move);
    void new_pawn_entry(const PTEntry& pte);
    [[nodiscard]] PTEntry probe_pawn(Key key) const;
    TTEntry probe(Key key);

  private:
    static size_t hash_index(Key key);
    static size_t pawn_hash_index(Key key);
    static constexpr size_t pt_size = 131072;
    static inline size_t tt_size = 4194304;
    std::unique_ptr<TTBucket[]> ht;
    std::unique_ptr<PTEntry[]> pt;
};

inline size_t TTable::hash_index(const Key key) { return key & (tt_size - 1ULL); }

inline size_t TTable::pawn_hash_index(const Key key) { return key & (pt_size - 1ULL); }

inline PTEntry TTable::probe_pawn(const Key key) const { return pt[pawn_hash_index(key)]; }

inline void TTable::new_pawn_entry(const PTEntry& pte) { pt[pawn_hash_index(pte.key)] = pte; }

extern TTable tt;

} // namespace clovis
