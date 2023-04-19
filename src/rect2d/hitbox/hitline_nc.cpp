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
  '-');
}
#endif
