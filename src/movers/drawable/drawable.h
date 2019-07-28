#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "src/movers/movable.h"
#include "src/image/image.h"
#include <SDL2/SDL.h>

class drawable : public movable {
public:
  drawable(const std::string im) : image_name(im), frame_bump(rand()) { }

protected:
  void update() override { movable::update(); }
  std::string image_name;

  //this is so when the image is drawn, each one has a different
  //frame
  double frame_bump;


};

#endif
