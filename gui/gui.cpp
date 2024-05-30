#include "gui.h"
#include <unistd.h>
#include <iostream>

g_gui::g_gui(SDL_Renderer *_renderer, int width, int height)
  : renderer(_renderer), bg(_renderer, width, height), spritehandler(_renderer, width, height),
	eventhandler(width, height)
{
  spritehandler.reset();
}

g_gui::~g_gui()
{
  ;
}

void g_gui::draw()
{
  bg.draw();
  spritehandler.draw();
  SDL_RenderPresent(renderer);
}

s_pos g_gui::getStartPos() {
  s_pos p;
  SDL_Point pt;

  while (true) {
    if (eventhandler.getStartPos(p, pt)) {
      spritehandler.startDragging(p, pt);
      return p;
    }

    draw();
    usleep(sleep_duration);
  }
}

s_pos g_gui::getEndPos() {
	s_pos p;
	SDL_Point pt;

	while (true) {
		bool result = eventhandler.getEndPos(p, pt);
		spritehandler.moveDragPiece(pt);
		if (result) {
			stopDragging();
			return p;
		}

		draw();
		usleep(sleep_duration);
	}
}

s_pos g_gui::bitboardPosition(u64 b)
{
	int n = 0;

	while (!(b & 1)) {
		b >>= 1;
		n++;
	}

	return {n % 8, 7 - n / 8};
}

void g_gui::doMove(u64 start, u64 end)
{
	s_pos startPos = bitboardPosition(start);
	s_pos endPos = bitboardPosition(end);

	spritehandler.doMove(startPos, endPos);
}

void g_gui::stopDragging()
{
	bg.unhighlightSquares();
	spritehandler.stopDragging();
}

void g_gui::showAvailableMoves(u64 bitboard)
{
	bg.highlightSquares(bitboard);
}

void g_gui::doPawnUpgrade(u64 bitboard, piece pc)
{
	s_pos pos = bitboardPosition(bitboard);
	spritehandler.pawnUpgrade(pos, pc);
}

void g_gui::doEnPassant(u64 end)
{
	s_pos pos = bitboardPosition(end);
	spritehandler.enPassant(pos);
}

void g_gui::doQueensideCastle(u64 end)
{
	s_pos pos = bitboardPosition(end);
	spritehandler.queensideCastle(pos);
}

void g_gui::doKingsideCastle(u64 end)
{
	s_pos pos = bitboardPosition(end);
	spritehandler.kingsideCastle(pos);
}

piece g_gui::getPawnUpgrade()
{
	return piece::queen;
}
