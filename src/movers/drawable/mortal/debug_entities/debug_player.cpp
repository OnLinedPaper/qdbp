#include "src/movers/drawable/mortal/debug_entities/debug_player.h"
#include "src/movers/drawable/mortal/weapon.h"
#include "src/utils/rng.h"

#if defined RENDER_NC
#include "debug_player_nc.cpp"
#endif

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

#if defined RENDER_SDL
void d_player::draw() const {

  vec2d dest_v = {pos[0], pos[1]};
  vec2d rand_v = {0, 0};

  static int debug_target_spin = 0;

  //TODO: twitchy outline
  //the outline, fading with health
  //image_handler::get().draw_v2(image_name + "_outline", dest_r.x, dest_r.y, last_angle, false, 0, team_col, get_health_frac());

  rand_v = {(float)rng::get().get_gfx(), (float)rng::get().get_gfx()};
  rand_v = rand_v.normalize();
  image_handler::get().draw_v2(
      image_name + "_outline"
    , dest_v[0] + (10 * (1 - get_health_frac()) * rand_v[0])
    , dest_v[1] + (10 * (1 - get_health_frac()) * rand_v[1])
    , last_angle
    , false
    , 0
    , team_col
    , 1
    , "/pink_scanlines"
    , 0.1
    , debug_target_spin
    , 0
    , 1
  );
  //the actual wireframe
  image_handler::get().draw_v2(image_name, dest_v[0], dest_v[1], last_angle, false, 0, team_col, 1, "/pink_scanlines", 0.1, debug_target_spin++, 0, 1);

  player::draw();
}
#endif
