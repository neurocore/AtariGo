#include <algorithm>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <windows.h>
#include "board.h"
#include "hash.h"

using namespace std;

Board * B;

/*
void BoardMailbox::place_stone(SQ sq)
{
    Point & point = points[sq];
    assert(point.code != Point::Bad);

    // 1. Place stone on board
    point.code = Point::BlackStone + btm;

    // 2. Substract point from group liberties
    set<SQ> stones{sq};
    set<SQ> liberties;
    int alive = 0;
    
    for (auto s : get_adjacents(sq))
    {
        liberties.insert(s);
        const Point & p = points[s];
        Group & g = groups[p.group_id];

        auto it = g.liberties.find(sq);
        if (it != g.liberties.end())
            g.liberties.erase(it);
        
        if ((p.code != Point::Empty) && same_color(btm, g.color))
        {
            // 3. Connect own groups
            stones.insert(g.stones.begin(), g.stones.end());
            liberties.insert(g.liberties.begin(), g.liberties.end());
            alive |= g.alive;
        }
    }
    
    // 3.1. Save group union
    int gid = groups.rbegin()->first + 1;
    groups[gid] = Group(btm & 1, stones, liberties, alive);
    point.group_id = gid;

    // 4. Recalculate influences
    int px = get_x(sq);
    int py = get_y(sq);

    for (int x = 0; x < N; x++)
    {
        for (int y = 0; y < N; y++)
        {
            SQ s = make_sq(x, y);
            int dx = x - px;
            int dy = y - py;
            float b = powf(2, -sqrtf(dx * dx + dy * dy));
            float & a = points[s].infl[btm & 1];

            a += b - a * b;
        }
    }
}
*/

Board::Board()
{
    this->init();
    this->set_start();
}

void Board::init()
{
    hash = EMPTY;
    stones[0].clear();
    stones[1].clear();
}

void Board::set_start()
{
    wtm = 0;
    place_stone("E4"_sq);
    place_stone("D4"_sq);
    place_stone("D5"_sq);
    place_stone("E5"_sq);

    cout << "eval = " << eval() << "\n";
}

void Board::print() const
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int colors[] = { 0x05, 0x4C, 0x09, 0x0E };

    cout << hex;
    for (int y = N - 1; y >= 0; y--)
    {
        cout << setfill(' ') << setw(3) << (y + 1) << " " << setfill('0');
        for (int x = 0; x < N; x++)
        {
            SQ sq = make_sq(x, y);
            bool b = stones[Black].test(sq);
            bool w = stones[White].test(sq);
            int code = ((b ^ w) << 1) | (!b & w);

            if (code > Point::Bad)
            {
                SetConsoleTextAttribute(hConsole, colors[code]);
                cout << "xx";
            }
            else
            {
                SetConsoleTextAttribute(hConsole, colors[code]);
                cout << (char)250 << (char)250;
            }

            SetConsoleTextAttribute(hConsole, 7);
            cout << " ";
        }
        cout << "\n";
    }
    cout << "     ";
    for (int x = 0; x < N; x++)
        cout << (char)('A' + x) << "  ";
    cout << dec << setfill(' ') << "\n\n";
}

void Board::place_stone(SQ sq)
{
    stones[wtm].set(sq);
    hash ^= hash_key[sq][wtm] ^ hash_wtm;
    wtm ^= 1;
}

void Board::remove_stone(SQ sq)
{
    wtm ^= 1;
    hash ^= hash_key[sq][wtm] ^ hash_wtm;
    stones[wtm].reset(sq);
}

bool Board::remove_dead_after_move()
{
    bool has_dead = false;
    BB empty = ~(stones[0] | stones[1]);
    BB bb = stones[B->wtm];

    while (BB group = bb.get_group())
    {
        bb &= ~group;

        BB libs = group.expand() & empty;
        if (!libs.popcnt())
        {
            stones[B->wtm] &= ~group;
            has_dead = true;
        }
    }
    return has_dead;
}

bool Board::is_empty(SQ sq) const
{
    int x = get_x(sq);
    int y = get_y(sq);
    if (x < 0 || x > N - 1) return false;
    if (y < 0 || y > N - 1) return false;
    return !stones[0].test(sq) && !stones[1].test(sq);
}

int Board::eval(int ply) const
{
    int val[2] = {0, 0};
    BB empty = ~(stones[0] | stones[1]);

    for (int i = 0; i < 2; i++)
    {
        BB bb = stones[i];

        while (BB group = bb.get_group())
        {
            bb &= ~group;

            BB libs = group.expand() & empty;
            int n = libs.popcnt();
            if (!n)
            {
                val[i] = -INF + ply;
                break;
            }
            val[i] -= static_cast<int>(10000 * exp(-n / 2.0));
        }
    }
    ASSERT(abs(val[0] - val[1]) < 2 * INF, {});
    if (val[0] < -MATE) val[1] = 0;
    if (val[1] < -MATE) val[0] = 0;

    return wtm ? val[1] - val[0] : val[0] - val[1];
}
