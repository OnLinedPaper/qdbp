#include "movable.h"
#include "debug_draw.h"
#include "render.h"
#include "vec2d/vec2d.h"
#include <iostream>

d_draw::d_draw() {
  pos = vec2d(0, 0);
  vel = vec2d(0, 0);
}
void d_draw::draw() const {
  SDL_Rect r;
  r.x = this->pos[0];
  r.y = this->pos[1];
  r.w = 10;
  r.h = 10;
  SDL_RenderFillRect(render::get().get_renderer(), &r);
}
