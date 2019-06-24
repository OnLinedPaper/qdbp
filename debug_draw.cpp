#include "movable.h"
#include "debug_draw.h"
#include "render.h"
#include "vec2d/vec2d.h"
#include <iostream>

d_draw::d_draw() : moved(false) {
  pos = vec2d(0, 0);
  vel = vec2d(0, 0);

  vel_accel = 1;
  vel_cap = 20;
  vel_decay = 0.1;
}

void d_draw::move_up() { vel[1] -= vel_accel; moved = true; }
void d_draw::move_down() { vel[1] += vel_accel; moved = true; }
void d_draw::move_left() { vel[0] -= vel_accel; moved = true; }
void d_draw::move_right() { vel[0] += vel_accel; moved = true; }

void d_draw::update() {

  //cap velocity
  vel = vel.cap(vel_cap);

  //move, based on velocity
  pos[0] += vel[0];
  pos[1] += vel[1];

  if(moved == false) {
    //decay velocity only when user hasn't moved
    vel = vel.decay(vel_decay);
  }

  //reset this
  moved = false;
}

void d_draw::draw() const {
  SDL_Rect r;
  r.x = this->pos[0];
  r.y = this->pos[1];
  r.w = 10;
  r.h = 10;
  SDL_RenderFillRect(render::get().get_renderer(), &r);
}
