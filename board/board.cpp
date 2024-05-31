#include "b_common.h"
#include "board.h"

// Add draw by repetition
// Add draw by insufficient material
// Add pawn upgrade GUI
// Optimisation

void board::reset()
{
  resetPieces();
  setPiecesSide();
  curPlayer = side::white;
  opponent = side::black;
  canCastle[side::white][queenside] = canCastle[side::white][kingside]
    = canCastle[side::black][queenside] = canCastle[side::black][kingside] = true;
  fiftyMoveCounter = 0;
  enPassant = 0;
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

  if (m.start.pc == piece::pawn && (m.end.loc == m.start.loc >> 16))
	enPassant = m.start.loc >> 8;
  else if (m.start.pc == piece::pawn && (m.end.loc == m.start.loc << 16))
	enPassant = m.start.loc << 8;
  else
	enPassant = 0;

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

void board::saveStartMoves(u64 m)
{
  startMoves = m;
}

bool board::validateMove(const move &m)
{
	return m.end.loc & startMoves;
}

side board::getOpponent()
{
  return (curPlayer == side::white ? side::black : side::white);
}

bool board::validateStartPos(const b_pos &p)
{
  return p.loc > 0 && p.bitboard != nullptr && *p.bitboard & pieces_side[curPlayer];
}

void board::togglePiece(const b_pos &p, side s)
{
  *p.bitboard ^= p.loc;
  pieces_side[s] ^= p.loc;
  empty ^= p.loc;
}

// This function is now broken - should regenerate opponent moves, but can it be optimised?
// Check intersection between enemy ray tracing pieces and king's ray tracing
bool board::pieceCanMove(const b_pos &p)
{
	std::cout << "niiga\n";
	printBitboard(genOpponentRayMoves());
	return (p.loc & genOpponentRayMoves()
		& genQueenMoves(pieces[curPlayer][piece::king], opponent)) == 0;
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
  opponent = curPlayer;
  curPlayer = getOpponent();
  opponentMoves = genMoves(opponent);
}

u64 board::genWhitePawnMoves(u64 b)
{
  return (b >> 8 & empty) | ((b & piece_start[side::white][piece::pawn]) >> 16 & empty)
    | (((b >> 7 & ~left_boundary) | (b >> 9 & ~right_boundary)) & (pieces_side[side::black] | enPassant));
}

u64 board::genBlackPawnMoves(u64 b)
{
  return (b << 8 & empty) | ((b & piece_start[side::black][piece::pawn]) << 16 & empty)
    | (((b << 7 & ~right_boundary) | (b << 9 & ~left_boundary)) & (pieces_side[side::white] | enPassant));
}

u64 board::genPawnMoves(u64 b, side s)
{
  return (s == side::white ? genWhitePawnMoves(b) : genBlackPawnMoves(b));
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

  if (canCastle[s][queenside] && ((b >> 1 | b >> 2 | b >> 3) & empty & ~opponentMoves))
    result |= b >> 2;
  if (canCastle[s][kingside] && ((b << 1 | b << 2) & empty & ~opponentMoves))
    result |= b << 2;

  return result;
}

u64 board::genKingMoves(u64 b, side s)
{
  return genCastlingMoves(b, s) | ((((b << 1 | b << 9 | b >> 7) & ~left_boundary)
		 | ((b >> 1 | b >> 9 | b << 7) & ~right_boundary) | b >> 8 | b << 8) & ~pieces_side[s]);
}

u64 board::genDiagonalPawnMoves(u64 b, side s)
{
	if (s == side::white)
		return (((b >> 7 & ~left_boundary) | (b >> 9 & ~right_boundary)) & (pieces_side[side::black] | enPassant));
	else
		return (((b << 7 & ~right_boundary) | (b << 9 & ~left_boundary)) & (pieces_side[side::white] | enPassant));
}

u64 board::genOpponentRayMoves()
{
	u64 b = genRookMoves(pieces[opponent][piece::rook], opponent);
	b |= genBishopMoves(pieces[opponent][piece::bishop], opponent);
	b |= genQueenMoves(pieces[opponent][piece::queen], opponent);
	return b;
}

u64 board::genMoves(side s)
{
  u64 b = genDiagonalPawnMoves(pieces[s][piece::pawn], s);
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

  if (!inCheck()) return (pieceCanMove(p) ? b : 0);
  return pieceCanBlockCheck(p, b);
}

// If we're in check, see whether our piece can block it. First we take the intersection of b's moves with the opponent's attack squares, then we check whether any of the remaining moves block check
u64 board::pieceCanBlockCheck(const b_pos &p, u64 b)
{
        return b & (opponentMoves | pieces_side[opponent])
		& genQueenMoves(pieces[curPlayer][piece::king], curPlayer);
}

u64 board::validateKingMoves(u64 b, side s)
{
	std::cout << "King moves\n";
	printBitboard(b);
        if (b == 0) return 0;
        pieces_side[curPlayer] ^= pieces[curPlayer][piece::king];
        empty ^= pieces[curPlayer][piece::king];
        b &= ~opponentMoves;
	printBitboard(b);
        pieces_side[curPlayer] ^= pieces[curPlayer][piece::king];
        empty ^= pieces[curPlayer][piece::king];
        return b;
}

bool board::inCheck()
{
  return opponentMoves & pieces[curPlayer][piece::king];
}

// king vs king, king and bishop vs king, king and knight vs king, king and bishop vs and bishop on same coliur
bool board::checkInsufficientMaterial()
{
  if (pieces_side[curPlayer] == pieces[curPlayer][piece::king]) {
	u64 enemy_pieces = pieces_side[opponent]  ^ pieces[opponent][piece::king];
  }

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

void board::setMoveType(move &m)
{
	if (m.end.loc == enPassant)
		m.type = move_type::en_passant;
	else if (m.start.pc == piece::pawn && (m.end.loc & 0xFF000000000000FF))
		m.type = move_type::pawn_upgrade;
	else if (m.start.pc == piece::king && (m.end.loc == m.start.loc >> 2))
		m.type = move_type::queenside_castle;
	else if (m.start.pc == piece::king && (m.end.loc == m.start.loc << 2))
		m.type = move_type::kingside_castle;
	else
		m.type = move_type::normal;
}

void board::doPawnUpgrade(const move &m, piece pc)
{
	pieces[curPlayer][piece::pawn] ^= m.end.loc;
	pieces[curPlayer][pc] ^= m.end.loc;
	setPiecesSide();
}

void board::doEnPassant(const move &m)
{
	if (curPlayer == side::white)
		pieces[side::black][piece::pawn] ^= (m.end.loc << 8);
	else
		pieces[side::white][piece::pawn] ^= (m.end.loc >> 8);

	setPiecesSide();
}

void board::doQueensideCastle(const move &m)
{
	pieces[curPlayer][piece::rook] ^= (m.end.loc << 1 | m.end.loc >> 2);
	setPiecesSide();
}

void board::doKingsideCastle(const move &m) {
	pieces[curPlayer][piece::rook] ^= (m.end.loc << 1 | m.end.loc >> 1);
	setPiecesSide();
}
