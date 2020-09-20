#pragma once
#include "bitboard.h"

enum HashType
{
    HashAlpha,
    HashExact,
    HashBeta
};

struct HashEntry // 16
{
    U64 key;     // 8
    short move;  // 2
    char type;   // 1
    char depth;  // 1
    short age;   // 2
    short val;   // 2
};

struct Hash
{
    int size;
    HashEntry * table;

    Hash(int sizeMb = Options::HashSize);
    ~Hash();
    void _delete();

    void init(int sizeMb);
    void clear();
    HashEntry * get(U64 key);
    HashEntry * get(U64 key, int & alpha, int & beta, int depth, int ply);
    void set(U64 key, SQ move, int depth, int ply, int val, int type);
};

extern Hash * H;
extern U64 hash_wtm;
extern U64 hash_key[BBI_MAX][2];
extern void init_hash(int verbose = 0);
