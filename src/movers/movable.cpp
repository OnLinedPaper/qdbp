#include "movable.h"

movable::movable(std::string path) :
  pos(0,0),
  last_pos(0,0),
  vel(0,0),
  vel_accel(xmlparse::get().get_xml_float(path + "/movement/vel_accel") * t_frame::get().t_factor()),
  vel_cap(xmlparse::get().get_xml_float(path + "/movement/vel_cap") * t_frame::get().t_factor()),
  vel_overcap(xmlparse::get().safe_get_xml_float(path + "/movement/vel_overcap")),
  vel_decay(xmlparse::get().get_xml_float(path + "/movement/vel_decay") * t_frame::get().t_factor()),
  moved(false),
  boosted(false),
  active(true)
{ }

movable::movable(std::string path, const vec2d p, const vec2d v) :
  movable(path)
{ pos = p; vel = v; }

void movable::rebuff(unsigned char posi) {
  
  if(posi) {
    //somewhere along the way, we tripped a barrier we shouldn't have
    if(posi & chunk::UP) {
      vel[1] = fabs(vel[1]);
    }
    if(posi & chunk::DN) {
      vel[1] = -fabs(vel[1]);
    }
    if(posi & chunk::LF) {
      vel[0] = fabs(vel[0]);
    }
    if(posi & chunk::RT) {
      vel[0] = -fabs(vel[0]);
    }

    //reset position to previous
    pos = last_pos;
  }
}

void movable::update() {
  //cap velocity
  if(boosted && (vel_overcap != 0)) {
    vel = vel.cap(vel_overcap);
  }
  else {
    vel = vel.cap(vel_cap);
  }

  //turn boost off, it must be re-enabled each time
  boosted = false;

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


  //check if this next move is legal
  rebuff(map_h::get().check_rebuff(pos, last_pos));

  //update position
  last_pos = pos;
}

void movable::draw() const { }
