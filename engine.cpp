#include "engine.h"

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
}

move engine::getMove()
{ 
  bool validMove = false;
  move m;

  while (!validMove) {
    b.print();
    m.start = getStartPos();
    b.genStartMoves(m.start);
    m.end = getEndPos();
    calcDiff(m);
    std::cout << "d = " << m.d << '\n';
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
    if (b.inCheck())
      std::cout << "Check please\n";
  }
}

int main(int argc, char *argv[])
{
  engine chess;
  chess.run();
  return 0;
} 
