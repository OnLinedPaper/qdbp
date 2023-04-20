#if defined RENDER_NC
#include "hitline.h"
#include "src/viewport/viewport.h"
#include "src/rect2d/rect2d.h"
#include "src/image/image_handler_nc.h"


void hitline::draw() const {
  image_handler::get().draw_line({
    start[0] - viewport::get().get_tlc_x(),
    start[1] - viewport::get().get_tlc_y()
  }, {
    end[0] - viewport::get().get_tlc_x(),
    end[1] - viewport::get().get_tlc_y()
  },
  angle_to_char());
}

//determine angle of line and return an appropriate char
char hitline::angle_to_char() const {
/*
switch vars around to ensure run is always positive
calculate slope, and then: 

-2 to +2, and inf: |
+2 to +0.5: /
+0.5 to -0.5: -
-0.5 to -2: \
*/

  return '=';
}
#endif
