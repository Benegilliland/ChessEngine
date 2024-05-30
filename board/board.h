#pragma once
#include "../common.h"
#include "b_common.h"
#include <string>
#include <iostream>
#include <vector>
#include <list>

class board {
private:
  static constexpr u64 piece_start[NUM_SIDES][NUM_PIECES] = {
    {0x1000000000000000, 0x0800000000000000, 0x2400000000000000,
     0x4200000000000000, 0x8100000000000000, 0x00ff000000000000},
    {0x0000000000000010, 0x0000000000000008, 0x0000000000000024,
     0x0000000000000042, 0x0000000000000081, 0x000000000000ff00}
  };

  static const u64 right_boundary = 0x8080808080808080;
  static const u64 left_boundary = 0x101010101010101;
  static const u64 left_boundary_knight = 0xC0C0C0C0C0C0C0C0;
  static const u64 right_boundary_knight = 0x0303030303030303;

  u64 pieces[NUM_SIDES][NUM_PIECES];
  u64 pieces_side[NUM_SIDES];
  u64 empty;

  bool canCastle[NUM_SIDES][2];
  side curPlayer;

  std::list<gamestate> move_history;
  int fiftyMoveCounter;
  u64 enPassant;
  u64 startMoves;

  // Console functions
  char printPos(u64);
  bool validateInput(const std::string &);
  std::string getValidInput(const std::string &);
  s_pos inputToPos(const std::string &);

  // Internal functions
  void resetPieces();
  void setPiecesSide();
  side getOpponent();
  u64 traceRay(u64, int, bool, u64, side);
  u64 genWhitePawnMoves(u64);
  u64 genBlackPawnMoves(u64);
  u64 genPawnMoves(u64, side);
  u64 genRookMoves(u64, side);
  u64 genBishopMoves(u64, side);
  u64 genKnightMoves(u64, side);
  u64 genQueenMoves(u64, side);
  u64 genCastlingMoves(u64, side);
  u64 genKingMoves(u64, side);
  u64 validateKingMoves(u64, side);
  u64 genMoves(side);
  void togglePiece(const b_pos &, side);
  bool checkFiftyMoveDraw();
  bool checkInsufficientMaterial();
  bool checkRepetitionDraw();
  bool pieceCanMove(const b_pos &);
  u64 pieceCanBlockCheck(const b_pos &, u64);

public:
  void reset();
  void printBitboard(u64);
  void print();
  b_pos getBoardPos(s_pos);
  b_pos getBoardPos(u64);
  s_pos getStartPos();
  s_pos getEndPos();
  piece getPawnUpgrade();
  move getMove();
  void movePieces(const move &);
  void doMove(const move &);
  void undoLastMove();
  bool validateStartPos(const b_pos &);
  void saveStartMoves(u64);
  bool validateMove(const move &);
  void switchPlayer();
  u64 genStartMoves(const b_pos &);
  bool hasAvailableMoves();
  bool gameOver();
  bool inCheck();
  void doPawnUpgrade(const move &, piece);
  void doQueensideCastle(const move &);
  void doKingsideCastle(const move &);
  void doEnPassant(const move &);
  void setMoveType(move &);
};
