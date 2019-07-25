#ifndef DEBUG_DRAWABLE_H
#define DEBUG_DRAWABLE_H
#include <chrono>
#include "drawable.h"
#include "src/image/image.h"
#include "src/timeframe/timeframe.h"

class d_drawable : public drawable {
public:
  static const std::string path;

  d_drawable();

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

  image i;
  double last_angle;

};

#endif
