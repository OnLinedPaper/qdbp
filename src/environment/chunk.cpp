#include "chunk.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"

const unsigned char chunk::IN = 0;
const unsigned char chunk::UP = 1;
const unsigned char chunk::DN = 2;
const unsigned char chunk::LF = 4;
const unsigned char chunk::RT = 8;


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

unsigned char chunk::chunk_pos(vec2d v) const {
  return( this->chunk_pos(v[0], v[1]) );
}

unsigned char chunk::chunk_pos(double x_coord, double y_coord) const {
  unsigned char retval = 0;
  //return 0 if it's in this chunk - otherwise, use bitwise
  //OR to add the location qualifiers

  if(x_coord < tlc[0]) {
    //it's to the left
    retval = retval|LF;
  }
  if(x_coord > tlc[0] + length) {
    //to the right
    retval = retval|RT;
  }
  if(y_coord < tlc[1]) {
    //above
    retval = retval|UP;
  }
  if(y_coord > tlc[1] + length) {
    //below
    retval = retval|DN;
  }

  return retval;
}

void chunk::draw() const {

  double x = tlc[0] - viewport::get().get_tlc_x();
  double y = tlc[1] - viewport::get().get_tlc_y();
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
