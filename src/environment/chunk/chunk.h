#ifndef CHUNK_H_
#define CHUNK_H_

#include "src/vec2d/vec2d.h"
#include "src/image/image.h"

class chunk {
public:
  chunk(vec2d&);
  chunk(float, float);
  chunk(vec2d&, bool, bool, bool, bool);
  chunk(float, float, bool, bool, bool, bool);
  chunk(float, float, bool, bool, bool, bool, std::string);

  chunk(const chunk&);
  chunk &operator=(const chunk&);

  static const unsigned char IN;
  static const unsigned char UP;
  static const unsigned char DN;
  static const unsigned char LF;
  static const unsigned char RT;

  static const float length;

  //check if a given point is in a chunk - if not, check if it's above,
  //below, left, or right
  unsigned char chunk_pos(vec2d&) const;
  unsigned char chunk_pos(float, float) const;
  unsigned char chunk_rebuff(vec2d &) const;
  unsigned char chunk_rebuff_forced(vec2d &) const;

  //check if the edges are "borders", that is, if they touch nothing
  bool is_b_up() const { return border[0]; }
  bool is_b_dn() const { return border[1]; }
  bool is_b_lf() const { return border[2]; }
  bool is_b_rt() const { return border[3]; }

  void set_b_up(bool b) { border[0] = b; }
  void set_b_dn(bool b) { border[1] = b; }
  void set_b_lf(bool b) { border[2] = b; }
  void set_b_rt(bool b) { border[3] = b; }

  //if it's in bounds, set a barrier; else, un-set it
  void set_bound_b_up() { border[0] = in_bounds; }
  void set_bound_b_dn() { border[1] = in_bounds; }
  void set_bound_b_lf() { border[2] = in_bounds; }
  void set_bound_b_rt() { border[3] = in_bounds; }

  const vec2d get_midpoint() const { return vec2d(
    tlc[0] + length/2,
    tlc[1] + length/2
  );
  }

  bool get_in_bounds() const { return in_bounds; }
  void set_in_bounds(bool i) { in_bounds = i; }
  bool get_has_gate() const { return has_gate; }
  std::string get_gate_dest() const { return g_dest; }

  void add_gate(std::string dest, std::string name);

  void draw() const;
  void draw_nc() const;
  void debug_draw(float x, float y) const;

private:

  vec2d tlc; //top-left corner
  bool border[4];

  chunk();

  std::string i1_name;
  std::string i2_name;
  float frame_bump;

  bool in_bounds;

  bool has_gate;
  std::string g_dest;
  std::string g_name;

};

#endif
