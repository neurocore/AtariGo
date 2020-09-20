#include <iostream>
#include <iomanip>
#include <algorithm>
#include "search.h"
#include "board.h"
#include "hash.h"

using namespace std;

Search * S;

void Node::generate(SQ hash_move)
{
    stage = 0;
    moves = BB();
    if (hash_move > 0)
        moves.set(hash_move);
    looked = moves;
}

SQ Node::get_next_move()
{
    SQ move = 0;
    while (!move)
    {
        if (moves)
        {
            BB move = moves.lsb();
            moves &= ~move;
            return move.bitscan();
        }

        BB opp, occ = B->stones[0] | B->stones[1];

        switch (stage)
        {
            case Stage::StageHash:
                stage++;

            case Stage::StageStickers:
                stage++;
                opp = B->stones[B->wtm ^ 1];
                moves = opp.expand() & ~occ & ~looked;
                looked |= moves;
                continue;

            case Stage::StageGood:
                stage++;
                moves = occ.expand() & ~occ & ~looked;
                looked |= moves;
                continue;

            case Stage::StageOthers:
                stage++;
                moves = ~occ & ~looked;
                looked |= moves;
                break;

            default:
                break;
        }
        return 0;
    }
    return move;
}

void Search::think()
{
    nodes = EMPTY;
	status = Status::Playing;
    state = node;
	timer.set();

	for (int depth = 1; depth <= Options::MaxPly; depth++)
	{
		search_depth = depth;
		int val = pvs(-INF, INF, depth);
		if (status == Status::Waiting) break;

		int ms = static_cast<int>(timer.getms());

		cout << right << setw(2) << depth << " - " << SQ_OUT(node->best) << " | ";

		if      (val >  MATE) cout << "#" << left << setw(6) <<  (INF - val) / 2 + 1;
		else if (val < -MATE) cout << "#" << left << setw(6) << -(INF + val) / 2 - 1;
		else                  cout << left << setw(7) << val;

		cout << " nps " << setw(6) << 1000 * nodes / (ms + 1)
             << " " << ms << "ms\n";

		if (correct(node->best)) best = node->best;

		if (val >  MATE) if (INF - val <= depth) break;
		if (val < -MATE) if (INF + val <= depth) break;
	}
	cout << "bestmove " << SQ_OUT(best) << "\n\n";
}

int Search::pvs(int alpha, int beta, int depth)
{
    int ply = state - node;
    if (depth <= 0) return B->eval(ply); // qs(alpha, beta);

    int hash_type = HashAlpha;
    bool search_pv = true;
    state->best = SQ();
    int val = -INF;
    S->nodes++;

    if (S->status == Status::Waiting) return 0;
    if (ply >= Options::MaxPly) return 0;

	bool in_pv = (beta - alpha) > 1;

    // 1.1. Mate pruning /////////////////////////////////////

	if (ply > 0)
	{
		alpha = max(-INF + ply, alpha);
		beta = min(INF - (ply + 1), beta);
		if (alpha >= beta) return alpha;
	}

    // 1.2. Hash probe ///////////////////////////////////////

	HashEntry * he = 0;
    SQ hash_move = 0;

    if (Options::Hashing)
    {
	    he = H->get(B->hash);
        if (he)
        {
            if (!B->stones[B->wtm].test(he->move))
                hash_move = he->move;

            if (ply > 0) // Not in root
            {
                if (he->depth >= depth)
                {
                    int val = he->val;
                    if      (val >  MATE && val <=  INF) val -= ply;
                    else if (val < -MATE && val >= -INF) val += ply;

                    // Exact score
                    if (he->type == HashExact) return val;
                    else if (he->type == HashAlpha && val <= alpha) return alpha;
                    else if (he->type == HashBeta  && val >= beta) return beta;
                }
            }
        }
    }

    state->generate(hash_move);

    while (SQ move = state->get_next_move())
    {
        make(move);

#ifdef DEBUG_MOVES
        for (int i = 0; i < state - node; i++) cout << " ";
        cout << SQ_OUT(move) << "\n";
#endif
        int new_depth = depth - 1;
        bool reduced = false;

        if (search_pv)
            val = -pvs(-beta, -alpha, new_depth);
        else
        {
            val = -pvs(-alpha - 1, -alpha, new_depth);
            if (val > alpha && val < beta)
                val = -pvs(-beta, -alpha, new_depth);
        }

        unmake(move);

        if (val > alpha)
        {
            alpha = val;
            hash_type = HashExact;
            state->best = move;
            search_pv = false;

            if (val >= beta)
            {
                alpha = beta;
                hash_type = HashBeta;
                break;
            }
        }
    }

    if (Options::Hashing)
        H->set(B->hash, state->best, depth, ply, alpha, hash_type);

    return alpha;
}

void Search::make(SQ move)
{
    ASSERT(B->is_empty(move),
    {
        B->print();
        cout << SQ_OUT(move) << "\n";
        state->looked.print();
        state->moves.print();
    });
    B->place_stone(move);
    state++;
}

void Search::unmake(SQ move)
{
    state--;
    B->remove_stone(move);
}
