#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

#include "types.h"

namespace Clovis {

    constexpr size_t pt_size = 131072;

    struct Score {
    public:
        Score() : mg(0), eg(0) {}
        Score(int mg, int eg) : mg(mg), eg(eg) {}
        void operator+=(const Score& rhs) {
            this->mg += rhs.mg;
            this->eg += rhs.eg;
        }
        void operator-=(const Score& rhs) {
            this->mg -= rhs.mg;
            this->eg -= rhs.eg;
        }
        bool operator==(const Score& rhs) {
            return this->mg == rhs.mg && this->eg == rhs.eg;
        }
        short mg;
        short eg;
    };

	inline Score operator-(Score s)             { return Score(-s.mg, -s.eg); }
    inline Score operator+(Score s1, Score s2)  { return Score(s1.mg + s2.mg, s1.eg + s2.eg); }
    inline Score operator+(Score s1, int i)     { return Score(s1.mg + i, s1.eg + i); }
    inline Score operator-(Score s1, Score s2)  { return Score(s1.mg - s2.mg, s1.eg - s2.eg); }
    inline Score operator-(Score s1, int i)     { return Score(s1.mg - i, s1.eg - i); }
    inline Score operator*(Score s1, int i)     { return Score(s1.mg * i, s1.eg * i); }
    inline Score operator*(Score s1, Score s2)  { return Score(s1.mg * s2.mg, s1.eg * s2.eg); }
    inline Score operator/(Score s1, int i)     { return Score(s1.mg / i, s1.eg / i); }

    struct KingZone {
        KingZone() : outer_ring(0ULL), inner_ring(0ULL) { ; }
        KingZone(Bitboard outer_ring, Bitboard inner_ring) : outer_ring(outer_ring), inner_ring(inner_ring) { ; }
        void operator=(const KingZone& rhs) {
            this->outer_ring = rhs.outer_ring;
            this->inner_ring = rhs.inner_ring;
        }
        Bitboard outer_ring;
        Bitboard inner_ring;
    };

	struct TTEntry {
        TTEntry(Key key = 0ULL, int depth = 0, int eval = 0, HashFlag flags = HASH_NONE, Move move = MOVE_NONE) : key(key), depth(depth), flags(flags), eval(eval), move(move) {};
        void operator=(const TTEntry& rhs){
            key = rhs.key;
            depth = rhs.depth;
            eval = rhs.eval;
            flags = rhs.flags;
            move = rhs.move;
        };
        Key key;        // 8 bytes
        uint8_t depth;  // 1 byte
        uint8_t flags;  // 1 byte
        short eval;     // 2 bytes
        Move move;      // 4 bytes
	};

    struct Bucket {
    public:
        TTEntry e1;
        TTEntry e2;
    };

    struct PTEntry {
        PTEntry() { clear(); }
        void clear() { memset(this, 0, sizeof(PTEntry)); }
        void operator=(const PTEntry& rhs) {
            this->key = rhs.key;
            this->score = rhs.score;
            this->weight[WHITE] = rhs.weight[WHITE];
            this->weight[BLACK] = rhs.weight[BLACK];
            this->zone[WHITE] = rhs.zone[WHITE];
            this->zone[BLACK] = rhs.zone[BLACK];
            this->attacks[WHITE] = rhs.attacks[WHITE];
            this->attacks[BLACK] = rhs.attacks[BLACK];
            this->n_att[WHITE] = rhs.n_att[WHITE];
            this->n_att[BLACK] = rhs.n_att[BLACK];
        }
        Key key;
        Score score;
        Score weight[COLOUR_N];
        KingZone zone[COLOUR_N];
        Bitboard attacks[COLOUR_N];
        int n_att[COLOUR_N];
    };

	class TTable {
    public:
        TTable();
        void resize(size_t mb);
        void clear();
        void new_entry(Key key, int depth, int eval, HashFlag flags, Move move);
        TTEntry* probe(Key key);
        void new_pawn_entry(PTEntry pte) { pt[pawn_hash_index(pte.key)] = pte; }
        PTEntry probe_pawn(Key key);
        ~TTable();
    private:
        int hash_index(Key key) const;
        int pawn_hash_index(Key key) const;
        Bucket* ht = nullptr;
        PTEntry* pt = nullptr;
        size_t tt_size;
    };

    inline int TTable::hash_index(Key key) const {
        return key & (tt_size - 1ULL);
    }

    inline int TTable::pawn_hash_index(Key key) const {
        return key & (pt_size - 1ULL);
    }

    extern TTable tt;

} // Clovis
