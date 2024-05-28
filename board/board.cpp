#include "board.h"

void board::reset()
{
  pieces[side::black][piece::pawn] = 0x000000000000ff00;
  pieces[side::white][piece::pawn] = 0x00ff000000000000;
  
  pieces[side::black][piece::knight] = 0x0000000000000024;
  pieces[side::white][piece::knight] = 0x2400000000000000;

  pieces[side::black][piece::bishop] = 0x0000000000000042;
  pieces[side::white][piece::bishop] = 0x4200000000000000;

  pieces[side::black][piece::rook] = 0x0000000000000081;
  pieces[side::white][piece::rook] = 0x8100000000000000;

  pieces[side::black][piece::queen] = 0x0000000000000010;
  pieces[side::white][piece::queen] = 0x1000000000000000;

  pieces[side::black][piece::king] = 0x0000000000000008;
  pieces[side::white][piece::king] = 0x0800000000000000;

  setPiecesSide();
  curPlayer = side::white;
}

void board::setPiecesSide()
{
  for (int s = 0; s < NUM_SIDES; s++) {
    pieces_side[s] = 0;

    for (int p = 0; p < NUM_PIECES; p++) {
      pieces_side[s] |= pieces[s][p];
    }
  }

  empty = ~(pieces_side[side::white] | pieces_side[side::black]);
}

start_pos board::locToStartPos(u64 loc)
{
  for (int s = 0; s < NUM_SIDES; s++)
    for (int p = 0; p < NUM_PIECES; p++) 
      if (pieces[s][p] & loc)
        return start_pos{&pieces[s][p], (piece)p, loc};

  return start_pos{nullptr, piece::none, loc};
}

void board::doMove(const move &m)
{
  u64 mv = m.start.loc | m.end.loc;
  
  *m.start.bitboard ^= mv;
  if (m.end.bitboard) *m.end.bitboard ^= m.end.loc;

  // Optimise this
  setPiecesSide();
  //move_history.push_front(m);
}

void board::undoMove(const move &m) {
  //move m = move_history.front();
  u64 mv = m.start.loc | m.end.loc;
  *m.start.bitboard ^= mv;
  if (m.end.bitboard) *m.end.bitboard ^= m.end.loc;

  setPiecesSide();
  //move_history.pop_front();
}

side board::getOpponent()
{
  return (curPlayer == side::white ? side::black : side::white);
}

bool board::validateStartPos(const start_pos &p)
{
  return p.loc > 0 && p.bitboard != nullptr;
}

bool board::validatePawnMove(const move &m)
{
  u64 startPos;
  int sign;

  if (*m.start.bitboard == pieces[side::white][piece::pawn]) {
    startPos = 0x00ff000000000000;
    sign = 1;
  }
  else {
    startPos = 0x000000000000ff00;
    sign = -1;
  }

  if (m.d == 8 * sign)
    return m.end.bitboard == nullptr;
  if (m.d == 16 * sign)
    return (m.start.loc & startPos) && m.end.bitboard == nullptr && checkCollision(m, 8 * m.sign);
  if (m.d == 7 * sign || m.d == 9 * sign)
    return m.end.bitboard != nullptr;

  return false;
}

bool board::validateRookMove(const move &m)
{
  int d = (m.d > 0 ? m.d : -m.d);
  
  if (d < 8)
    return checkCollision(m, m.sign);
  if (d % 8 == 0)
    return checkCollision(m, 8 * m.sign);
  
  return false;
}

bool board::validateBishopMove(const move &m)
{
  if (m.d % 9 == 0)
    return checkCollision(m, 9 * m.sign);
  if (m.d % 7 == 0)
    return checkCollision(m, 7 * m.sign);

  return false;
}

bool board::validateKnightMove(const move &m)
{
  int d = (m.d > 0 ? m.d : -m.d);
  return d == 10 || d == 6 || d == 15 || d == 17;
}

bool board::validateQueenMove(const move &m)
{
  return validateRookMove(m) || validateBishopMove(m);
}

bool board::validateKingMove(const move &m)
{
  int d = (m.d > 0 ? m.d : -m.d);
  return d == 1 || d == 8 || d == 9 || d == 7;
}

