#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "src/movers/movable.h"
#include "src/image/image.h"
#include <SDL2/SDL.h>

class drawable : public movable {
public:
  drawable(const std::string im) : i(im) { }

protected:
  void update() override { movable::update(); }
  image i;


};

#endif
