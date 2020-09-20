#include <iostream>
#include <iomanip>
#include <random>
#include "hash.h"

using namespace std;

Hash * H;
U64 hash_wtm;
U64 hash_key[BBI_MAX][2];

class Random
{
    static mt19937_64 gen;

public:
    static void seed(uint64_t new_seed = mt19937_64::default_seed)
    {
        gen.seed(new_seed);
    }

    static uint64_t get()
    {
        static uniform_int_distribution<U64> dist(0, FULL);
        return dist(gen);
    }
};

mt19937_64 Random::gen;

void init_hash(int verbose)
{
    Random::seed(42);
    for (int i = 0; i < BBI_MAX; i++)
        hash_key[i][0] = hash_key[i][1] = EMPTY;

    hash_wtm = Random::get();
    for (int y = 0; y < N; y++)
    {
        for (int x = 0; x < N; x++)
        {
            SQ sq = make_sq(x, y);
            hash_key[sq][0] = Random::get();
            hash_key[sq][1] = Random::get();

            if (verbose)
            cout << SQ_OUT(sq) << " - " << setfill('0')
                 << hex << setw(16) << hash_key[sq][0] << " | "
                 << hex << setw(16) << hash_key[sq][1] << "\n";
        }
    }
    if (verbose) cout << setfill(' ') << dec << "\n";
}

Hash::Hash(int sizeMb)
{
	init(sizeMb);
}

Hash::~Hash()
{
	_delete();
}

void Hash::_delete()
{
	if (table)
	{
		delete[] table;
		table = 0;
	}
}

void Hash::init(int sizeMb)
{
	size = sizeMb * MB / sizeof(HashEntry);
	size = 2 * MSB(size - 1);
	table = new HashEntry[size];
	clear();
}

void Hash::clear()
{
	for (HashEntry * the = table; the - table < size; the++)
	{
		the->key = EMPTY;
		the->move = 0;
		the->type = 0;
		the->depth = 0;
		the->val = 0;
	}
}

HashEntry * Hash::get(U64 key)
{
	HashEntry * the = table + (key & (size - 1));
    if (the->key == key)
    {
        //S->hash_read++;
        return the;
    }
	return NULL;
}

HashEntry * Hash::get(U64 key, int & alpha, int & beta, int depth, int ply)
{
	HashEntry * the = table + (key & (size - 1));
	if (the->key == key)
	{
		//S->hash_read++;

        if (the->depth >= depth)
        {
            int val = the->val;
            if      (val >  MATE && val <=  INF) val -= ply;
            else if (val < -MATE && val >= -INF) val += ply;

            // Exact score
            if (the->type == HashExact) alpha = beta = val;
            else if (the->type == HashAlpha && val <= alpha) beta = alpha;
            else if (the->type == HashBeta  && val >= beta) alpha = beta;
        }
		return the;
	}
	return NULL;
}

void Hash::set(U64 key, SQ move, int depth, int ply, int val, int type)
{
	//S->hash_write++;
    //short age = S->movecnt;
	HashEntry * the = table + (key & (size - 1));
	
	//if (/*the->age == age && */the->depth > depth) return;

    if      (val >  MATE && val <=  INF) val += ply;
	else if (val < -MATE && val >= -INF) val -= ply;

	the->key = key;
	the->move = move;
	the->type = type;
	the->depth = depth;
	the->val = val;
    //the->age = age;
}
