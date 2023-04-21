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
      (tlc[0]),// - viewport::get().get_tlc_x()/2),
      (tlc[1]),// - viewport::get().get_tlc_y()/2)
    }, {
      (tlc[0] + dims[0]),// - viewport::get().get_tlc_x()/2),
      (tlc[1] + dims[1])// - viewport::get().get_tlc_y()/2)
    },
    true,
    colo_to_char(red, green, blue)
  );
}

//"converts" color to char - uses the passed-in color of the different
//types of hitbox to turn them into a char
char rect2d::colo_to_char(int r, int g, int b) const {
  if(r==  0 && g==200 && b==  0) { return 'H'; }
  if(r==255 && g==  0 && b==  0) { return '+'; }
  if(r==255 && g==255 && b==  0) { return 'w'; }
  if(r==255 && g==128 && b==  0) { return 'A'; }
  if(r==  0 && g==255 && b==255) { return 'S'; }
  if(r==255 && g==128 && b==255) { return 'p'; }
  if(r==  0 && g==  0 && b==128) { return ' '; }

  return '?';
}
#endif
