#include <iostream>
#include "types.h"

using namespace std;

std::ostream & operator << (std::ostream & os, const SQ_OUT & sq)
{
    int x = get_x(sq);
    int y = get_y(sq);

    if (x < 0 || x >= N || y < 0 || y >= N)
        os << "??";
    else
        os << (char)('A' + x) << (char)('1' + y);
    return os;
}

bool correct(SQ sq)
{
    int x = get_x(sq);
    int y = get_y(sq);

    return x >= 0 && x < N && y >= 0 && y < N;
}

void print64(U64 bb)
{
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            int sq = 8 * y + x;
            if (bb & (BIT << sq)) cout << "x";
            else                  cout << ".";
        }
        cout << "\n";
    }
    cout << "\n";
}