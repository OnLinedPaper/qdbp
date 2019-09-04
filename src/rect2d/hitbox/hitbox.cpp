#include "hitbox.h"
#include "src/vec2d/vec2d.h"
#include "math.h"

const int hitbox::TYPE_HITBOX = 0;
const int hitbox::TYPE_HURTBOX = 1;
const int hitbox::TYPE_WEAKBOX = 2;
const int hitbox::TYPE_SHIELDBOX = 3;

hitbox::hitbox(double size, vec2d offs, int t) :
  rect2d(0, 0, size, size),
  offset(offs),
  type(t)
{ }

void hitbox::set_box_center(vec2d pos, double last_angle) {
  //adjust rectangle's position based on last angle and offset
  double PI = 3.141592653589793238462643383279502884;
  double angle = (last_angle * PI) / 180;

  vec2d new_center = pos;

  //rotate the offset according to the last angle, to rotate the hitboxes
  //with the model
  vec2d ofs(0,0);
  ofs[0] = cos(angle) * offset[0] - sin(angle) * offset[1];
  ofs[1] = sin(angle) * offset[0] + cos(angle) * offset[1];

  //use calculated value to set new hitbox location
  new_center = (new_center - ofs);

  set_center(new_center);
}

void hitbox::draw() const {
  //color the boxes based on their type
  if(type == TYPE_HITBOX) { rect2d::draw(0, 200, 0); }
  if(type == TYPE_HURTBOX) { rect2d::draw(255, 0, 0); }
  if(type == TYPE_WEAKBOX) { rect2d::draw(255, 255, 0); }
  if(type == TYPE_SHIELDBOX) { rect2d::draw(0, 255, 255); }

}
