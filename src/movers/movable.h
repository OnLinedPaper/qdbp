#ifndef MOVABLE_H_
#define MOVABLE_H_

#include "src/vec2d/vec2d.h"
#include "src/environment/chunk.h"

class movable {
public:

  void set_x(double x) { pos[0] = x; }
  void set_y(double y) { pos[1] = y; }
  vec2d get_pos() const { return pos; }

  void rebuff(unsigned char posi) {
    if(posi & chunk::UP) {
      vel[1] = fabs(vel[1]);
      pos[1] += 1;
    }
    if(posi & chunk::DN) {
      vel[1] = -fabs(vel[1]);
      pos[1] -= 1;
    }
    if(posi & chunk::LF) {
      vel[0] = fabs(vel[0]);
      pos[0] += 1;
    }
    if(posi & chunk::RT) {
      vel[0] = -fabs(vel[0]);
      pos[0] -= 1;
    }

  }

  virtual void draw() const = 0;

protected:
  void update() { }

  vec2d pos;
  vec2d vel;

  std::string path;

};


#endif
