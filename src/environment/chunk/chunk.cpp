#include "chunk.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"
#include "src/image/image_handler.h"

const unsigned char chunk::IN = 0;
const unsigned char chunk::UP = 1;
const unsigned char chunk::DN = 2;
const unsigned char chunk::LF = 4;
const unsigned char chunk::RT = 8;
const double chunk::length = 1000;

chunk::chunk(vec2d &v) :
  tlc(v),
  border {0, 0, 0, 0},
  i_name("/boundary_marker"),
  frame_bump(rand())
  { }

chunk::chunk(double x, double y) :
  tlc(x*length, y*length),
  border {0, 0, 0, 0},
  i_name("/boundary_marker"),
  frame_bump(rand())
  { }

chunk::chunk(vec2d &v, bool u, bool d, bool l, bool r) :
  tlc(v),
  border {u, d, l, r},
  i_name("/boundary_marker"),
  frame_bump(rand())
  { }

chunk::chunk(double x, double y, bool u, bool d, bool l, bool r) :
  tlc(x*length, y*length),
  border {u, d, l, r},
  i_name("/boundary_marker"),
  frame_bump(rand())
  { }

unsigned char chunk::chunk_pos(vec2d &v) const {
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

unsigned char chunk::chunk_rebuff(vec2d &pos) const {
  unsigned char c = chunk_pos(pos);
  unsigned char ret = 0;
  if(!c) {
    //still in chunk, no check
    return ret;
  }

  //check each "hit" to see if it's also a barrier - if so,
  //rebuff it in that direction
  if((c & UP) && is_b_up()) { ret = ret|UP; }
  if((c & DN) && is_b_dn()) { ret = ret|DN; }
  if((c & LF) && is_b_lf()) { ret = ret|LF; }
  if((c & RT) && is_b_rt()) { ret = ret|RT; }


  return ret;
}

void chunk::draw() const {

  double x = tlc[0];
  double y = tlc[1];
  //debug_draw(x, y);
  //draw 2 barriers on each line where border[i] = true -
  //that's where a border is
  //check in order: u, d, l, r
  if(border[0]) {
    //the top is a border
    image_handler::get().draw_rotate(i_name, x + (length / 4), y, frame_bump, 0);
    image_handler::get().draw_rotate(i_name, x + 3 * (length / 4), y, frame_bump, 0);
  }
  if(border[1]) {
    //the bottom is a border
    image_handler::get().draw_rotate(i_name, x + (length / 4), y + length, frame_bump, 180);
    image_handler::get().draw_rotate(i_name, x + 3 * (length / 4), y + length, frame_bump, 180);
  }
  if(border[2]) {
    //the left is a border
    image_handler::get().draw_rotate(i_name, x, y + (length / 4), frame_bump, 270);
    image_handler::get().draw_rotate(i_name, x, y + 3 * (length / 4), frame_bump, 270);
  }
  if(border[3]) {
    //the right is a border
    image_handler::get().draw_rotate(i_name, x + length, y + (length / 4), frame_bump, 90);
    image_handler::get().draw_rotate(i_name, x + length, y + 3 * (length / 4), frame_bump, 90);
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
