#pragma once
#include <cstdint>

typedef uint64_t u64;

static const int NUM_SIDES = 2;
static const int NUM_PIECES = 6;

// The order of these values is used implicitly by the spritesheet and terminal output
enum side : int {
  white = 0,
  black = 1,
};

// The order of these values is used implicitly by the spritesheet and terminal output
enum piece : int {
  king = 0,
  queen = 1,
  bishop = 2,
  knight = 3,
  rook = 4,
  pawn =  5,
  none = 6,
};

struct s_pos {
	int rank, file;
};

int count_bits(u64);
