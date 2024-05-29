#pragma once
#include "common.h"
#include <string>
#include <iostream>
#include <vector>
#include <list>

class board {
private:
  static const int NUM_SIDES = 2;
  static const int NUM_PIECES = 6;

  static constexpr u64 piece_start[NUM_SIDES][NUM_PIECES] = { 
    {0x00ff000000000000, 0x4200000000000000, 0x2400000000000000,
     0x8100000000000000, 0x0800000000000000, 0x1000000000000000},

    {0x000000000000ff00, 0x0000000000000042, 0x0000000000000024,
     0x0000000000000081, 0x0000000000000008, 0x0000000000000010},
  };
  
  static const u64 right_boundary = 0x8080808080808080;
  static const u64 left_boundary = 0x101010101010101;
  u64 pieces[NUM_SIDES][NUM_PIECES];
  u64 pieces_side[NUM_SIDES];
  u64 empty;
  bool canCastle[NUM_SIDES][2];
  side curPlayer;
  std::list<gamestate> move_history;
  int fiftyMoveCounter;

  // Console functions
  void printBitboard(u64);
  char printPos(u64);
  bool validateInput(const std::string &);
  std::string getValidInput(const std::string &);
  start_pos inputToStartPos(const std::string &);
  end_pos inputToEndPos(const std::string &);

  // Internal functions
  void resetPieces();
  start_pos locToStartPos(u64);
  void setPiecesSide();
  bool validateEnPassant(const move &);
  bool validatePawnMove(const move &);
  bool validateRookMove(const move &);
  bool validateBishopMove(const move &);
  bool validateKnightMove(const move &);
  bool validateQueenMove(const move &);
  bool validateQueensideCastling(const move &, u64, u64);
  bool validateKingsideCastling(const move  &, u64, u64);
  bool validateCastling(const move &);
  bool validateKingMove(const move &);
  bool checkCollision(const move &, int, u64);
  side getOpponent();
  u64 traceRay(u64, int, bool, u64, side);
  u64 genWhitePawnMoves(u64);
  u64 genBlackPawnMoves(u64);
  u64 genEnPassantMoves(u64, side);
  u64 genPawnMoves(u64, side);
  u64 genRookMoves(u64, side);  
  u64 genBishopMoves(u64, side);
  u64 genKnightMoves(u64, side);
  u64 genQueenMoves(u64, side);
  u64 genCastlingMoves(u64, side);
  u64 genKingMoves(u64, side);
  u64 genMoves(side);
  void togglePiece(const start_pos &, side);
  //u64 genValidatedMoves(const pos &, u64);
  void checkInsufficientMaterial();
  void checkDrawRepetition();

public:
  void reset();
  void print();
  start_pos getStartPos();
  end_pos getEndPos();
  move_type getPawnUpgradeType();
  move getMove();
  void movePieces(const move &);
  void doMove(const move &);
  void undoLastMove();
  bool validateStartPos(const start_pos &);
  bool validateMove(const move &);
  void switchPlayer();
  u64 genStartMoves(const start_pos &);
  bool hasAvailableMoves();
  bool gameOver();
  bool inCheck();
};
