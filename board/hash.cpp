#include "board.h"

static constexpr double piece_scores[NUM_PIECES] = {200, 9, 3, 3, 5, 1};
static const double mobility_weight = 0.1;

int board::countMoves(side s)
{
	int numMoves = 0;

	for (u64 i = 1; i > 0; i <<= 1) {
		if (i & pieces_side[s]) {
			b_pos p = getBoardPos(i);
			numMoves += count_bits(genStartMoves(p));
		}
	}

	return numMoves;
}

double board::hashBoard()
{
	double score = 0;

	for (int p = 1; p < NUM_PIECES; p++) {
		score += count_bits(pieces[side::white][p]) * piece_scores[p];
		score -= count_bits(pieces[side::black][p]) * piece_scores[p];
	}

	score += mobility_weight * countMoves(curPlayer);
	score -= mobility_weight * countMoves(opponent);
	return score;
}
