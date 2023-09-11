#include "movable.h"
#include "src/utils/message.h"

movable::movable(std::string path) :
  id(xmlparse::get().get_xml_string(path + "/id")),
  entity_type(xmlparse::get().get_xml_string(path + "/entity_type")),
  pos(0,0),
  last_pos(0,0),
  vel(0,0),
  vel_accel(xmlparse::get().get_xml_float(path + "/movement/vel_accel") * t_frame::get().t_factor()),
  vel_accel_mod(1),
  vel_cap(xmlparse::get().get_xml_float(path + "/movement/vel_cap") * t_frame::get().t_factor()),
  vel_cap_mod(1),
//  vel_overcap(xmlparse::get().safe_get_xml_float(path + "/movement/vel_overcap")),
  vel_decay(xmlparse::get().get_xml_float(path + "/movement/vel_decay") * t_frame::get().t_factor()),
  moved(false),
  follow_thru_portals(false),
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

    if(posi & chunk::UP && posi & chunk::DN) {
      msg::get().print_warn("movable::rebuff triggered UP and DN at the same time");
    }
    if(posi & chunk::LF && posi & chunk::RT) {
      msg::get().print_warn("movable::rebuff triggered LF and RT at the same time");
    }
  }
}

void movable::update() {
  //cap velocity
  vel = vel.cap(vel_cap * vel_cap_mod);

  //move, based on velocity AND time since last frame -
  //this prevents lag spikes from "slowing time"
  pos[0] += vel[0] * t_frame::get().t_adjust() * t_frame::get().get_t_dilate();
  pos[1] += vel[1] * t_frame::get().t_adjust() * t_frame::get().get_t_dilate();

  if(moved == false) {
    //decay velocity only when user hasn't moved
    vel = vel.decay(vel_decay * t_frame::get().t_adjust() * t_frame::get().get_t_dilate());
  }
  //reset moved
  moved = false;


  //check if this next move is legal
  rebuff(map_h::get().check_rebuff(pos, last_pos));

  //update position
  last_pos = pos;
}

void movable::draw() const { }
