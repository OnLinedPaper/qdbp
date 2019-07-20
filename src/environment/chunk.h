#ifndef CHUNK_H_
#define CHUNK_H_

#include "src/vec2d/vec2d.h"
#include "src/image/image.h"

class chunk {
public:
  chunk(vec2d);
  chunk(double, double);
  chunk(vec2d, bool, bool, bool, bool);
  chunk(double, double, bool, bool, bool, bool);

  //check if the edges are "borders", that is, if they touch nothing
  bool is_b_up() { return border[0]; }
  bool is_b_dn() { return border[1]; }
  bool is_b_lf() { return border[2]; }
  bool is_b_rt() { return border[3]; }

  void set_b_up(bool b) { border[0] = b; }
  void set_b_dn(bool b) { border[1] = b; }
  void set_b_lf(bool b) { border[2] = b; }
  void set_b_rt(bool b) { border[3] = b; }

  void draw() const;
  void debug_draw(double x, double y) const;

private:

  double length = 1000; //pre-determined - this doesn't change
  vec2d tlc; //top-left corner
  bool border[4];

  chunk();
  chunk(const chunk&) = delete;
  chunk &operator=(const chunk&) = delete;

  image i;

};

#endif
