#include "eventhandler.h"
#include <iostream>

g_eventhandler::g_eventhandler(int _width, int _height)
	:  width(_width), height(_height) {
	eventQueue = std::queue<s_event>();
}

void g_eventhandler::setState(s_state _st) {
	st = _st;
}

g_pos g_eventhandler::mouseToBoard(const SDL_Point &pt)
{
	return g_pos{pt.x / 8, 7 - pt.y / 8};	
}

void g_eventhandler::poll()
{
  while (SDL_PollEvent(&event) > 0) {
    switch (event.type) {
      case SDL_QUIT:
        exit(1);
        break;
      case SDL_MOUSEBUTTONDOWN:
	mouseDown();
	break;
      case SDL_MOUSEBUTTONUP:
	mouseUp();
	break;
      case SDL_MOUSEMOTION:
	mouseMotion();
	break;
    }
  }
}

void g_eventhandler::mouseDown()
{
	std::cout << "Mouse down\n";
	if  (st == s_state::awaiting_move)
		startDragging();
}

void g_eventhandler::mouseUp()
{
	std::cout << "Mouse up\n";
	if (st == s_state::dragging_piece)
		stopDragging();
}

void g_eventhandler::mouseMotion()
{
	if (st == s_state::dragging_piece)
		dragPiece();
}

void g_eventhandler::startDragging()
{
	SDL_Point pt = {event.button.x, event.button.y};
	g_pos p = mouseToBoard(pt);

	eventQueue.push({event_type::start_drag, p, pt});
	st = s_state::dragging_piece;
}

void g_eventhandler::stopDragging()
{
	SDL_Point pt = {event.button.x, event.button.y};
	g_pos p = mouseToBoard(pt);

	eventQueue.push({event_type::stop_drag, p, pt});
	st = s_state::not_waiting;
}

void g_eventhandler::dragPiece()
{
	SDL_Point pt = {event.motion.xrel, event.motion.yrel};
	eventQueue.push({event_type::drag_motion, {0, 0}, pt});
}

bool g_eventhandler::getStartPos(start_pos &p)
{
	SDL_Event e;
	while (SDL_PollEvent(&e) > 0) {
		if (e.type == SDL_QUIT) {
			exit(1);
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) {
			
			return true;
		}
	}
}
