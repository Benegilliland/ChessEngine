#include <iostream>
#include "spritehandler.h"

void g_spritehandler::createSprites(SDL_Renderer *_renderer, int width, int height)
{
  int tile_width = width / 8;
  int tile_height = height / 8;

  SDL_Rect pos = {0, 0, tile_width, tile_height};

  dragSprite.create(_renderer, spritesheet, &pos);

  for (int rank = 0; rank < NUM_RANKS; rank++) {
    for (int file = 0; file < NUM_FILES; file++) {
      pos.x = tile_width * rank;
      pos.y = tile_height * (7 - file);

      sprites[rank][file].create(_renderer, spritesheet, &pos);
    }
  }
}

void g_spritehandler::reset()
{
  dragSprite.setSrc(nullptr);

  for (int rank = 0; rank < 8; rank++) {
    for (int file = 2; file <= 6; file++) {
      sprites[rank][file].setSrc(nullptr);
    }

    sprites[rank][1].setSrc(&piece_sources[side::white][piece::pawn]);
    sprites[rank][6].setSrc(&piece_sources[side::black][piece::pawn]);
  }

  sprites[0][0].setSrc(&piece_sources[side::white][piece::rook]);
  sprites[7][0].setSrc(&piece_sources[side::white][piece::rook]);
  sprites[0][7].setSrc(&piece_sources[side::black][piece::rook]);
  sprites[7][7].setSrc(&piece_sources[side::black][piece::rook]);

  sprites[1][0].setSrc(&piece_sources[side::white][piece::knight]);
  sprites[6][0].setSrc(&piece_sources[side::white][piece::knight]);
  sprites[1][7].setSrc(&piece_sources[side::black][piece::knight]);
  sprites[6][7].setSrc(&piece_sources[side::black][piece::knight]);

  sprites[2][0].setSrc(&piece_sources[side::white][piece::bishop]);
  sprites[5][0].setSrc(&piece_sources[side::white][piece::bishop]);
  sprites[2][7].setSrc(&piece_sources[side::black][piece::bishop]);
  sprites[5][7].setSrc(&piece_sources[side::black][piece::bishop]);

  sprites[3][0].setSrc(&piece_sources[side::white][piece::queen]);
  sprites[3][7].setSrc(&piece_sources[side::black][piece::queen]);

  sprites[4][0].setSrc(&piece_sources[side::white][piece::king]);
  sprites[4][7].setSrc(&piece_sources[side::black][piece::king]);
}

void g_spritehandler::setPiece(int pos, piece pc, side s)
{
  int rank = pos % 8;
  int file = pos / 8;

  sprites[rank][file].setSrc(&piece_sources[s][pc]);
}

void g_spritehandler::loadSpritesheet(SDL_Renderer *_renderer)
{
  spritesheet = IMG_LoadTexture(_renderer, spritesheet_file);

  if (!spritesheet) {
    std::cout << "Error: couldn't load spritesheet\n";
    exit(1);
  }

  int spritesheet_width, spritesheet_height;

  if (SDL_QueryTexture(spritesheet, NULL, NULL, &spritesheet_width, &spritesheet_height) != 0) {
    std::cout << "Error: couldn't query texture\n";
    exit(1);
  }

  int sprite_width = spritesheet_width / 6;
  int sprite_height = spritesheet_height / 2;

  for (int side = 0; side < NUM_SIDES; side++) {
    for (int piece = 0; piece < NUM_PIECES; piece++) {
      piece_sources[side][piece] = {sprite_width * piece, sprite_height * side, sprite_width, sprite_height};
    }
  }
}

g_spritehandler::g_spritehandler(SDL_Renderer *_renderer, int width, int height)
{
  loadSpritesheet(_renderer);
  createSprites(_renderer, width, height);
  reset();
}

void g_spritehandler::draw() {
  for (int rank = 0; rank < NUM_RANKS; rank++)
    for (int file = 0; file < NUM_FILES; file++)
      sprites[rank][file].draw();

  dragSprite.draw();
}

void g_spritehandler::doMove(const s_pos &start, const s_pos &end)
{
	SDL_Rect *attacker = sprites[start.rank][start.file].getSrc();
	sprites[end.rank][end.file].setSrc(attacker);
	sprites[start.rank][start.file].setSrc(nullptr);
}

void g_spritehandler::startDragging(const s_pos &p, const SDL_Point &pt)
{
	SDL_Rect *src = sprites[p.rank][7 - p.file].getSrc();
	sprites[p.rank][7 - p.file].setSrc(nullptr);
	dragSprite.setSrc(src);
	dragSprite.setPos(pt);
	dragStartPos = p;
}

void g_spritehandler::moveDragPiece(const SDL_Point &pt)
{
	dragSprite.move(pt);
}

void g_spritehandler::stopDragging()
{
	SDL_Rect *pc = dragSprite.getSrc();
	dragSprite.setSrc(nullptr);
	sprites[dragStartPos.rank][7 - dragStartPos.file].setSrc(pc);
}
