#ifndef MOVABLE_H_
#define MOVABLE_H_

#include "src/vec2d/vec2d.h"
#include "src/environment/chunk/chunk.h"
#include "src/environment/map/map_handler.h"

class movable {
public:

  void set_x(double x) { pos[0] = x; }
  void set_y(double y) { pos[1] = y; }
  vec2d get_pos() const { return pos; }
  void set_pos(vec2d p) { pos = p; }
  void set_pos(double x, double y) { pos[0] = x; pos[1] = y; }

  void rebuff(unsigned char posi) {
    if(posi) {
      //somewhere along the way, we tripped a barrier we shouldn't have
      if(posi & chunk::UP) {
        vel[1] = fabs(vel[1]);
      }
      if(posi & chunk::DN) {
        vel[1] = -fabs(vel[1]);
      }
      if(posi & chunk::LF) {
        vel[0] = fabs(vel[0]);
      }
      if(posi & chunk::RT) {
        vel[0] = -fabs(vel[0]);
      }

      //reset position to previous
      pos = last_pos;
    }
  }

  virtual void draw() const = 0;

protected:
  void update() {
    //check if this next move is legal
    rebuff(map_h::get().check_rebuff(pos, last_pos));

    //update position
    last_pos = pos;
  }

  vec2d pos;
  vec2d last_pos;
  vec2d vel;

  std::string path;

};


#endif
