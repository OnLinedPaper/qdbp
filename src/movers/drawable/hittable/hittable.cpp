#include "hittable.h"
#include "src/rect2d/rect2d.h"
#include "src/rect2d/hitbox/hitbox.h"
#include "src/movers/drawable/hittable/hittable.h"

void hittable::update() {
  drawable::update();
}

void hittable::update_boxes() {
  //called AFTER all other updates are done, in order to prevent
  //hitboxes from lagging behind the model

  //update location of the hitboxes, based on their positions
  for(hitbox &h : hitboxes) { h.set_box_center(pos, last_angle); }
  for(hitbox &h : hurtboxes) { h.set_box_center(pos, last_angle); }
  for(hitbox &h : weakboxes) { h.set_box_center(pos, last_angle); }
  for(hitbox &h : shieldboxes) { h.set_box_center(pos, last_angle); }
}

void hittable::draw_boxes() const {
  for(hitbox h : hitboxes) { h.draw(); }
  for(hitbox h : hurtboxes) { h.draw(); }
  for(hitbox h : weakboxes) { h.draw(); }
  for(hitbox h : shieldboxes) { h.draw(); }
}
