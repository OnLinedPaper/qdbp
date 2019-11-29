
#include "debug_follower.h"
#include "src/image/image_handler.h"

d_follower::d_follower(const std::string path) :
  hittable(path),
  player_pos(0,0)
{ }

void d_follower::move_up() { return; }
void d_follower::move_dn() { return; }
void d_follower::move_lf() { return; }
void d_follower::move_rt() { return; }

void d_follower::update() {
  hittable::update();

  //check how far from the player we are
  float distance = (this->get_pos() - player_pos).magnitude();
  if(distance < 1100 && distance > 200) {
    //accelerate towards player
    vec2d move_towards = this->get_pos() - player_pos;
    vel = vel -(move_towards * t_frame::get().t_adjust()).normalize() * vel_accel;
    moved = true;
  }
  if(distance > 1500) {
    //teleport to player
    teleport(player_pos);
    last_pos = player_pos;
  }
}

void d_follower::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate(image_name, dest_r.x, dest_r.y, 0, last_angle);
}
