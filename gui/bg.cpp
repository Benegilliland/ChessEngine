#include "bg.h"

g_bg::g_bg(SDL_Renderer *_renderer, int width, int height) : renderer(_renderer) {
  int tile_width = width / 8;
  int tile_height = height / 8;

  for (int i = 0; i < 64; i++) {
    int tile_x = (i % 8) * tile_width;
    int tile_y = (7 - (i / 8)) * tile_height;
    int pos_index = i / 2;
    int color_index = (i + i / 8) % 2;

    tiles[color_index][pos_index] = SDL_Rect{tile_x, tile_y, tile_width, tile_height};
  }
}

void g_bg::draw()
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, colors[dark].r, colors[dark].g, colors[dark].b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(renderer, tiles[dark], NUM_SQUARES);

  SDL_SetRenderDrawColor(renderer, colors[light].r, colors[light].g, colors[light].b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(renderer, tiles[light], NUM_SQUARES);
}
