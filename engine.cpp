#include "engine.h"
#include <unistd.h>

void engine::reset()
{
  b.reset();
}

start_pos engine::getStartPos()
{
  start_pos p;
  bool validPos = false;

  while (!validPos) {
    p = b.getStartPos();
    validPos = b.validateStartPos(p);
  }

  return p;
}

end_pos engine::getEndPos()
{
  return b.getEndPos();
}

void engine::calcDiff(move &m)
{
  u64 dividend;
  int sign;

  if (m.start.loc > m.end.loc) {
    dividend = m.start.loc / m.end.loc;
    sign = 1;
  }
  else {
    dividend = m.end.loc / m.start.loc;
    sign = -1;
  }

  int d = 0;

  while (dividend >>= 1 > 0)
    d += sign;

  m.sign = sign;
  m.d = d;

  if (m.start.pc == piece::pawn && m.end.pc == piece::none && (m.d == 7 || m.d == 9 || m.d == -9 || m.d == -7))
    m.type = move_type::en_passant;
}

void engine::showAvailableMoves(const start_pos &p)
{
  b.genStartMoves(p);
}

move engine::getMove()
{ 
  bool validMove = false;
  move m;

  while (!validMove) {
    b.print();
    m.start = getStartPos();
    showAvailableMoves(m.start);
    m.end = getEndPos();
    calcDiff(m);
    
    // Pawn
    if (m.start.pc == piece::pawn && (m.end.loc & 0xFF000000000000FF))
      m.type = b.getPawnUpgradeType();

    validMove = b.validateMove(m);
  }

  return m;
}

void engine::doMove(move &m)
{
  b.doMove(m);
}

void engine::switchPlayer()
{
  b.switchPlayer();
}

void engine::run()
{
  reset();

  while (true) {
    move m = getMove();
    doMove(m);
    switchPlayer();

    if (b.gameOver()) {
      std::cout << (b.inCheck() ? "Checkmate" : "Stalemate") << "\n";
      sleep(5);
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  engine chess;
  chess.run();
  return 0;
} 
