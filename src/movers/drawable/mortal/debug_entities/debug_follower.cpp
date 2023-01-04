#include "debug_follower.h"
#include "src/image/image_handler.h"
#include "src/entity_handler/entity_handler.h"

#if defined RENDER_NC
#include "debug_follower_nc.cpp"
#endif

d_follower::d_follower(const std::string path) :
  mortal(path),
  player_pos(0, 0)
{ 
  set_follow_thru_portals(true);
}

void d_follower::update() {
  mortal::update();

  //update player's position
  set_player_pos(e_handler::get().get_plr_pos());

  //check how far we are
  float distance = (this->get_pos() - player_pos).magnitude();
  if(distance < 1100 && distance > 200) {
    //accelerate towards player
    vec2d move_towards = this->get_pos() - player_pos;
    vel = vel -(move_towards.normalize() * t_frame::get().t_adjust()) * vel_accel;
    moved = true;
  }
  if(distance > 1500) {
    //teleport to player
    teleport(player_pos);
    last_pos = player_pos;
  }
}

#if defined RENDER_SDL
void d_follower::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_v2(image_name, dest_r.x, dest_r.y, last_angle, false, 0, team_col, 1);

  mortal::draw();
 
}
#endif
