#if defined RENDER_NC
#include "hitline.h"
#include "src/viewport/viewport.h"
#include "src/rect2d/rect2d.h"
#include "src/image/image_handler_nc.h"


void hitline::draw() const {
  image_handler::get().draw_line({
    start[0],// - viewport::get().get_tlc_x(),
    start[1],// - viewport::get().get_tlc_y()
  }, {
    end[0],// - viewport::get().get_tlc_x(),
    end[1]// - viewport::get().get_tlc_y()
  },
  angle_to_char());
}

//determine angle of line and return an appropriate char
char hitline::angle_to_char() const {
/*
switch vars around to ensure run is always positive
calculate slope, and then: 

-2.5 to +2.5, and inf:  |
+2.5 to +0.4:           /
+0.4 to -0.4:           -
-0.4 to -2.5:           \

112 to 68:  |
68 to 22:   /
23 to 338:  -
338 to 292: \

*/

  //lp is left point; rp is right point
  //also multi by 10 to meet slope requirements
  bool p1_left = start[0] < end[0];
  int lp_x = (p1_left ? start[0] : end[0]);
  int lp_y = (p1_left ? start[1] : end[1]);
  int rp_x = (p1_left ? end[0] : start[0]);
  int rp_y = (p1_left ? end[1] : start[1]);

  int rise = rp_y - lp_y;
  int run = rp_x - lp_x;  //never negative

  if(run == 0 || -25 * run > 10 * rise || 10 * rise >= 25 * run) { return '|'; }
  if(25 * run > 10 * rise && 10 * rise >= 4 * run) { return '\\'; }
  if(4 * run > 10 * rise && 10 * rise >= -4 * run) { return '-'; }
  if(-4 * run > 10 * rise && 10 * rise >= -25 * run) { return '/'; }

  return '=';
}
#endif
