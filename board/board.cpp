#include "b_common.h"
#include "board.h"

// Add draw by repetition
// Add draw by insufficient material
// En passant broken
// Pawn move generation broken
// Add pawn upgrade GUI
// Optimisation

void board::reset()
{
  resetPieces();
  setPiecesSide();
  curPlayer = side::white;
  canCastle[side::white][queenside] = canCastle[side::white][kingside]
    = canCastle[side::black][queenside] = canCastle[side::black][kingside] = true;
  fiftyMoveCounter = 0;
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

b_pos board::getBoardPos(s_pos p)
{
	u64 loc = (u64)1 << (p.file * 8 + p.rank);
	return getBoardPos(loc);
}

b_pos board::getBoardPos(u64 loc)
{
  for (int s = 0; s < NUM_SIDES; s++)
    for (int p = 0; p < NUM_PIECES; p++)
      if (pieces[s][p] & loc)
        return {&pieces[s][p], (piece)p, loc};

  return {nullptr, piece::none, loc};
}

void board::movePieces(const move &m)
{
  *m.start.bitboard ^= (m.start.loc | m.end.loc);
  if (m.end.bitboard) *m.end.bitboard ^= m.end.loc;

  // Castling
  if (m.start.pc == piece::king && (m.d == 2 || m.d == -2)) {
    if (m.end.loc << 1 & pieces[curPlayer][piece::rook])
      pieces[curPlayer][piece::rook] ^= (m.end.loc << 1 | m.end.loc >> 1);
    else
      pieces[curPlayer][piece::rook] ^= (m.end.loc >> 1 | m.end.loc << 1);
  }

  // En passant
  if (m.type == move_type::en_passant) {
    if (curPlayer == side::white)
      pieces[side::black][piece::pawn] ^= m.end.loc << 8;
    else
      pieces[side::white][piece::pawn] ^= m.end.loc >> 8;
  }

  // Pawn upgrades
  if (m.type == move_type::pawn_upgrade_queen) {
    pieces[curPlayer][piece::pawn] ^= m.end.loc;
    pieces[curPlayer][piece::queen] ^= m.end.loc;
  }
  else if (m.type == move_type::pawn_upgrade_rook) {
    pieces[curPlayer][piece::pawn] ^= m.end.loc;
    pieces[curPlayer][piece::rook] ^= m.end.loc;
  }
  else if (m.type == move_type::pawn_upgrade_knight) {
    pieces[curPlayer][piece::pawn] ^= m.end.loc;
    pieces[curPlayer][piece::knight] ^= m.end.loc;
  }
  else if (m.type == move_type::pawn_upgrade_bishop) {
    pieces[curPlayer][piece::pawn] ^= m.end.loc;
    pieces[curPlayer][piece::bishop] ^= m.end.loc;
  }

  setPiecesSide();
}

void board::doMove(const move &m)
{
  movePieces(m);

  if (m.start.loc == piece_start[curPlayer][piece::king])
    canCastle[curPlayer][queenside] = canCastle[curPlayer][kingside] = false;

  if (m.start.loc & piece_start[curPlayer][piece::rook]) {
    castling side = (m.start.loc & (u64)0x8000000000000080) ? kingside : queenside;
    canCastle[curPlayer][side] = false;
  }

  if (m.end.pc != piece::none)
    checkInsufficientMaterial();

  move_history.push_front(gamestate{m, fiftyMoveCounter});

  if (m.end.pc == piece::none && m.start.pc != piece::pawn)
    fiftyMoveCounter++;
  else
    fiftyMoveCounter = 0;
}

void board::undoLastMove() {
  gamestate history = move_history.front();
  move_history.pop_front();

  movePieces(history.m);
  fiftyMoveCounter = history.fiftyMoveCounter;
}

side board::getOpponent()
{
  return (curPlayer == side::white ? side::black : side::white);
}

bool board::validateStartPos(const b_pos &p)
{
  return p.loc > 0 && p.bitboard != nullptr && *p.bitboard & pieces_side[curPlayer];
}

bool board::validateEnPassant(const move &m)
{
  move prev = move_history.front().m;
  return prev.start.pc == piece::pawn && (prev.d == 16 || prev.d == -16)
    && m.end.loc == (prev.sign > 0 ? prev.start.loc >> 8 : prev.start.loc << 8);
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
    return (m.type == move_type::en_passant ? validateEnPassant(m) : m.end.bitboard != nullptr);

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

bool board::validateQueensideCastling(const move &m, u64 mask, u64 enemy_moves)
{
  u64 collisions = 0x1E0000000000001E & mask;
  u64 own_pieces = (pieces_side[curPlayer] ^ pieces[curPlayer][piece::king]) & collisions;
  enemy_moves &= collisions;

  return m.end.loc & 0x0400000000000004 && canCastle[curPlayer][queenside]
        && !enemy_moves && !own_pieces;
}

bool board::validateKingsideCastling(const move &m, u64 mask, u64 enemy_moves)
{
  u64 collisions = 0x7000000000000070 & mask;
  u64 own_pieces = (pieces_side[curPlayer] ^ pieces[curPlayer][piece::king]) & collisions;
  enemy_moves &= collisions;

  return m.end.loc & 0x4000000000000040 && canCastle[curPlayer][kingside]
      && enemy_moves == 0 && own_pieces == 0;
}

bool board::validateCastling(const move &m)
{
  u64 mask = (curPlayer == side::white) ? 0xFF00000000000000 : 0x00000000000000FF;
  u64 enemy_moves = genMoves(getOpponent()) & mask;

  return validateQueensideCastling(m, mask, enemy_moves) || validateKingsideCastling(m, mask, enemy_moves);
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

void board::togglePiece(const b_pos &p, side s)
{
  *p.bitboard ^= p.loc;
  pieces_side[s] ^= p.loc;
  empty ^= p.loc;
}

bool board::pieceCanMove(const b_pos &p)
{
	togglePiece(p, curPlayer);
	bool canMove = !inCheck();
	togglePiece(p, curPlayer);
	return canMove;
}

bool board::hasAvailableMoves()
{
  u64 b = 1;

  while (b > 0) {
    if ((b & pieces_side[curPlayer]) == 0) {
      b <<= 1;
      continue;
    }

    b_pos p = getBoardPos(b);
    if (genStartMoves(p) > 0) return true;
    b <<= 1;
  }

  return false;
}

void board::switchPlayer()
{
  curPlayer = getOpponent();
}

u64 board::genWhitePawnMoves(u64 b)
{
  return (b >> 8 & empty) | ((b & piece_start[side::white][piece::pawn]) >> 16 & empty)
    | (((b >> 7 & ~left_boundary) | (b >> 9 & ~right_boundary)) & pieces_side[side::black]);
}

u64 board::genBlackPawnMoves(u64 b)
{
  return (b << 8 & empty) | ((b & piece_start[side::black][piece::pawn]) << 16 & empty)
    | (((b << 7 & ~right_boundary) | (b << 9 & ~left_boundary)) & pieces_side[side::white]);
}

u64 board::genEnPassantMoves(u64 b, side s)
{
  if (move_history.empty()) return false;
  move prev = move_history.front().m;

  if (s == side::white && prev.d == -16)
    return (((b >> 1 & ~right_boundary) | (b << 1 & ~left_boundary))
      & pieces[side::black][piece::pawn]) >> 8;
  if (s == side::black && prev.d == 16)
    return (((b >> 1 & ~right_boundary) | (b << 1 & ~left_boundary))
      & pieces[side::white][piece::pawn]) << 8;

  return 0;
}

u64 board::genPawnMoves(u64 b, side s)
{
  return (s == side::white ? genWhitePawnMoves(b) : genBlackPawnMoves(b)) | genEnPassantMoves(b, s);
}

u64 board::traceRay(u64 bitboard, int direction, bool left, u64 boundary, side s)
{
  u64 result = 0, ray = bitboard;
  u64 mask = ~pieces_side[s] & ~boundary;

  while (ray > 0) {
    ray = (left ? ray << direction : ray >> direction) & mask;
    result |= ray;
    ray &= empty;
  }

  return result;
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
  return (((b << 10 | b >> 6) & ~right_boundary_knight)
      | ((b << 17 | b >> 15) & ~left_boundary)
      | ((b << 6 | b >> 10) & ~left_boundary_knight)
      | ((b << 15 | b >> 17) & ~right_boundary))
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

u64 board::genStartMoves(const b_pos &p)
{
  u64 b = 0;

  switch (p.pc) {
    case piece::pawn:
      b = genPawnMoves(p.loc, curPlayer);
	std::cout << "Test\n";
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
      return validateKingMoves(b, curPlayer);
      break;
    default:
      return 0;
  }

  if (b == 0) return 0;
  // if b is king, check king moves (algorithm up top)
  if (!inCheck()) return (pieceCanMove(p) ? b : 0);
  return pieceCanBlockCheck(p, b);
}

// If we're in check, see whether our piece can block it. First we take the intersection of b's moves with the opponent's attack squares, then we check whether any of the remaining moves block check
u64 board::pieceCanBlockCheck(const b_pos &p, u64 b)
{
        return b & genMoves(getOpponent()) & genQueenMoves(pieces[curPlayer][piece::king], curPlayer);
}

u64 board::validateKingMoves(u64 b, side s)
{
        if (b == 0) return 0;
        pieces_side[curPlayer] ^= pieces[curPlayer][piece::king];
        empty ^= pieces[curPlayer][piece::king];
        b &= ~genMoves(getOpponent());
        pieces_side[curPlayer] ^= pieces[curPlayer][piece::king];
        empty ^= pieces[curPlayer][piece::king];
        return b;
}

bool board::inCheck()
{
  return genMoves(getOpponent()) & pieces[curPlayer][piece::king];
}

// king vs king, king and bishop vs king, king and knight vs king, king and bishop vs and bishop on same coliur
bool board::checkInsufficientMaterial()
{
  return false;
}

// threefold repetition - same player to move, same board position, same castling and en passant rules
bool board::checkRepetitionDraw()
{
  return false;
}

bool board::checkFiftyMoveDraw()
{
  return fiftyMoveCounter >= 50;
}

bool board::gameOver()
{
  return !hasAvailableMoves() || checkFiftyMoveDraw()
      || checkInsufficientMaterial() || checkRepetitionDraw();
}
