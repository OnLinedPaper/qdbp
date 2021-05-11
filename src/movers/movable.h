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

  movable(std::string path);
  movable(std::string path, const vec2d pos, const vec2d vel);
  virtual ~movable() = default;

  const std::string &get_id() { return id; }

  void set_x(float x) { pos[0] = x; }
  void set_y(float y) { pos[1] = y; }
  const vec2d get_vel() const { return vel; }
  void set_vel(vec2d v) { vel = v; }
  void set_vel(float x, float y) { vel[0] = x; vel[1] = y; }
  const vec2d get_pos() const { return pos; }
  virtual void set_pos(const vec2d p) { this->set_pos(p[0], p[1]); }
  virtual void set_pos(float x, float y) { pos[0] = x; pos[1] = y; last_pos = pos; }
  void teleport(const vec2d p) { set_pos(p); }
  void stop() { vel[0] = 0; vel[1] = 0; }

  void rebuff(unsigned char posi);

  bool is_active() const { return active; }
  void set_active(bool b) { active = b; }

  virtual void move_up() = 0;
  virtual void move_dn() = 0;
  virtual void move_lf() = 0;
  virtual void move_rt() = 0;

  virtual void destroy() { set_active(false); };
  virtual void draw() const;

protected:
  virtual void update();

  std::string id;

  vec2d pos;
  vec2d last_pos;
  vec2d vel;
  float vel_accel;
  float vel_cap;
  float vel_overcap;
  float vel_decay;
  bool moved;
  bool boosted;

  bool active;

  std::string path;

};


#endif
