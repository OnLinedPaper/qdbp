#if defined RENDER_NC
#include "rect2d.h"
#include "src/vec2d/vec2d.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/image/image_handler_nc.h"

void rect2d::coord_draw(float x, float y) const { 
  image_handler::get().draw_box({
      x,
      y
    }, {
      x + dims[0],
      y + dims[0]
    }, 
    true, 
    'X'
  );
}

void rect2d::draw(int red, int green, int blue) const { 
  image_handler::get().draw_box({
      (tlc[0] - viewport::get().get_tlc_x()),
      (tlc[1] - viewport::get().get_tlc_y())
    }, {
      (tlc[0] + dims[0] - viewport::get().get_tlc_x()),
      (tlc[1] + dims[1] - viewport::get().get_tlc_y())
    },
    true,
    'X'
  );
}
#endif
