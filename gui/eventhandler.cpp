#include "eventhandler.h"
#include <iostream>

g_eventhandler::g_eventhandler(int _width, int _height)
	:  width(_width), height(_height) {;}

s_pos g_eventhandler::mouseToBoard(const SDL_Event &e)
{
	SDL_Point mousePos = {e.button.x, e.button.y};
	return {8 * mousePos.x / width, 8 * mousePos.y / height};
}

void g_eventhandler::checkQuit(const SDL_Event &e)
{
	if (e.type == SDL_QUIT)
		exit(1);
}

bool g_eventhandler::getStartPos(s_pos &p, SDL_Point &pt)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) > 0) {
		checkQuit(e);

		if (e.type == SDL_MOUSEBUTTONDOWN) {
			pt = {e.button.x - width / 16, e.button.y - height / 16};
			p = mouseToBoard(e);
			std::cout << "Mouse down\n";
			return true;
		}
	}

	return false;
}

bool g_eventhandler::getEndPos(s_pos &p, SDL_Point &pt)
{
	SDL_Event e;
	pt = {0, 0};
	while (SDL_PollEvent(&e) > 0) {
		checkQuit(e);
		if (e.type == SDL_MOUSEMOTION) {
			pt.x += e.motion.xrel;
			pt.y += e.motion.yrel;
		}

		else if (e.type == SDL_MOUSEBUTTONUP) {
			p = mouseToBoard(e);
			std::cout << "Mouse up\n";
			return true;
		}

	}

	return false;
}

bool g_eventhandler::getMenuPiece(const s_pos &p, piece &pc, const SDL_Event &e)
{
	s_pos clickPos = mouseToBoard(e);
	if (clickPos.rank != p.rank) return false;

	if (p.file == 0) {
		if (clickPos.file < 1 || clickPos.file > 4) return false;
		if (clickPos.file == 1) pc = piece::queen;
		else if (clickPos.file == 2) pc = piece::rook;
		else if (clickPos.file == 3) pc = piece::knight;
		else pc = piece::bishop;
		return true;
	}
	else {
		if (clickPos.file > 6 || clickPos.file < 3) return false;
		if (clickPos.file == 6) pc = piece::queen;
		else if (clickPos.file == 5) pc = piece::rook;
		else if (clickPos.file == 4) pc = piece::knight;
		else pc = piece::bishop;
		return true;
	}
}

bool g_eventhandler::getPawnUpgrade(const s_pos &p, piece &pc)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) > 0) {
		checkQuit(e);
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			return getMenuPiece(p, pc, e);
		}
	}

	return false;
}
