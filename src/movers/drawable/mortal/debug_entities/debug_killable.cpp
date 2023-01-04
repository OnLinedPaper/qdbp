#include "src/movers/drawable/mortal/debug_entities/debug_killable.h"

#if defined RENDER_NC
#include "debug_killable_nc.cpp"
#endif

d_killable::d_killable(const std::string &path) :
  mortal(path)
{ }

void d_killable::update() {
  mortal::update(); 
}

#if defined RENDER_SDL
void d_killable::draw() const {

  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  //the outline, fading with health
  image_handler::get().draw_v2(image_name + "_outline", dest_r.x, dest_r.y, last_angle, false, 0, team_col, get_health_frac());
  //the actual wireframe
  image_handler::get().draw_v2(image_name, dest_r.x, dest_r.y, last_angle, false, 0, team_col, 1);

  mortal::draw();
}
#endif
