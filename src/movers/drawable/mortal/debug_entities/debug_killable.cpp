#include "src/movers/drawable/mortal/debug_entities/debug_killable.h"

d_killable::d_killable(const std::string &path) :
  mortal(path)
{ }

void d_killable::update() {
  mortal::update(); 
}

void d_killable::draw() const {

  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate_color_outline(
    image_name, dest_r.x, dest_r.y, 0, last_angle, team_col, true, get_health_percent() 
  );

  mortal::draw();

}
