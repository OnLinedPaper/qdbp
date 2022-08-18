#include "src/movers/drawable/mortal/debug_entities/debug_player.h"
#include "src/movers/drawable/mortal/weapon.h"

d_player::d_player(const std::string path) :
  player(path)
{ }

void d_player::move_up()
  { vel[1] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_player::move_dn()
  { vel[1] += vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_player::move_lf()
  { vel[0] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_player::move_rt()
  { vel[0] += vel_accel * t_frame::get().t_adjust(); moved = true; }

void d_player::update() { player::update(); }

void d_player::draw() const {

  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  //the outline, fading with health
  //image_handler::get().draw_v2(image_name + "_outline", dest_r.x, dest_r.y, last_angle, false, 0, team_col, get_health_frac());
  image_handler::get().draw_v2(image_name + "_outline", dest_r.x, dest_r.y, last_angle, false, 0, team_col, 1);
  //the actual wireframe
  static int debug_target_spin = 0;
  image_handler::get().draw_v2(image_name, dest_r.x, dest_r.y, last_angle, false, 0, team_col, 1, "/pink_scanlines", 0.1, debug_target_spin++, 0, 1);

  player::draw();
}
