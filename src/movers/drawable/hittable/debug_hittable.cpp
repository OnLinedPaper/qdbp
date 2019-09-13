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

  //cap velocity
  vel = vel.cap(vel_cap);

  //move, based on velocity AND time since last frame -
  //this prevents lag spikes from "slowing time"
  pos[0] += vel[0] * t_frame::get().d_factor();
  pos[1] += vel[1] * t_frame::get().d_factor();

  if(moved == false) {
    //decay velocity only when user hasn't moved
    vel = vel.decay(vel_decay * t_frame::get().d_factor());
  }
  //reset moved
  moved = false;

  //update hitboxes
  hittable::update_boxes();
}

void d_hittable::draw() const {
  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate(image_name, dest_r.x, dest_r.y, 0, last_angle);

  hittable::draw_boxes();
}
