#include "board.h"
//castling not working
//move generation penetrates enemy pieces
void board::reset()
{
  resetPieces();
  setPiecesSide();
  curPlayer = side::white;
  canCastle[side::white][queenside] = canCastle[side::white][kingside]
    = canCastle[side::black][queenside] = canCastle[side::black][kingside] = true;
}

void board::resetPieces()
{
  for (int s = 0; s < NUM_SIDES; s++)
    for (int p = 0; p < NUM_PIECES; p++)
      pieces[s][p] = piece_start[s][p];
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

void board::movePieces(const move &m)
{
  *m.start.bitboard ^= (m.start.loc | m.end.loc);
  if (m.end.bitboard) *m.end.bitboard ^= m.end.loc;

  // Castling
  if (m.start.pc == piece::king && m.d == 2) {
    std::cout << "Castling\n";
    if (m.end.loc << 1 & pieces[curPlayer][piece::rook])
      pieces[curPlayer][piece::rook] ^= (m.end.loc << 1 | m.end.loc >> 1);
    else
      pieces[curPlayer][piece::rook] ^= (m.end.loc >> 2 | m.end.loc << 1);
  }

  setPiecesSide();
}

void board::doMove(const move &m, bool pretend)
{
  movePieces(m);
  
  if (!pretend) {
    if (m.start.loc == piece_start[curPlayer][piece::king])
      canCastle[curPlayer][queenside] = canCastle[curPlayer][kingside] = false;

    if (m.start.loc & piece_start[curPlayer][piece::rook]) {
      castling side = (m.start.loc & (u64)0x8000000000000080) ? kingside : queenside;
      canCastle[curPlayer][side] = false;
    }

    move_history.push_front(m);
  }
}

void board::undoMove(const move &m) {
  //move m = move_history.front();
  movePieces(m);
  //move_history.pop_front();
}

side board::getOpponent()
{
  return (curPlayer == side::white ? side::black : side::white);
}

bool board::validateStartPos(const start_pos &p)
{
  return p.loc > 0 && p.bitboard != nullptr && *p.bitboard & pieces_side[curPlayer];
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
    return (m.start.loc & startPos) && m.end.bitboard == nullptr && checkCollision(m, 8 * m.sign, 0);
  if (m.d == 7 * sign || m.d == 9 * sign)
    return m.end.bitboard != nullptr;

  return false;
}

bool board::validateRookMove(const move &m)
{
  int d = (m.d > 0 ? m.d : -m.d);
  
  if (d < 8)
    return checkCollision(m, m.sign, (m.sign > 0 ? left_boundary : right_boundary));
  if (d % 8 == 0)
    return checkCollision(m, 8 * m.sign, 0);
  
  return false;
}

bool board::validateBishopMove(const move &m)
{
  if (m.d % 9 == 0)
    return checkCollision(m, 9 * m.sign, (m.sign > 0 ? right_boundary : left_boundary));
  if (m.d % 7 == 0)
    return checkCollision(m, 7 * m.sign, (m.sign > 0 ? left_boundary : right_boundary));

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

bool board::validateCastling(const move &m)
{
  std::cout << "Testing castling\n";
  u64 enemy_moves = genMoves(getOpponent());
  std::cout << "Enemy moves:\n";
  printBitboard(enemy_moves);
  std::cout << "End loc: " << m.end.loc << "\n";
  std::cout << "Queenside: " << (m.end.loc & (u64)0x0400000000000004) << "\n";
  std::cout << "Kingside: " << (m.end.loc & (u64)0x4000000000000040) << "\n";
  // Queenside castling
  if (m.end.loc & (u64)0x0400000000000004 && canCastle[curPlayer][queenside] 
    && (enemy_moves & (u64)0x3800000000000038) == 0)
    return true;
  
  // Kingside castling
  if (m.end.loc & (u64)0x4000000000000040 && canCastle[curPlayer][kingside]
    && (enemy_moves & (u64)0x0E0000000000000E) == 0)
    return true;

  return false;
}

bool board::validateKingMove(const move &m)
{
  int d = (m.d > 0 ? m.d : -m.d);
  
  if (d == 2)
    return validateCastling(m);

  return d == 1 || d == 8 || d == 9 || d == 7;
}

bool board::checkCollision(const move &m, int d, u64 boundary)
{
  int abs = (d > 0 ? d : -d);
  boundary = ~boundary;
  u64 ray = m.start.loc;
  ray = (d > 0 ? ray >> abs : ray << abs) & boundary;

  while (ray != m.end.loc) {
    if (ray == 0 || ray & ~empty)
      return false;

    ray = (d > 0 ? ray >> abs : ray << abs) & boundary;
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
    default:
      return false;
  }

  if (valid) {
    movePieces(m);
    valid = !inCheck();
    movePieces(m);
  }

  return valid;
}

void board::switchPlayer()
{
  curPlayer = getOpponent();
}

u64 board::genWhitePawnMoves(u64 b)
{
  return (b >> 8 & empty) | ((b & piece_start[side::white][piece::pawn]) >> 16 & empty)
    | ((b >> 7 | b >> 9) & pieces_side[side::black]);
}

u64 board::genBlackPawnMoves(u64 b)
{
  return (b << 8 & empty) | ((b & piece_start[side::black][piece::pawn]) << 16 & empty)
    | ((b << 7 | b << 9) & pieces_side[side::white]);
}

u64 board::genPawnMoves(u64 b, side s)
{
  return s == side::white ? genWhitePawnMoves(b) : genBlackPawnMoves(b);
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
  return traceRay(b, 7, true, right_boundary, s) | traceRay(b, 9, true, left_boundary, s) | traceRay(b, 7, false, left_boundary, s) | traceRay(b, 9, false, right_boundary, s);
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

u64 board::genCastlingMoves(u64 b, side s)
{
  u64 result = 0;
  if (canCastle[s][queenside] && (b >> 1 & empty) && (b >> 2 & empty) && (b >> 3 & empty))
    result |= b >> 2;
  if (canCastle[s][kingside] && (b << 1 & empty) && (b << 2 & empty))
    result |= b << 2;
  
  return result;
}

u64 board::genKingMoves(u64 b, side s)
{
  return genCastlingMoves(b, s) | ((b << 1 | b << 8 | b << 9 | b << 7 | b >> 1 
  | b >> 8 | b >> 9 | b >> 7) & ~pieces_side[s]);
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
    default:
      return;
  }

  printBitboard(b);
}

bool board::inCheck()
{
  return genMoves(getOpponent()) & pieces[curPlayer][piece::king];
}
