#pragma once
#include "types.h"
#include "timer.h"
#include "bitboard.h"

enum class Status { Waiting, Playing, Learning };
enum Stage { StageHash, StageStickers, StageGood, StageOthers };

struct Node
{
	bool can_null;
	SQ curr, best;

	SQ hashMove;
	SQ killers[2];

    BB looked;
	BB moves;
	int stage;

	Node() : can_null(false) {}

    void generate(SQ hash_move);
    SQ get_next_move();
};

struct Search
{
    Status status = Status::Waiting;
    bool infinite = false;
    int think_time = 60000;
    int input_time = 1000;
    U64 nodes = EMPTY;
    int search_depth;
    Timer timer;

    int color = White;
    SQ best;

    U64 hash_read = EMPTY;
    U64 hash_write = EMPTY;

    Node node[Options::MaxPly];
    Node * state;

    Search() {}

    void think();
    int pvs(int alpha, int beta, int depth);
    int qs(int alpha, int beta, int qply = 0);

    void make(SQ move);
    void unmake(SQ move);
};

extern Search * S;
