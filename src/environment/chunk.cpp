#include "chunk.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"

chunk::chunk(vec2d v) :
  length(1000),
  tlc(v),
  border {0, 0, 0, 0},
  i("/boundary_marker")
  { }

chunk::chunk(double x, double y) :
  length(1000),
  tlc(x*length, y*length),
  border {0, 0, 0, 0},
  i("/boundary_marker")
  { }

chunk::chunk(vec2d v, bool u, bool d, bool l, bool r) :
  length(1000),
  tlc(v),
  border {u, d, l, r},
  i("/boundary_marker")
  { }

chunk::chunk(double x, double y, bool u, bool d, bool l, bool r) :
  length(1000),
  tlc(x*length, y*length),
  border {u, d, l, r},
  i("/boundary_marker")
  { }

void chunk::draw() const {

  double x = this->tlc[0] - viewport::get().get_tlc_x();
  double y = this->tlc[1] - viewport::get().get_tlc_y();
  //debug_draw(x, y);
  //draw 2 barriers on each line where border[i] = true -
  //that's where a border is
  //check in order: u, d, l, r
  if(border[0]) {
    //the top is a border
    i.draw_rotate(x + (length / 4), y, 0);
    i.draw_rotate(x + 3 * (length / 4), y, 0);
  }
  if(border[1]) {
    //the bottom is a border
    i.draw_rotate(x + (length / 4), y + length, 180);
    i.draw_rotate(x + 3 * (length / 4), y + length, 180);
  }
  if(border[2]) {
    //the left is a border
    i.draw_rotate(x, y + (length / 4), 270);
    i.draw_rotate(x, y + 3 * (length / 4), 270);
  }
  if(border[3]) {
    //the right is a border
    i.draw_rotate(x + length, y + (length / 4), 90);
    i.draw_rotate(x + length, y + 3 * (length / 4), 90);
  }

}

void chunk::debug_draw(double x, double y) const {
  SDL_Rect r;
  r.x = x;
  r.y = y;
  r.w = r.h = length;



  //save color
  SDL_Color c;
  SDL_GetRenderDrawColor(render::get().get_r(), &(c.r), &(c.g), &(c.b), &(c.a));

  SDL_SetRenderDrawColor(render::get().get_r(), 200, 200, 200, 255);

  //draw rect
  SDL_RenderFillRect(render::get().get_r(), &r);
  SDL_SetRenderDrawColor(render::get().get_r(), 64, 255, 64, 255);
  SDL_RenderDrawRect(render::get().get_r(), &r);

  //draw borders
  //for debugging purposes, make border walls red and passable walls green
  SDL_SetRenderDrawColor(render::get().get_r(), 255, 64, 64, 255);
  if(border[0]) {
    SDL_RenderDrawLine(render::get().get_r(), x, y, x+length, y);
  }
  if(border[1]) {
    SDL_RenderDrawLine(render::get().get_r(), x, y+length, x+length, y+length);
  }
  if(border[2]) {
    SDL_RenderDrawLine(render::get().get_r(), x, y, x, y+length);
  }
  if(border[3]) {
    SDL_RenderDrawLine(render::get().get_r(), x+length, y, x+length, y+length);
  }



  //restore color
  SDL_SetRenderDrawColor(render::get().get_r(), c.r, c.g, c.b, c.a);

}
