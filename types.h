#pragma once
#include <ostream>
#include <cassert>
#include <string>
#include <set>
#include "options.h"

using std::string;
using std::set;
using SQ = int;
using U64 = unsigned long long;

const int MB = 1 << 20;

const U64 EMPTY = 0;
const U64 BIT = 1;
constexpr U64 FULL = ~EMPTY;

const int MATE = 900000;
const int INF = 1000000;

const int N = 9; // Board size

enum Color { Black, White };
constexpr inline bool is_black(int code)       { return !(code & 1); }
constexpr inline bool is_white(int code)       { return  (code & 1); }
constexpr inline bool same_color(int a, int b) { return  (a & 1) == (b & 1); }

constexpr inline int make_sq(int x, int y) { return (y + 1) * (N + 2) + x + 1; }
constexpr inline int get_x(int sq)         { return (sq % (N + 2)) - 1; }
constexpr inline int get_y(int sq)         { return (sq / (N + 2)) - 1; }

constexpr int operator "" _sq(const char * s, const size_t size)
{
    return make_sq(s[0] - 'A', s[1] - '1');
}

constexpr int MSB(int x)
{
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return (x >> 1) + 1;
}

#ifdef _DEBUG
#define ASSERT(x, execute)  { if (!(x)) { execute; }; assert(x); }
#else
#define ASSERT(x, execute)  { }
#endif

enum SQ_OUT {};
extern std::ostream & operator << (std::ostream & os, const SQ_OUT & sq);
extern bool correct(SQ sq);
extern void print64(U64 bb);
