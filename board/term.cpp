#include "b_common.h"
#include "board.h"
#include <iostream>
#include <string>

/* Here we define our functions for interacting with the board using
   the terminal instead of a GUI*/

void board::printBitboard(u64 b)
{
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      std::cout << (b & 1) << ' ';
      b >>= 1;
    }

    std::cout << '\n';
  }
  std::cout << '\n';
}

char board::printPos(u64 pos)
{
  const char lookup[] = "KQBNRPkqbnrp";

  for (int i = 0; i < NUM_SIDES; i++) {
    for (int j = 0; j < NUM_PIECES; j++) {
      if (pieces[i][j] & pos)
        return lookup[i * NUM_PIECES + j];
    }
  }

  return ' ';
}

void board::print()
{
  u64 pos = 1;

  for (int i = 0; i < 8; i++) {
    std::cout << (8 - i) << ' ';

    for (int j = 0; j < 8; j++) {
      std::cout << printPos(pos) << ' ';
      pos <<= 1;
    }

    std::cout << "\n";
  }

  std::cout << "  A B C D E F G H\n";
}

bool board::validateInput(const std::string &input)
{
  if (input.length() != 2)
    return false;

  char rank = input[0];
  if (rank < 'A' || rank > 'H')
    return false;

  char file = input[1];
  if (file < '1' || file > '8')
    return false;

  return true;
}

std::string board::getValidInput(const std::string &cmd)
{
  std::string input;

  while (true) {
    std::cout << cmd;
    std::cin >> input;

    if (validateInput(input))
      break;

    std::cout << "Invalid input: " << input << '\n';
  }

  return input;
}

s_pos board::inputToPos(const std::string &input)
{
	char rank = input[0];
	char file = input[1];
	return {rank - 'A', '8' - file};
}

s_pos board::getStartPos()
{
  print();
  std::string input = getValidInput("Select a piece to move: ");
  return inputToPos(input);
}

s_pos board::getEndPos()
{
  std::string input = getValidInput("Select where to move: ");
  return inputToPos(input);
}

piece board::getPawnUpgrade()
{
  char c = '\0';

  while (c != 'Q' && c != 'B' && c != 'R' && c != 'N') {
    std::cout << "Enter pawn upgrade type: ";
    std::cin >> c;
  }

  switch (c) {
    case 'Q':
      return piece::queen;
    case 'B':
      return piece::bishop;
    case 'R':
      return piece::rook;
    case 'N':
      return piece::knight;
    default:
      return piece::none;
  }
}
