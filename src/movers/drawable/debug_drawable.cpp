#include "debug_drawable.h"
#include "src/renders/render.h"
#include "src/vec2d/vec2d.h"
#include "src/xml_parser/xmlparse.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"
#include "src/timeframe/timeframe.h"
#include <iostream>
#include <SDL2/SDL.h>

const std::string d_drawable::path = "/movers/drawable/debug_drawable";

using xmlp = xmlparse;

d_drawable::d_drawable() :
  drawable("/" + xmlp::get().get_xml_string(path + "/textures/body")),
  last_angle(0)
{
  pos = vec2d(0, 0);
  vel = vec2d(0, 0);


  vel_accel = xmlp::get().get_xml_double(path + "/movement/vel_accel");
  vel_cap = xmlp::get().get_xml_double(path + "/movement/vel_cap");
  vel_decay = xmlp::get().get_xml_double(path + "/movement/vel_decay");
  moved = false;

  std::string s = path + xmlp::get().get_xml_string(path + "/textures/body");
}

void d_drawable::move_up() 
  { vel[1] -= vel_accel * t_frame::get().d_factor(); moved = true; }
void d_drawable::move_dn()
  { vel[1] += vel_accel * t_frame::get().d_factor(); moved = true; }
void d_drawable::move_lf()
  { vel[0] -= vel_accel * t_frame::get().d_factor(); moved = true; }
void d_drawable::move_rt()
  { vel[0] += vel_accel * t_frame::get().d_factor(); moved = true; }

void d_drawable::update() {
  drawable::update();

  //cap velocity
  vel = vel.cap(vel_cap);

  //move, based on velocity AND time since last frame -
  //this prevents lag spikes from "slowing time"
  pos[0] += vel[0] * t_frame::get().d_factor();
  pos[1] += vel[1] * t_frame::get().d_factor();

  if(moved == false) {
    //decay velocity only when user hasn't moved
    vel = vel.decay(vel_decay * t_frame::get().d_factor());
  }

//recompute the angle the ship will be drawn at, smoothing it out for sudden
//changes in direction
  if(vel.magnitude() != 0) {
    //smooth-spin the image
    double threshold = 15;
    //if the angle change is greater than threshold, turn threshold at a time
    double a = vel.angle_deg();

    //add 360 so there's no issue with dropping from 0 to 360, this
    //will later be modded out
    a += 360;
    last_angle += 360;

    //check that the angle is greater than some threshold degrees, but also
    //check for wrap-around from 360 to 0
    if(abs(a - last_angle) > threshold &&
      abs(abs(a- last_angle) - 360) > threshold)
    {
      //steep turn - change it by finding which way to turn
      if((int(a - last_angle) + 360) % 360 > 180) {
        last_angle -= threshold;
      }
      else {
        last_angle += threshold;
      }
    }
    else {
      last_angle = a;
    }
  }
  last_angle = int(last_angle) % 360;

  //reset moved
  moved = false;
}

void d_drawable::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  i.draw_rotate(dest_r.x, dest_r.y, last_angle);
  //SDL_RenderCopy(render::get().get_r(), t, NULL, &dest_r);
}
