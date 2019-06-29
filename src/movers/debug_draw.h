#ifndef DEBUG_DRAW_H_
#define DEBUG_DRAW_H_

#include "movable.h"
#include "../vec2d/vec2d.h"

class d_draw : public movable {
public:
  d_draw();
  void adjust_x(double d) { pos[0] += d; }
  void adjust_y(double d) { pos[1] += d; }
  void adjust_x_vel(double d) { vel[0] += d; moved = true; }
  void adjust_y_vel(double d) { vel[1] += d; moved = true; }

  void move_up();
  void move_down();
  void move_left();
  void move_right();

  void update();

  void draw() const;

private:
  double vel_accel;
  double vel_cap;
  double vel_decay;
  bool moved;

};

#endif
