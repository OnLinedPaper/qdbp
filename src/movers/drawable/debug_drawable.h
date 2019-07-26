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

  void move_up() override;
  void move_dn() override;
  void move_lf() override;
  void move_rt() override;

  void update() override;

  void draw() const override;

private:

  double last_angle;

};

#endif
