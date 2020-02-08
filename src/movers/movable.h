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
  virtual ~movable() = default;

  void set_x(float x) { pos[0] = x; }
  void set_y(float y) { pos[1] = y; }
  const vec2d get_pos() const { return pos; }
  void set_pos(vec2d p) { pos = p; last_pos = p; }
  void set_pos(float x, float y) { pos[0] = x; pos[1] = y; }
  void teleport(vec2d p) { set_pos(p); }
  void stop() { vel[0] = 0; vel[1] = 0; }

  void rebuff(unsigned char posi);

  virtual void move_up() = 0;
  virtual void move_dn() = 0;
  virtual void move_lf() = 0;
  virtual void move_rt() = 0;

  virtual void draw() const = 0;

protected:
  virtual void update();

  vec2d pos;
  vec2d last_pos;
  vec2d vel;
  float vel_accel;
  float vel_cap;
  float vel_overcap;
  float vel_decay;
  bool moved;
  bool boosted;

  std::string path;

};


#endif
