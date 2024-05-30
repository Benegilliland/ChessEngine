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
    if (!validPos && gui) gui->stopDragging();
  }

  return board_pos;
}

b_pos engine::getEndPos()
{
	s_pos pos = gui ? gui->getEndPos() : b.getEndPos();
	return b.getBoardPos(pos);
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
}

void engine::showAvailableMoves(const b_pos &p)
{
  u64 moves = b.genStartMoves(p);
  b.saveStartMoves(moves);
  gui ? gui->showAvailableMoves(moves) : b.printBitboard(moves);
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
    validMove = b.validateMove(m);
  }

  b.setMoveType(m);
  return m;
}

piece engine::getPawnUpgrade()
{
	return (gui ? gui->getPawnUpgrade() : b.getPawnUpgrade());
}

void engine::doMove(move &m)
{
  b.doMove(m);
  if (gui) gui->doMove(m.start.loc, m.end.loc);

  piece pc = piece::none;

  switch (m.type) {
	case move_type::queenside_castle:
		b.doQueensideCastle(m);
		if (gui) gui->doQueensideCastle(m.end.loc);
		break;
	case move_type::kingside_castle:
		b.doKingsideCastle(m);
		if (gui) gui->doKingsideCastle(m.end.loc);
		break;
	case move_type::en_passant:
		b.doEnPassant(m);
		if (gui) gui->doEnPassant(m.end.loc);
		break;
	case move_type::pawn_upgrade:
		pc = getPawnUpgrade();
		b.doPawnUpgrade(m, pc);
		if (gui) gui->doPawnUpgrade(m.end.loc, pc);
		break;
	default:
		break;
  }
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
