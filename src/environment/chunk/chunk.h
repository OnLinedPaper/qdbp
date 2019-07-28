#ifndef CHUNK_H_
#define CHUNK_H_

#include "src/vec2d/vec2d.h"
#include "src/image/image.h"

class chunk {
public:
  chunk(vec2d&);
  chunk(double, double);
  chunk(vec2d&, bool, bool, bool, bool);
  chunk(double, double, bool, bool, bool, bool);

  static const unsigned char IN;
  static const unsigned char UP;
  static const unsigned char DN;
  static const unsigned char LF;
  static const unsigned char RT;

  static const double length;

  //check if a given point is in a chunk - if not, check if it's above,
  //below, left, or right
  unsigned char chunk_pos(vec2d&) const;
  unsigned char chunk_pos(double, double) const;
  unsigned char chunk_rebuff(vec2d &) const;

  //check if the edges are "borders", that is, if they touch nothing
  bool is_b_up() const { return border[0]; }
  bool is_b_dn() const { return border[1]; }
  bool is_b_lf() const { return border[2]; }
  bool is_b_rt() const { return border[3]; }

  void set_b_up(bool b) { border[0] = b; }
  void set_b_dn(bool b) { border[1] = b; }
  void set_b_lf(bool b) { border[2] = b; }
  void set_b_rt(bool b) { border[3] = b; }

  vec2d get_midpoint() const { return vec2d(
    tlc[0] + length/2,
    tlc[1] + length/2
  );
  }

  void draw() const;
  void debug_draw(double x, double y) const;

private:

  vec2d tlc; //top-left corner
  bool border[4];

  chunk();
  chunk(const chunk&) = delete;
  chunk &operator=(const chunk&) = delete;

  std::string i_name;
  double frame_bump;

};

#endif
