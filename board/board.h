#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <vector>
#include <list>

typedef uint64_t u64;

enum side : int {
  white = 0,
  black = 1,
};

enum piece : int {
  pawn = 0,
  knight = 1,
  bishop = 2,
  rook = 3,
  queen = 4,
  king =  5,
  none = 6,
};

enum castling : int {
  queenside = 0,
  kingside = 1,
};

struct start_pos {
  u64 *bitboard;
  piece pc;
  u64 loc;
};

struct end_pos {
  u64 *bitboard;
  piece pc;
  u64 loc;
};

struct move {
  start_pos start;
  end_pos end;
  int d;
  int sign;
};

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
  std::list<move> move_history;

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
  bool validatePawnMove(const move &);
  bool validateRookMove(const move &);
  bool validateBishopMove(const move &);
  bool validateKnightMove(const move &);
  bool validateQueenMove(const move &);
  bool validateCastling(const move &);
  bool validateKingMove(const move &);
  bool checkCollision(const move &, int, u64);
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
  u64 genMoves(side);

public:
  void reset();
  void print();
  start_pos getStartPos();
  end_pos getEndPos();  
  move getMove();
  void movePieces(const move &);
  void doMove(const move &, bool pretend = false);
  void undoMove(const move &);
  void undoLastMove();
  bool validateStartPos(const start_pos &);
  bool validateMove(const move &);
  void switchPlayer();
  void genStartMoves(const start_pos &);
  bool inCheck();
};
