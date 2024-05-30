#pragma once
#include <SDL2/SDL.h>
#include "bg.h"
#include "eventhandler.h"
#include "spritehandler.h"
#include "../common.h"

class g_gui {
private:
  static const int sleep_duration = 10000;
  SDL_Renderer *renderer;
  g_bg bg;
  g_spritehandler spritehandler;
  g_eventhandler eventhandler;
  s_pos bitboardPosition(u64);

public:
  g_gui(SDL_Renderer *, int, int);
  ~g_gui();
  void draw();
  s_pos getStartPos();
  void stopDragging();
  s_pos getEndPos();
  void doMove(u64, u64);
  void showAvailableMoves(u64);
};
