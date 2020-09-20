#pragma once
#include "types.h"

const U64 debruijn = 0x03f79d71b4cb0a89ULL;
const int btscn64[64] =
{
     0,  1, 48,  2, 57, 49, 28,  3,
    61, 58, 50, 42, 38, 29, 17,  4,
    62, 55, 59, 36, 53, 51, 43, 22,
    45, 39, 33, 30, 24, 18, 12,  5,
    63, 47, 56, 27, 60, 41, 37, 16,
    54, 35, 52, 21, 44, 32, 23, 11,
    46, 26, 40, 15, 34, 20, 31, 10,
    25, 14, 19,  9, 13,  8,  7,  6
};

constexpr int BBI_MAX = make_sq(N, N) + 1;

constexpr U64 bbi_m0(U64 mask, int j)
{
    unsigned i = make_sq(j % 9, j / 9);
    return j >= 0
        ? bbi_m0(mask | (BIT << i), j - 1)
        : mask;
}

constexpr U64 bbi_m1(U64 mask, int j)
{
    unsigned i = (make_sq(j % 9, j / 9) - 64);
    return j >= 0
        ? bbi_m1(mask | (BIT << (i >= 0 ? i : 64)), j - 1)
        : mask;
}

constexpr U64 bbi_mask0 = bbi_m0(EMPTY, N * N - 1);
constexpr U64 bbi_mask1 = bbi_m1(EMPTY, N * N - 1);

struct SQ_Info
{
    int x, y;
    SQ_Info(int x = -1, int y = -1) : x(x), y(y) {}
};

struct BB
{
    U64 part[2];

    BB(U64 p1 = U64(), U64 p2 = U64());
    void clear();
    BB & set(SQ sq);
    BB & reset(SQ sq);
    bool test(SQ sq) const;
    BB operator & (const BB & bb) const;
    BB operator | (const BB & bb) const;
    BB operator ^ (const BB & bb) const;
    BB & operator &= (const BB & bb);
    BB & operator |= (const BB & bb);
    BB & operator ^= (const BB & bb);
    BB operator ~ ();
    bool operator == (const BB & bb) const;
    bool operator != (const BB & bb) const;
    BB lshift() const;
    BB rshift() const;
    BB ushift() const;
    BB dshift() const;
    BB lsb() const;
    BB get_group() const;
    BB expand() const;
    int popcnt() const;
    int bitscan() const;
    operator bool() const;
    void print() const;
};

extern SQ_Info sq_info[BBI_MAX];
extern unsigned int lut[65536];

extern int popcnt(U64 x);
extern int bitscan(U64 x);
extern void init_bitboards();
extern void test_bitboards();
