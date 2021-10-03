#include "chunk.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"
#include "src/image/image_handler.h"
#include "src/xml_parser/xmlparse.h"

const unsigned char chunk::IN = 0;
const unsigned char chunk::UP = 1;
const unsigned char chunk::DN = 2;
const unsigned char chunk::LF = 4;
const unsigned char chunk::RT = 8;
const float chunk::length = 1000;

chunk::chunk(vec2d &v) :
  tlc(v),
  border {0, 0, 0, 0},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(float x, float y) :
  tlc(x*length, y*length),
  border {0, 0, 0, 0},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(vec2d &v, bool u, bool d, bool l, bool r) :
  tlc(v),
  border {u, d, l, r},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(float x, float y, bool u, bool d, bool l, bool r) :
  tlc(x*length, y*length),
  border {u, d, l, r},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
  { }

chunk::chunk(float x, float y, bool u, bool d, bool l, bool r, std::string type) :
  tlc(x*length, y*length),
  border {u, d, l, r},
  i1_name("/boundary_marker"),
  i2_name("/boundary_point"),
  frame_bump(rand()),
  in_bounds(false),
  has_gate(false),
  g_dest(""),
  g_name("")
{
  if(type != "") {
    //load the chunk data here
    in_bounds = xmlparse::get().get_xml_bool("/chunk_types/"+type+"/in_bounds");
  }
  else {
    in_bounds = true;
  }
}

chunk::chunk(const chunk&c) :
  tlc(c.tlc),
  i1_name(c.i1_name),
  i2_name(c.i2_name),
  frame_bump(c.frame_bump),
  in_bounds(c.in_bounds)
{
  this->set_b_up(c.is_b_up());
  this->set_b_dn(c.is_b_dn());
  this->set_b_lf(c.is_b_lf());
  this->set_b_rt(c.is_b_rt());
}

chunk &chunk::operator=(const chunk& c) {
  //check if these are the same chunk
  if(this == &c) { return *this; }

  this->set_b_up(c.is_b_up());
  this->set_b_dn(c.is_b_dn());
  this->set_b_lf(c.is_b_lf());
  this->set_b_rt(c.is_b_rt());

  this->tlc = c.tlc;

  this->i1_name = c.i1_name;
  this->i2_name = c.i2_name;
  this->frame_bump = c.frame_bump;

  this->in_bounds = c.in_bounds;

  return *this;
}


unsigned char chunk::chunk_pos(vec2d &v) const {
  return( this->chunk_pos(v[0], v[1]) );
}

unsigned char chunk::chunk_pos(float x_coord, float y_coord) const {
  unsigned char retval = 0;
  //return 0 if it's in this chunk - otherwise, use bitwise
  //OR to add the location qualifiers

  if(x_coord <= tlc[0]) {
    //it's to the left
    retval = retval|LF;
  }
  if(x_coord >= tlc[0] + length) {
    //to the right
    retval = retval|RT;
  }
  if(y_coord <= tlc[1]) {
    //above
    retval = retval|UP;
  }
  if(y_coord >= tlc[1] + length) {
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

unsigned char chunk::chunk_rebuff_forced(vec2d &pos) const {
  //force a rebuff - this is used to handle issues where it would otherwise
  //be possible to fly into out of bounds chunks

  unsigned char c = chunk_pos(pos);
  unsigned char ret = 0;
  if(!c) {
    //still in chunk, no check
    return ret;
  }

  //check each "hit" to see if it's out of bounds - ignore barriers or not
  //rebuff it in that direction
  if(c & UP) { ret = ret|UP; }
  if(c & DN) { ret = ret|DN; }
  if(c & LF) { ret = ret|LF; }
  if(c & RT) { ret = ret|RT; }

  return ret;
}

void chunk::add_gate(std::string dest, std::string name) {
  has_gate = true;
  g_dest = dest;
  g_name = "/" + name;
}

void chunk::spawn_initial_entities() {
  return;
}

void chunk::draw() const {

  float x = tlc[0];
  float y = tlc[1];
  //debug_draw(x, y);
  //draw 2 barriers on each line where border[i] = true -
  //that's where a border is
  //check in order: u, d, l, r
  if(border[0]) {
    //the top is a border
    image_handler::get().draw_rotate(i1_name, x + (length / 4), y, frame_bump, 0);
    image_handler::get().draw_rotate(i1_name, x + 3 * (length / 4), y, frame_bump, 0);

    image_handler::get().draw_rotate(i2_name, x, y, 0, 0);
    image_handler::get().draw_rotate(i2_name, x + (length / 2), y, 0, 0);
  }
  if(border[1]) {
    //the bottom is a border
    image_handler::get().draw_rotate(i1_name, x + (length / 4), y + length, frame_bump, 180);
    image_handler::get().draw_rotate(i1_name, x + 3 * (length / 4), y + length, frame_bump, 180);

    image_handler::get().draw_rotate(i2_name, x + (length / 2), y + length, 0, 0);
    image_handler::get().draw_rotate(i2_name, x + length, y + length, 0, 0);
  }
  if(border[2]) {
    //the left is a border
    image_handler::get().draw_rotate(i1_name, x, y + (length / 4), frame_bump, 270);
    image_handler::get().draw_rotate(i1_name, x, y + 3 * (length / 4), frame_bump, 270);

    image_handler::get().draw_rotate(i2_name, x, y + (length / 2), 0, 0);
    image_handler::get().draw_rotate(i2_name, x, y + length, 0, 0);
  }
  if(border[3]) {
    //the right is a border
    image_handler::get().draw_rotate(i1_name, x + length, y + (length / 4), frame_bump, 90);
    image_handler::get().draw_rotate(i1_name, x + length, y + 3 * (length / 4), frame_bump, 90);

    image_handler::get().draw_rotate(i2_name, x + length, y, 0, 0);
    image_handler::get().draw_rotate(i2_name, x + length, y + (length / 2), 0, 0);
  }

  if(has_gate) {
    image_handler::get().draw_rotate(g_name, x + length/2, y + length/2, 0, 0);
  }

}

void chunk::debug_draw(float x, float y) const {
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
