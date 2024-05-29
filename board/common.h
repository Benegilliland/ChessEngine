#pragma once
#include <cstdint>

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
