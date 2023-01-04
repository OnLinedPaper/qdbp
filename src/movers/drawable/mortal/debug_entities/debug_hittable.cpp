#include "src/movers/drawable/mortal/debug_entities/debug_hittable.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include "src/rect2d/hitbox/hitbox.h"

#if defined RENDER_NC
#include "debug_hittable_nc.cpp"
#endif

d_hittable::d_hittable(const std::string path) :
  mortal(path)
{ }

void d_hittable::update() {
  mortal::update();
}

#if defined RENDER_SDL
void d_hittable::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_v2(image_name, dest_r.x, dest_r.y, last_angle, false, 0, team_col, 1);

  mortal::draw();
}
#endif
