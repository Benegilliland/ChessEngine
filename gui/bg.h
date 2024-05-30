#include <SDL2/SDL.h>
#include "../common.h"
#include <vector>

enum bg_squares : int {light, dark};
enum highlight : int {unhighlighted, highlighted};

class g_bg {
private:
  int tile_width, tile_height;
  static const int NUM_COLORS = 2;
  static const int NUM_SQUARES = 32;
  static const int NUM_HIGHLIGHT_TYPES = 2;

  SDL_Renderer *renderer;

  static constexpr SDL_Color colors[NUM_COLORS][NUM_HIGHLIGHT_TYPES] = {
    {{238, 238, 210}, {0, 150, 100}},
    {{118, 150, 86}, {0, 250, 200}},
  };

  SDL_Rect tiles[NUM_COLORS][NUM_SQUARES];
  std::vector<SDL_Rect> highlighted_tiles[NUM_COLORS];

public:
  g_bg(SDL_Renderer *, int, int);
  void highlightSquares(u64);
  void unhighlightSquares();
  void draw();
};
