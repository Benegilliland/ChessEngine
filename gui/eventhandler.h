#include <SDL2/SDL.h>
#include "../common.h"

class g_eventhandler {
private:
  int width, height;

  void checkQuit(const SDL_Event &);

public:
  g_eventhandler(int, int);
  s_pos mouseToBoard(const SDL_Event &);
  bool getStartPos(s_pos &, SDL_Point &pt);
  bool getEndPos(s_pos &, SDL_Point &);
};
