#ifndef MOVABLE_H_
#define MOVABLE_H_


#include <SDL2/SDL.h>

class movable {

public:
  movable();
  void set_position(int, int);
  void adjust_x(int);
  void adjust_y(int);

  void draw() const;

private:
  int x, y;

};

#endif
