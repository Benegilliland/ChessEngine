#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprite.h"
#include "../common.h"

class g_spritehandler {
private:
  static constexpr char *spritesheet_file = "images/spritesheet.png";
  static const int NUM_RANKS = 8;
  static const int NUM_FILES = 8;
  SDL_Texture *spritesheet;
  SDL_Rect piece_sources[NUM_SIDES][NUM_PIECES];

  g_sprite sprites[NUM_RANKS][NUM_FILES];
  g_sprite dragSprite;

  void loadSpritesheet(SDL_Renderer *);
  void createSprites(SDL_Renderer *, int, int);

public:
  g_spritehandler(SDL_Renderer *, int, int);
  void draw();
  void reset();
  void setPiece(int, piece, side);
};