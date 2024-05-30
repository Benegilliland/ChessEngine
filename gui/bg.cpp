#include "bg.h"

g_bg::g_bg(SDL_Renderer *_renderer, int width, int height) : renderer(_renderer) {
  tile_width = width / 8;
  tile_height = height / 8;

  for (int i = 0; i < 64; i++) {
    int tile_x = (i % 8) * tile_width;
    int tile_y = (7 - i / 8) * tile_height;
    int pos_index = i / 2;
    int color_index = (i + i / 8) % 2;

    tiles[color_index][pos_index] = SDL_Rect{tile_x, tile_y, tile_width, tile_height};
  }
}

void g_bg::highlightSquares(u64 bitboard)
{
	int counter = 0;

	while (bitboard > 0) {
		if (bitboard & 1) {
			int tile_x = (counter % 8) * tile_width;
			int tile_y = (counter / 8) * tile_height;
			//int pos_index = counter / 2;
			int color_index = (counter + counter / 8) % 2;

			highlighted_tiles[color_index].push_back(SDL_Rect{tile_x, tile_y, tile_width, tile_height});
		}

		bitboard >>= 1;
		counter++;
	}
}

void g_bg::unhighlightSquares()
{
	highlighted_tiles[light] = std::vector<SDL_Rect>();
	highlighted_tiles[dark] = std::vector<SDL_Rect>();
}

void g_bg::draw()
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, colors[dark][unhighlighted].r,
colors[dark][unhighlighted].g, colors[dark][unhighlighted].b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(renderer, tiles[dark], NUM_SQUARES);

  SDL_SetRenderDrawColor(renderer, colors[light][unhighlighted].r,
colors[light][unhighlighted].g, colors[light][unhighlighted].b, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRects(renderer, tiles[light], NUM_SQUARES);

  if (!highlighted_tiles[dark].empty()) {
	SDL_SetRenderDrawColor(renderer, colors[dark][highlighted].r,
colors[dark][highlighted].g, colors[dark][highlighted].b, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRects(renderer, highlighted_tiles[dark].data(), highlighted_tiles[dark].size());
  }

  if (!highlighted_tiles[light].empty()) {
	SDL_SetRenderDrawColor(renderer, colors[light][highlighted].r,
colors[light][highlighted].g, colors[light][highlighted].b, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRects(renderer, highlighted_tiles[light].data(), highlighted_tiles[light].size());
  }
}
