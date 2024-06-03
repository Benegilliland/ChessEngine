#pragma once
#include "common.h"
#include "board/b_common.h"
#include "board/board.h"
#include "gui/gui.h"
#include "ai/ai.h"

class engine {
private:
  board b;
  g_gui *gui;
  c_ai *ai;
  SDL_Window *window;
  SDL_Renderer *renderer;
  control players[NUM_SIDES];

  void init_gui(int, int);
  void destroy_gui();
  void reset();
  b_pos getStartPos();
  b_pos getEndPos();
  move getMove();
  void showAvailableMoves(const b_pos &);
  void doMove(move &);
  void switchPlayer();
  piece getPawnUpgrade(u64);

public:
  engine(control, control, bool, int, int);
  ~engine();
  void run();
};
