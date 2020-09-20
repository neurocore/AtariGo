#pragma once
#include <set>
#include <map>
#include <vector>
#include "bitboard.h"

using std::set;
using std::map;
using std::vector;

struct Point
{
    enum Code { Empty, Bad, BlackStone, WhiteStone };
};

struct Group
{
    int color;
    int alive;
    set<SQ> stones;
    set<SQ> liberties;

    Group(int color = Black, set<SQ> stones = set<SQ>(),
          set<SQ> liberties = set<SQ>(), int alive = 0)
        : color(color), stones(stones), liberties(liberties), alive(alive)
    {}
};

struct Board
{
    int wtm;
    BB stones[2];
    U64 hash;

    Board();
    void init();
    void set_start();
    void print() const;

    bool is_empty(SQ sq) const;
    void place_stone(SQ sq);
    void remove_stone(SQ sq);
    bool remove_dead_after_move();
    int eval(int ply = 0) const;
};

extern Board * B;
