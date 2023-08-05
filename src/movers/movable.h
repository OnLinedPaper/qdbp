#ifndef MOVABLE_H_
#define MOVABLE_H_

#include "src/vec2d/vec2d.h"
#include "src/environment/chunk/chunk.h"
#include "src/environment/map/map_handler.h"
#include "src/xml_parser/xmlparse.h"
#include <cmath>
#include "src/timeframe/timeframe.h"
#include "types.h"

class movable {
public:

  movable(std::string path);
  movable(std::string path, const vec2d pos, const vec2d vel);
  virtual ~movable() = default;

  const std::string &get_id() { return id; }
  const std::string &get_type() { return entity_type; }

  void set_x(cint x) { pos[0] = x; }
  void set_y(cint y) { pos[1] = y; }
  cint get_x() const { return pos[0]; }
  cint get_y() const { return pos[1]; }
  void shift_x(cint x) { pos[0] += x; last_pos = pos; }
  void shift_y(cint y) { pos[1] += y; last_pos = pos; }
  const vec2d get_vel() const { return vel; }
  void set_vel(vec2d v) { vel = v; }
  void set_vel(cint x, cint y) { vel[0] = x; vel[1] = y; }
  cint get_vel_cap() { return vel_cap; }
  const vec2d get_pos() const { return pos; }
  virtual void set_pos(const vec2d p) { this->set_pos(p[0], p[1]); }
  virtual void set_pos(cint x, cint y) { pos[0] = x; pos[1] = y; last_pos = pos; }
  void teleport(const vec2d p) { set_pos(p); }
  void stop() { vel[0] = 0; vel[1] = 0; }

  void rebuff(unsigned char posi);

  bool get_follow_thru_portals() const { return follow_thru_portals; }
  void set_follow_thru_portals(bool b) { follow_thru_portals = b; }

  bool is_active() const { return active; }
  void set_active(bool b) { active = b; vel_accel_mod = 1; vel_cap_mod = 1; }

  virtual void destroy() { set_active(false); };
  virtual void draw() const;

protected:
  virtual void update();

  std::string id;
  std::string entity_type;

  vec2d pos;
  vec2d last_pos;
  vec2d vel;
  cint vel_accel;
  cint vel_accel_mod;
  cint vel_cap;
  cint vel_cap_mod;
  //cint vel_overcap;
  cint vel_decay;
  bool moved;

  bool follow_thru_portals;

  bool active;

  std::string path;

};


#endif
