#pragma once
#include "common.h"
#include "board/b_common.h"
#include "board/board.h"
#include "gui/gui.h"

class engine {
private:
  board b;
  g_gui *gui;
  SDL_Window *window;
  SDL_Renderer *renderer;

  void init_gui(int, int);
  void destroy_gui();
  void reset();
  b_pos getStartPos();
  b_pos getEndPos();
  move getMove();
  void calcDiff(move &);
  void showAvailableMoves(const b_pos &);
  void doMove(move &);
  void switchPlayer();

public:
  engine(bool, int, int);
  ~engine();
  void run();
};
