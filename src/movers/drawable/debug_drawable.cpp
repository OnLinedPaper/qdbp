#include "debug_drawable.h"
#include "src/renders/render.h"
#include "src/vec2d/vec2d.h"
#include "src/xml_parser/xmlparse.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"
#include "src/timeframe/timeframe.h"
#include <iostream>
#include <SDL2/SDL.h>

using xmlp = xmlparse;

d_drawable::d_drawable() : moved(false), i("/arrow"), last_angle(0) {
  pos = vec2d(0, 0);
  vel = vec2d(0, 0);

  path = "/movers/drawable/debug_drawable";

  vel_accel = xmlp::get().get_xml_double(path + "/movement/vel_accel");
  vel_cap = xmlp::get().get_xml_double(path + "/movement/vel_cap");
  vel_decay = xmlp::get().get_xml_double(path + "/movement/vel_decay");

  load_texture();


}

void d_drawable::load_texture() {

  //load the texture
  set_texture(NULL);
  SDL_Surface *surf = IMG_Load((xmlp::get().get_xml_string(path + "/textures/body")).c_str());
  if(surf == NULL) {
    fprintf(stderr, "couldn't load image! SDL_Error: %s\n", SDL_GetError());
  }
  else {
    t = SDL_CreateTextureFromSurface(render::get().get_r(), surf);
    if(t == NULL) {
      fprintf(stderr, "couldn't create texture! SDL_Error: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(surf);
  }
}

void d_drawable::move_up() { vel[1] -= vel_accel * t_frame::get().d_factor(); moved = true; }
void d_drawable::move_down() { vel[1] += vel_accel * t_frame::get().d_factor(); moved = true; }
void d_drawable::move_left() { vel[0] -= vel_accel * t_frame::get().d_factor(); moved = true; }
void d_drawable::move_right() { vel[0] += vel_accel * t_frame::get().d_factor(); moved = true; }

void d_drawable::update() {

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
    //if the angle change is greater than 30, turn 30 at a time
    double a = vel.angle_deg();

    //add 360 so there's no issue with dropping from 0 to 360, this
    //will later be modded out
    a += 360;
    last_angle += 360;

    if(abs(a - last_angle) > 15) {
      //steep turn - change it by finding which way to turn
      if((int(a - last_angle) + 360) % 360 > 180) {
        last_angle -= 15;
      }
      else {
        last_angle += 15;
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
