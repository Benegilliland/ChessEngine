#include "../common.h"
#include "../board/board.h"

struct best_move {
	int score;
	move m;
};

class c_ai {
private:
	b_pos endPos;
	move bestMove[4];

	int getBestMove(board &, int);

public:
	b_pos getStartPos(board &);
	b_pos getEndPos();
	piece getPawnUpgrade();
};
