#include <iostream>
#include <string>
#include "hash.h"
#include "board.h"
#include "search.h"

using namespace std;

int main()
{
    cout << "Atari Go v0.1 by Nick Kurgin (c) 2020\n\n";
    init_bitboards();
    init_hash();
    test_bitboards();

    H = new Hash;
    S = new Search;
    B = new Board;
    B->print();

    bool has_dead = false;
    bool first_run = true;

    while(true)
    {
        S->think();
        B->place_stone(S->best);
        has_dead = B->remove_dead_after_move();
        B->print();

        if (has_dead)
        {
            cout << "--- I won! ---\n";
            break;
        }

        cout << "Your move";
        if (first_run) cout << " (for example, c4)";
        cout << ":\n";

        SQ sq = -1;
        while (!B->is_empty(sq))
        {
            cout << "> ";
            string s;
            cin >> s;

            if (s[1] < '1' || s[1] > '9') continue;
            if (s[0] >= 'A' && s[0] <= 'I')
            {
                sq = make_sq(s[0] - 'A', s[1] - '1');
            }
            else if (s[0] >= 'a' && s[0] <= 'i')
            {
                sq = make_sq(s[0] - 'a', s[1] - '1');
            }
        }
        first_run = false;
        B->place_stone(sq);
        has_dead = B->remove_dead_after_move();
        B->print();

        if (has_dead)
        {
            cout << "--- You won (( ---\n";
            break;
        }
    }

    system("pause");
    delete B;
    delete S;
    delete H;

    return 0;
}
