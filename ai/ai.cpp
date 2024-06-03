#include "ai.h"
#include <iostream>

/*double c_ai::evaluateMove(const move &m)
{
	b.doMove(m);
	b.switchPlayer();
	double score = b.hashBoard();
	b.undoLastMove();
	b.switchPlayer();
	return score;
}

b_pos c_ai::getStartPos(board b)
{
	move m, bestMove;
	double bestScore = -999;

	for (u64 i = 1; i > 0; i <<= 1) {
		m.start = b.getBoardPos(i);
		if (!b.validateStartPos(m.start))
			continue;

		u64 moves = b.genStartMoves(m.start);

		for (u64 j = 1; j > 0; j <<= 1) {
			std::cout << "i: " << i << ", j: " << j << "\n";
			if (moves & j) {
				m.end = b.getBoardPos(j);

				double score = evaluateMove(m);
				if (score > bestScore)  {
					bestMove = m;
					bestScore = score;
				}
			}
		}
	}
	std::cout << "Best score: " << bestScore << "\n";
	endPos = bestMove.end;
	return bestMove.start;
}*/

int c_ai::getBestMove(board &b, int depth)
{
	if (depth >= 4) return b.hashBoard();

	move m;
	double bestScore = -999;

	for (u64 i = 1; i > 0; i <<= 1) {
		m.start = b.getBoardPos(i);
		if (!b.validateStartPos(m.start))
			continue;

		u64 moves = b.genStartMoves(m.start);

		for (u64 j = 1; j > 0; j <<= 1) {
			if ((moves & j) == 0) continue;
			//std::cout << "i: " << i << ", j: " << j << "\n";
			m.end = b.getBoardPos(j);
			b.doMove(m);
			b.switchPlayer();
			double score = getBestMove(b, depth + 1);
			b.undoLastMove();
			b.switchPlayer();

			if (score > bestScore) {
				bestMove[depth] = m;
				bestScore = score;
			}
		}
	}

	//std::cout << "Best score: " << bestScore << "\n";
	return bestScore;
}

b_pos c_ai::getStartPos(board &b)
{
	getBestMove(b, 0);
	endPos = bestMove[0].end;
	return bestMove[0].start;
}

b_pos c_ai::getEndPos()
{
	return endPos;
}
