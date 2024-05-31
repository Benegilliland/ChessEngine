#include "ai.h"

ai::ai(const  board &_b, const side &_s) : b(_b), s(_s) {}

/*int ai::evaluateBoard()
{
	return 0;
}

u64 ai::getPieceMoves(u64 l)
{
	b_pos p = b.getBoardPos(l);
2	return (b.validateStartPos(p) ? genStartMoves(p) : 0);
}

int ai::decideMove()
{
	for (u64 i = 1; i > 0; i <<= 1) {
		u64 moves = getPieceMoves(i);
		if (moves == 0) continue;

	}
}*/
