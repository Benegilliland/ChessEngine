#include "eventhandler.h"
#include <iostream>

g_eventhandler::g_eventhandler(int _width, int _height)
	:  width(_width), height(_height) {;}

s_pos g_eventhandler::mouseToBoard(const SDL_Event &e)
{
	SDL_Point mousePos = {e.button.x, e.button.y};
	return {mousePos.x / 8, 7 - mousePos.y / 8};
}

void g_eventhandler::checkQuit(const SDL_Event &e)
{
	if (e.type == SDL_QUIT)
		exit(1);
}

bool g_eventhandler::getStartPos(s_pos &p)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) > 0) {
		checkQuit(e);

		if (e.type == SDL_MOUSEBUTTONDOWN) {
			p = mouseToBoard(e);
			std::cout << "Mouse down\n";
			return true;
		}
	}

	return false;
}

bool g_eventhandler::getEndPos(s_pos &p)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) > 0) {
		checkQuit(e);

		if (e.type == SDL_MOUSEBUTTONUP) {
			p = mouseToBoard(e);
			std::cout << "Mouse up\n";
			return true;
		}
	}

	return false;
}
