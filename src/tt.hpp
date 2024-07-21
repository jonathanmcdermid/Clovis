#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "common.hpp"
#include "position.hpp"

namespace clovis {

enum class HashFlag : uint8_t
{
    NONE,
    ALPHA,
    BETA,
    EXACT,
};

struct TTEntry
{
    constexpr TTEntry() = default;
    TTEntry(const Key k, const int d, const HashFlag f, const int e, const Move m)
        : key(k), move(m), eval(static_cast<int16_t>(e)), depth(static_cast<uint8_t>(d)), flags(f)
    {
    }

    Key key{0ULL};                  // 8 bytes
    Move move{0};                   // 4 bytes
    int16_t eval{0};                // 2 bytes
    uint8_t depth{0};               // 1 byte
    HashFlag flags{HashFlag::NONE}; // 1 byte
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

    Key key{0ULL};                                        // 8 bytes
    std::array<Bitboard, 2> pawn_attacks{0ULL};           // 16 bytes (2 x 8 bytes)
    std::array<Bitboard, 2> passers{0ULL};                // 16 bytes (2 x 8 bytes)
    std::array<Bitboard, 2> potential_pawn_attacks{0ULL}; // 16 bytes (2 x 8 bytes)
    std::array<Square, 2> ksq{SQ_NONE};                   // 8 bytes (2 x 4 bytes)
    std::array<short, 2> weight{0};                       // 4 bytes (2 x 2 bytes)
    Score score;                                          // 4 bytes
};

class TranspositionTable
{
  public:
    TranspositionTable() : ht(std::make_unique<std::vector<TTBucket>>(table_size)), pt(std::make_unique<std::vector<PTEntry>>(PAWN_TABLE_SIZE)) {}
    void resize(int mb);
    void clear();

    void new_entry(Key key, int depth, int eval, HashFlag flags, Move move);
    void new_pawn_entry(const PTEntry& pte);
    [[nodiscard]] PTEntry probe_pawn(Key key) const;
    TTEntry probe(Key key);

  private:
    static size_t hash_index(Key key);
    static size_t pawn_hash_index(Key key);
    static constexpr size_t PAWN_TABLE_SIZE = 131072;
    static inline size_t table_size = 4194304;
    std::unique_ptr<std::vector<TTBucket>> ht;
    std::unique_ptr<std::vector<PTEntry>> pt;
};

inline size_t TranspositionTable::hash_index(const Key key) { return key & (table_size - 1ULL); }

inline size_t TranspositionTable::pawn_hash_index(const Key key) { return key & (PAWN_TABLE_SIZE - 1ULL); }

inline PTEntry TranspositionTable::probe_pawn(const Key key) const { return pt->at(pawn_hash_index(key)); }

inline void TranspositionTable::new_pawn_entry(const PTEntry& pte) { (*pt)[pawn_hash_index(pte.key)] = pte; }

extern TranspositionTable tt;

} // namespace clovis
