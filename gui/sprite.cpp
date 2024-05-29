#include "sprite.h"

void g_sprite::create(SDL_Renderer *_renderer, SDL_Texture *_texture, SDL_Rect *_pos)
{
  renderer = _renderer;
  texture = _texture;
  pos = *_pos;
}

void g_sprite::setSrc(SDL_Rect *_src)
{
  src = _src;
}

void g_sprite::draw()
{
  if (src)
    SDL_RenderCopy(renderer, texture, src, &pos);
}
