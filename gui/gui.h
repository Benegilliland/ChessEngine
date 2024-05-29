#pragma once
#include <SDL2/SDL.h>
#include "bg.h"
#include "eventhandler.h"
#include "spritehandler.h"
#include "../common.h"

class g_gui {
private:
  SDL_Renderer *renderer;
  g_bg bg;
  g_spritehandler spritehandler;
  g_eventhandler eventhandler;

public:
  g_gui(SDL_Renderer *, int, int);
  ~g_gui();
  void draw();
  start_pos getStartPos();
};
