#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprite.h"
#include "../common.h"

class g_spritehandler {
private:
  const char *spritesheet_file = "images/spritesheet.png";
  static const int NUM_RANKS = 8;
  static const int NUM_FILES = 8;
  static const int NUM_UPGRADES = 4;
  SDL_Texture *spritesheet;
  SDL_Rect piece_sources[NUM_SIDES][NUM_PIECES];
  SDL_Renderer *renderer;

  g_sprite sprites[NUM_RANKS][NUM_FILES];
  g_sprite dragSprite;
  s_pos dragStartPos;

  int tile_width, tile_height;

  bool showUpgradeMenu;
  SDL_Rect pawnUpgradeTiles[NUM_UPGRADES];
  g_sprite pawnUpgradePieces[NUM_UPGRADES];

  void loadSpritesheet(SDL_Renderer *);
  void createSprites(SDL_Renderer *, int, int);

public:
  g_spritehandler(SDL_Renderer *, int, int);
  void draw();
  void reset();
  void setPiece(int, piece, side);
  void doMove(const s_pos &, const s_pos &);
  void startDragging(const s_pos &, const SDL_Point &);
  void stopDragging();
  void moveDragPiece(const SDL_Point &);
  void pawnUpgrade(const s_pos &, piece);
  void enPassant(const s_pos &);
  void queensideCastle(const s_pos &);
  void kingsideCastle(const s_pos &);
  void showPawnUpgrade(const s_pos &);
  void hidePawnUpgrade(const s_pos &);
};
