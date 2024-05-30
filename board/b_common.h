#pragma once
#include "../common.h"

enum castling : int {
	queenside = 0,
	kingside = 1,
};

struct b_pos {
	u64 *bitboard;
	piece pc;
	u64 loc;
};

enum class move_type {
  normal,
  queenside_castle,
  kingside_castle,
  en_passant,
  pawn_upgrade,
};

struct move {
  b_pos start, end;
  int d, sign;
  move_type type = move_type::normal;
};

struct gamestate {
  move m;
  int fiftyMoveCounter;
};
