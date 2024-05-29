#include <SDL2/SDL.h>

enum bg_squares : int {light, dark};

class g_bg {
private:
  static const int NUM_COLORS = 2;
  static const int NUM_SQUARES = 32;

  SDL_Renderer *renderer;

  static constexpr SDL_Color colors[NUM_COLORS] = {
    {238, 238, 210}, {118, 150, 86}
  };

  SDL_Rect tiles[NUM_COLORS][NUM_SQUARES];

public:
  g_bg(SDL_Renderer *, int, int);
  void draw();
};
