#ifndef DEBUG_DRAW_H_
#define DEBUG_DRAW_H_

#include "movable.h"
#include "vec2d/vec2d.h"

class d_draw : public movable {
public:
  d_draw();
  void adjust_x(double d) { pos[0] += d; }
  void adjust_y(double d) { pos[1] += d; }

  void draw() const;

};

#endif
