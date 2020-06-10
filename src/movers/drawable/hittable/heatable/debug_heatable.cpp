#include "debug_heatable.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include "src/rect2d/hitbox/hitbox.h"

d_heatable::d_heatable(const std::string path) :
  heatable(path),
  weapon(
    "/movers/hittable/" + xmlparse::get().get_xml_string(path + "/first_weapon"
  ))
{ }

void d_heatable::move_up()
  { vel[1] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_heatable::move_dn()
  { vel[1] += vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_heatable::move_lf()
  { vel[0] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_heatable::move_rt()
  { vel[0] += vel_accel * t_frame::get().t_adjust(); moved = true; }

void d_heatable::boost(bool b) {
  boosted = b;
}

void d_heatable::shoot() {
  return;
}

void d_heatable::update() {
  heatable::update();
}

void d_heatable::draw() const {

  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate(image_name, dest_r.x, dest_r.y, 0, last_angle);


  heatable::draw();
}
