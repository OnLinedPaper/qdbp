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

  void update() override;

  void draw() const override;
  
};

#endif
