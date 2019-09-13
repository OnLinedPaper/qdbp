#ifndef RECT_2D_
#define RECT_2D_

#include "src/vec2d/vec2d.h"

class rect2d {
public:
  rect2d(double x, double y, double w, double h) : tlc(x, y), dims(w, h) { }
  rect2d(const rect2d &r) {
    tlc[0] = r.tlc[0];
    tlc[1] = r.tlc[1];
    dims[0] = r.dims[0];
    dims[1] = r.dims[1];
  }

  void set_tlc(const vec2d &t) { tlc = t; }
  void set_center(const vec2d &c) {
    tlc[0] = c[0] - (dims[0] / 2);
    tlc[1] = c[1] - (dims[1] / 2);
  }
  vec2d get_dims() const { return dims; }
  void set_dims(const vec2d &d) { dims = d; }

  void draw() const;
  void draw(int, int, int) const;
  void tlc_draw() const;
  void coord_draw(double, double) const;

  bool overlap(const rect2d &r) const;

  vec2d get_tlc() const { return tlc; }
  vec2d get_brc() const { return vec2d(tlc[0] + dims[0], tlc[1] + dims[1]); }

private:
  vec2d tlc;
  vec2d dims;
};

#endif
