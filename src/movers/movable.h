#ifndef MOVABLE_H_
#define MOVABLE_H_

#include "../vec2d/vec2d.h"

class movable {
public:

  virtual void draw() const = 0;

protected:
  vec2d pos;
  vec2d vel;

};


#endif
