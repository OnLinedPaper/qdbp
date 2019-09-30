#ifndef MOVABLE_H_
#define MOVABLE_H_

#include "src/vec2d/vec2d.h"
#include "src/environment/chunk/chunk.h"
#include "src/environment/map/map_handler.h"
#include "src/xml_parser/xmlparse.h"
#include <cmath>
#include "src/timeframe/timeframe.h"

class movable {
public:

  movable(std::string path) :
    pos(0,0),
    last_pos(0,0),
    vel(0,0),
    vel_accel(xmlparse::get().get_xml_double(path + "/movement/vel_accel") * t_frame::get().f_factor()),
    vel_cap(xmlparse::get().get_xml_double(path + "/movement/vel_cap") * t_frame::get().f_factor()),
    vel_decay(xmlparse::get().get_xml_double(path + "/movement/vel_decay") * t_frame::get().f_factor()),
    moved(false)
  { }

  void set_x(double x) { pos[0] = x; }
  void set_y(double y) { pos[1] = y; }
  const vec2d get_pos() const { return pos; }
  void set_pos(vec2d p) { pos = p; last_pos = p; }
  void set_pos(double x, double y) { pos[0] = x; pos[1] = y; }
  void stop() { vel[0] = 0; vel[1] = 0; }

  void rebuff(unsigned char posi) {
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

  virtual void move_up() = 0;
  virtual void move_dn() = 0;
  virtual void move_lf() = 0;
  virtual void move_rt() = 0;

  virtual void draw() const = 0;

protected:
  virtual void update() {
    //check if this next move is legal
    rebuff(map_h::get().check_rebuff(pos, last_pos));

    //update position
    last_pos = pos;
  }

  vec2d pos;
  vec2d last_pos;
  vec2d vel;
  double vel_accel;
  double vel_cap;
  double vel_decay;
  bool moved;

  std::string path;

};


#endif
