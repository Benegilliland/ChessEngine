#include "board/board.h"

class engine {
private:
  board b;

  void reset();
  start_pos getStartPos();
  end_pos getEndPos();
  move getMove();
  void calcDiff(move &);
  void showAvailableMoves(const start_pos &);
  void doMove(move &);
  void switchPlayer();

public:
  void run();
};
