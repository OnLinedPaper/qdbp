#include "movable.h"
#include "debug_draw.h"
#include "../renders/render.h"
#include "../vec2d/vec2d.h"
#include "../xml_parser/xmlparse.h"
#include <iostream>

using xmlp = xmlparse;


d_draw::d_draw() : moved(false) {
  pos = vec2d(0, 0);
  vel = vec2d(0, 0);

  std::string path = "/movers/debug_draw";

  vel_accel = xmlp::get().get_xml_double(path + "/movement/vel_accel");
  vel_cap = xmlp::get().get_xml_double(path + "/movement/vel_cap");
  vel_decay = xmlp::get().get_xml_double(path + "/movement/vel_decay");
}

void d_draw::move_up() { vel[1] -= vel_accel; moved = true; }
void d_draw::move_down() { vel[1] += vel_accel; moved = true; }
void d_draw::move_left() { vel[0] -= vel_accel; moved = true; }
void d_draw::move_right() { vel[0] += vel_accel; moved = true; }

void d_draw::update() {

  //cap velocity
  vel = vel.cap(vel_cap);

  //move, based on velocity
  pos[0] += vel[0];
  pos[1] += vel[1];

  if(moved == false) {
    //decay velocity only when user hasn't moved
    vel = vel.decay(vel_decay);
  }

  //reset this
  moved = false;
}

void d_draw::draw() const {
  SDL_Rect r;
  r.x = this->pos[0];
  r.y = this->pos[1];
  r.w = r.h = 20;

  SDL_Color box;
  box.r = box.g = box.b = 0;
  box.a = 196;

  double mag = vel.magnitude();
  if(mag == 0) {
    box.r = 64;
  }
  else if(mag > (vel_cap - 0.05)) {
    box.g = 255;
    box.b = box.r = 128;
  }
  else {
    box.b = 255;
    box.r = box.g = 64;
  }

  //save color
  SDL_Color c;
  SDL_GetRenderDrawColor(render::get().get_renderer(), &(c.r), &(c.g), &(c.b), &(c.a));

  SDL_SetRenderDrawColor(render::get().get_renderer(), box.r, box.g, box.b, box.a);

  //draw rect
  SDL_RenderFillRect(render::get().get_renderer(), &r);

  //restore color
  SDL_SetRenderDrawColor(render::get().get_renderer(), c.r, c.g, c.b, c.a);
}
