#include "movable.h"
#include "render.h"
#include <stdio.h>

movable::movable() : x(0), y(0) {

}

void movable::set_position(int x, int y) {
  this->x = x;
  this->y = y;
}

void movable::adjust_x(int adj) {
  this->x += adj;
}

void movable::adjust_y(int adj) {
  this->y += adj;
}

void movable::draw() const {
  SDL_Rect r;
  r.x = this->x;
  r.y = this->y;
  r.w = 10;
  r.h = 10;
  SDL_RenderFillRect(render::get().get_renderer(), &r);
}
