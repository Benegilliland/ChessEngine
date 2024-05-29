#include "gui.h"
#include <unistd.h>

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

start_pos g_gui::getStartPos() {
  start_pos p;

  while (true) {
    if (eventhandler.getStartPos(p))
	    return p;
    draw();
    usleep(10000);    
  }
}
