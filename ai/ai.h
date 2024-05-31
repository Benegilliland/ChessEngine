#include "../common.h"
#include "../board/board.h"

class ai {
private:
	const board &b;
	const side s;

	u64 getPieceMoves(u64);
	int evaluateBoard();
	int evaluateMove(const move &);

public:
	ai(const board &, const side &);
	s_pos getStartMove();
	s_pos getEndMove();
	piece getPawnUpgrade();
};
