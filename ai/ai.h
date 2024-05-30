#include "../common.h"

class ai {
private:
	int evaluateBoard();

public:
	s_pos getStartMove();
	s_pos getEndMove();
	piece getPawnUpgrade();
};
