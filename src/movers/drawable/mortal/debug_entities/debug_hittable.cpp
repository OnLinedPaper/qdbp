#include "src/movers/drawable/mortal/debug_entities/debug_hittable.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include "src/rect2d/hitbox/hitbox.h"

d_hittable::d_hittable(const std::string path) :
  mortal(path)
{ }

void d_hittable::update() {
  mortal::update();
}

void d_hittable::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate_color(image_name, dest_r.x, dest_r.y, 0, last_angle, team_col);

  mortal::draw();
}
