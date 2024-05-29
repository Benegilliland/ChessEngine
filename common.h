#pragma once
#include <cstdint>

typedef uint64_t u64;

const int NUM_SIDES = 2;
const int NUM_PIECES = 6;

enum side : int {
  white = 0,
  black = 1,
};

enum piece : int {
  king = 0,
  queen = 1,
  bishop = 2,
  knight = 3,
  rook = 4,
  pawn =  5,
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

enum class move_type {
  normal, 
  queenside_castle, 
  kingside_castle, 
  en_passant, 
  pawn_upgrade_queen, 
  pawn_upgrade_rook, 
  pawn_upgrade_knight, 
  pawn_upgrade_bishop
};

struct move {
  start_pos start;
  end_pos end;
  int d;
  int sign;
  move_type type = move_type::normal;
};

struct gamestate {
  move m;
  int fiftyMoveCounter;
};
