#include "debug_hittable.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include "src/rect2d/hitbox/hitbox.h"

d_hittable::d_hittable(const std::string path) :
  hittable(path)
{ }

void d_hittable::move_up()
  { vel[1] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_hittable::move_dn()
  { vel[1] += vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_hittable::move_lf()
  { vel[0] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_hittable::move_rt()
  { vel[0] += vel_accel * t_frame::get().t_adjust(); moved = true; }

void d_hittable::update() {
  hittable::update();
}

void d_hittable::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate_color(image_name, dest_r.x, dest_r.y, 0, last_angle, team_col);
}
