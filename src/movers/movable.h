#ifndef MOVABLE_H_
#define MOVABLE_H_

#include "src/vec2d/vec2d.h"

class movable {
public:

  void set_x(double x) { pos[0] = x; }
  void set_y(double y) { pos[1] = y; }
  vec2d get_pos() const { return pos; }

  virtual void draw() const = 0;

protected:
  vec2d pos;
  vec2d vel;

  std::string path;

};


#endif
