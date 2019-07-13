#ifndef DEBUG_DRAWABLE_H
#define DEBUG_DRAWABLE_H
#include <chrono>
#include "drawable.h"
#include "src/image/image.h"
#include "src/timeframe/timeframe.h"

class d_drawable : public drawable {
public:
  d_drawable();

  void move_up();
  void move_down();
  void move_left();
  void move_right();

  void update();

  void draw() const;
  void load_texture();

private:
  double vel_accel;
  double vel_cap;
  double vel_decay;
  bool moved;

  image i;

};

#endif
