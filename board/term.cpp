#include "board.h"
#include <iostream>
#include <string>

// Here we define our functions for interacting with the board using the terminal instead of a GUI

void board::printBitboard(u64 b)
{
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      std::cout << (b & 1);
      b >>= 1;
    }
      
    std::cout << '\n';
  }
}

char board::printPos(u64 pos)
{
  const char lookup[] = "PNBRQKpnbrqk";

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

start_pos board::inputToStartPos(const std::string &input)
{
  char rank = input[0];
  char file = input[1];

  int l = (rank - 'A') + 8 * ('8' - file);
  u64 loc = (u64)1 << l;
  return locToStartPos(loc);
}

end_pos board::inputToEndPos(const std::string &input)
{
  start_pos p = inputToStartPos(input);
  return end_pos{p.bitboard, p.pc, p.loc};
}

start_pos board::getStartPos()
{
  std::string input = getValidInput("Select a piece to move: ");
  return inputToStartPos(input);
}

end_pos board::getEndPos()
{
  std::string input = getValidInput("Select where to move: ");
  return inputToEndPos(input);
}