bool board::checkCollision(const move &m, int d)
{
  int abs = (d > 0 ? d : -d);
  u64 ray = (d > 0 ? m.start.loc >> abs : m.start.loc << abs);

  while (ray != m.end.loc) {
    if (ray == 0 || ray & ~empty)
      return false;

    ray = (d > 0 ? ray >> abs : ray << abs);
  }

  return true;
}

bool board::validateMove(const move &m)
{
  if (m.start.loc == 0 || m.end.loc == 0)
      return false;

  if (m.start.loc & ~pieces_side[curPlayer])
    return false;

  if (m.end.loc & pieces_side[curPlayer])
    return false;

  bool valid = false;

  switch (m.start.pc) {
    case piece::pawn:
      valid = validatePawnMove(m);
      break;
    case piece::rook:
      valid = validateRookMove(m);
      break;
    case piece::bishop:
      valid = validateBishopMove(m);
      break;
    case piece::knight:
      valid = validateKnightMove(m);
      break;
    case piece::queen:
      valid = validateQueenMove(m);
      break;
    case piece::king:
      valid = validateKingMove(m);
      break;
  }

  if (valid) {
    doMove(m);
    valid = !inCheck();
    undoMove(m);
  }

  return valid;
}

void board::switchPlayer()
{
  curPlayer = getOpponent();
}

u64 board::genPawnMoves(u64 b, side s)
{
  return ((s == side::white ? b >> 8 : b << 8) & empty) | ((s == side::white ? (b & pawn_start[s]) >> 16 : (b & pawn_start[s] << 16)) & empty) 
    | ((s == side::white ? (b >> 7 | b >> 9) : (b << 7 | b << 9)) & pieces_side[(s == side::white ? side::black : side::white)]);
}

u64 board::traceRay(u64 bitboard, int direction, bool left, u64 boundary, side s)
{
  u64 result = bitboard, ray = bitboard;
  boundary = ~boundary;

  for (int i = 1; i < 8; i++) {
    ray = (left ? ray << direction : ray >> direction) & boundary & ~pieces_side[s];
    result |= ray;
  }

  return result ^ bitboard;
}

u64 board::genRookMoves(u64 b, side s)
{
  return traceRay(b, 8, true, 0, s) | traceRay(b, 8, false, 0, s) | traceRay(b, 1, true, left_boundary, s) | traceRay(b, 1, false, right_boundary, s);
}

u64 board::genBishopMoves(u64 b, side s)
{
  return traceRay(b, 7, true, left_boundary, s) | traceRay(b, 9, true, right_boundary, s) | traceRay(b, 7, false, right_boundary, s) | traceRay(b, 9, false, left_boundary, s);
}

u64 board::genKnightMoves(u64 b, side s)
{
  return (b << 10 | b << 6 | b << 15 | b << 17 | b >> 10 | b >> 6 | b >> 15 | b >> 17) 
    & ~pieces_side[s];
}

u64 board::genQueenMoves(u64 b, side s)
{
  return genRookMoves(b, s) | genBishopMoves(b, s);
}

u64 board::genKingMoves(u64 b, side s)
{
  return (b << 1 | b << 8 | b << 9 | b << 7 | b >> 1 | b >> 8 | b >> 9 | b >> 7) 
    & ~pieces_side[s];
}

u64 board::genMoves(side s)
{
  u64 b = genPawnMoves(pieces[s][piece::pawn], s);
  b |= genRookMoves(pieces[s][piece::rook], s);
  b |= genKnightMoves(pieces[s][piece::knight], s);
  b |= genBishopMoves(pieces[s][piece::bishop], s);
  b |= genQueenMoves(pieces[s][piece::queen], s);
  b |= genKingMoves(pieces[s][piece::king], s);
  return b;
}

void board::genStartMoves(const start_pos &p)
{
  u64 b = 0;

  switch (p.pc) {
    case piece::pawn:
      b = genPawnMoves(p.loc, curPlayer);
      break;
    case piece::knight:
      b = genKnightMoves(p.loc, curPlayer);
      break;
    case piece::bishop:
      b = genBishopMoves(p.loc, curPlayer);
      break;
    case piece::rook:
      b = genRookMoves(p.loc, curPlayer);
      break;
    case piece::queen:
      b = genQueenMoves(p.loc, curPlayer);
      break;
    case piece::king:
      b = genKingMoves(p.loc, curPlayer);
      break;
  }

  printBitboard(b);
}

bool board::inCheck()
{
  return genMoves(getOpponent()) & pieces[curPlayer][piece::king];
}
