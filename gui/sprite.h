#pragma once
#include <SDL2/SDL.h>

class g_sprite {
private:
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  SDL_Rect *src;
  SDL_Rect pos;

public:
  void create(SDL_Renderer *, SDL_Texture *,  SDL_Rect*);
  void setSrc(SDL_Rect *);
  SDL_Rect *getSrc();
  void setPos(const SDL_Point &);
  void move(const SDL_Point &);
  void draw();
};
