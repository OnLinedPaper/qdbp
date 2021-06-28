#include "debug_drawable.h"
#include "src/renders/render.h"
#include "src/vec2d/vec2d.h"
#include "src/xml_parser/xmlparse.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include <iostream>
#include <SDL2/SDL.h>

const std::string d_drawable::path = "/movers/drawable/debug_drawable";

using xmlp = xmlparse;

d_drawable::d_drawable() :
  drawable("/" + xmlp::get().get_xml_string(path + "/textures/body"))
{
  pos = vec2d(0, 0);
  vel = vec2d(0, 0);


  vel_accel = xmlp::get().get_xml_float(path + "/movement/vel_accel");
  vel_cap = xmlp::get().get_xml_float(path + "/movement/vel_cap");
  vel_decay = xmlp::get().get_xml_float(path + "/movement/vel_decay");
  moved = false;

  std::string s = path + xmlp::get().get_xml_string(path + "/textures/body");
}

void d_drawable::move_up()
  { vel[1] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_drawable::move_dn()
  { vel[1] += vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_drawable::move_lf()
  { vel[0] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_drawable::move_rt()
  { vel[0] += vel_accel * t_frame::get().t_adjust(); moved = true; }

void d_drawable::update() {
  drawable::update();

  //cap velocity
  vel = vel.cap(vel_cap);

  //move, based on velocity AND time since last frame -
  //this prevents lag spikes from "slowing time"
  pos[0] += vel[0] * t_frame::get().t_adjust();
  pos[1] += vel[1] * t_frame::get().t_adjust();

  if(moved == false) {
    //decay velocity only when user hasn't moved
    vel = vel.decay(vel_decay * t_frame::get().t_adjust());
  }
  //reset moved
  moved = false;
}

void d_drawable::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate(image_name, dest_r.x, dest_r.y, 0, last_angle);
}
