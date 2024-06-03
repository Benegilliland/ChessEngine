#include "board.h"
#include <iostream>

static constexpr double piece_scores[NUM_PIECES] = {200, 9, 3, 3, 5, 1};
static const double mobility_weight = 0.1;

int board::countMoves()
{
	int numMoves = 0;

	for (u64 i = 1; i > 0; i <<= 1) {
		if (i & pieces_side[curPlayer]) {
			b_pos p = getBoardPos(i);
			numMoves += count_bits(genStartMoves(p));
		}
	}

	return numMoves;
}

// Fix later - we are recalculating moves for our own player
double board::hashBoard()
{
	double score = 0;

	for (int p = 1; p < NUM_PIECES; p++) {
		score += count_bits(pieces[curPlayer][p]) * piece_scores[p];
		score -= count_bits(pieces[opponent][p]) * piece_scores[p];
	}

	score += mobility_weight * countMoves();
	opponent = curPlayer;
	curPlayer = getOpponent();
	score -= mobility_weight * countMoves();
	opponent = curPlayer;
	curPlayer = getOpponent();
	return score;
}
