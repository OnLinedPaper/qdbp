#include "rect2d.h"
#include "src/vec2d/vec2d.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"

void rect2d::draw() const {
  SDL_Rect r;
  r.x = (tlc[0] - viewport::get().get_tlc_x()) + dims[0] / 2;
  r.y = (tlc[1] - viewport::get().get_tlc_y()) + dims[1] / 2;
  r.w = dims[0];
  r.h = dims[1];


  SDL_Color c;
  SDL_GetRenderDrawColor(render::get().get_r(), &(c.r), &(c.g), &(c.b), &(c.a));

  SDL_SetRenderDrawColor(render::get().get_r(), 200, 200, 200, 128);

  //draw rect
  SDL_RenderFillRect(render::get().get_r(), &r);
  SDL_SetRenderDrawColor(render::get().get_r(), 64, 255, 64, 255);
  SDL_RenderDrawRect(render::get().get_r(), &r);


}

void rect2d::draw(int red, int green, int blue) const {
  SDL_Rect r;
  r.x = (tlc[0] - viewport::get().get_tlc_x()) + dims[0] / 2;
  r.y = (tlc[1] - viewport::get().get_tlc_y()) + dims[1] / 2;
  r.w = dims[0];
  r.h = dims[1];


  SDL_Color c;
  SDL_GetRenderDrawColor(render::get().get_r(), &(c.r), &(c.g), &(c.b), &(c.a));

  SDL_SetRenderDrawColor(render::get().get_r(), (red/2)+128, (green/2)+128, (blue/2)+128, 128);

  //draw rect
  SDL_RenderFillRect(render::get().get_r(), &r);
  SDL_SetRenderDrawColor(render::get().get_r(), red, green, blue, 255);
  SDL_RenderDrawRect(render::get().get_r(), &r);
}
