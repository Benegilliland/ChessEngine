#include "engine.h"
#include <unistd.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

engine::engine(bool gui_enabled, int width, int height)
{
  if (gui_enabled)
    init_gui(width, height);
}

engine::~engine()
{
  if (gui)
    destroy_gui();
}

// Move to GUI file
void engine::init_gui(int width, int height)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "Error: unable to initialise SDL\n";
    return;
  }

  if ((window = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      width, height, 0)) == NULL) {
    std::cout << "Error: unable to create window\n";
    return;
  }

  if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL) {
    std::cout << "Error: unable to create renderer\n";
    return;
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    std::cout << "Error: unable to initialise SDL Image\n";
    return;
  }

  gui = new g_gui(renderer, width, height);
}

void engine::destroy_gui()
{
  delete gui;
  IMG_Quit();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void engine::reset()
{
  b.reset();
}

b_pos engine::getStartPos()
{
  s_pos pos;
  b_pos board_pos;
  bool validPos = false;

  while (!validPos) {
    pos = gui ? gui->getStartPos() : b.getStartPos();
    board_pos = b.getBoardPos(pos);
    b.printBitboard(board_pos.loc);
    validPos = b.validateStartPos(board_pos);
  }

  return board_pos;
}

b_pos engine::getEndPos()
{
	s_pos pos = gui ? gui->getEndPos() : b.getEndPos();
	b_pos end = b.getBoardPos(pos);
	b.printBitboard(end.loc);
	return end;
}

void engine::calcDiff(move &m)
{
  u64 dividend;
  int sign;

  if (m.start.loc > m.end.loc) {
    dividend = m.start.loc / m.end.loc;
    sign = 1;
  }
  else {
    dividend = m.end.loc / m.start.loc;
    sign = -1;
  }

  int d = 0;

  while (dividend >>= 1 > 0)
    d += sign;

  m.sign = sign;
  m.d = d;

  if (m.start.pc == piece::pawn && m.end.pc == piece::none && (m.d == 7 || m.d == 9 || m.d == -9 || m.d == -7))
    m.type = move_type::en_passant;
}

void engine::showAvailableMoves(const b_pos &p)
{
  b.printBitboard(b.genStartMoves(p));
}

move engine::getMove()
{
  bool validMove = false;
  move m;

  while (!validMove) {
    m.start = getStartPos();
    showAvailableMoves(m.start);
    m.end = getEndPos();
    calcDiff(m);

    // Pawn
    if (m.start.pc == piece::pawn && (m.end.loc & 0xFF000000000000FF))
      m.type = b.getPawnUpgradeType();

    validMove = b.validateMove(m);
  }

  return m;
}

void engine::doMove(move &m)
{
  b.doMove(m);
  if (gui) gui->doMove(m.start.loc, m.end.loc);
}

void engine::switchPlayer()
{
  b.switchPlayer();
}

void engine::run()
{
  reset();

  while (true) {
    move m = getMove();
    doMove(m);
    switchPlayer();

    if (b.gameOver()) {
      std::cout << (b.inCheck() ? "Checkmate" : "Stalemate") << "\n";
      sleep(5);
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  engine chess(true, 800, 800);
  chess.run();
  return 0;
}
