#include "hittable.h"
#include "src/rect2d/rect2d.h"
#include "src/rect2d/hitbox/hitbox.h"
#include "src/rect2d/hitbox/hitline.h"
#include "src/movers/drawable/hittable/hittable.h"


hittable::hittable(const std::string path) :
  drawable(path)
{
  //load all hitboxes
  for(std::string p : xmlparse::get().get_all_child_tags(path + "/hitboxes")) {
    hitbox h(path + "/hitboxes/" + p);

      if(h.get_type() == hitbox::TYPE_HITBOX) {
        hitboxes.push_back(h);
      }
      else if(h.get_type() == hitbox::TYPE_HURTBOX) {
        hurtboxes.push_back(h);
      }
      else if(h.get_type() == hitbox::TYPE_WEAKBOX) {
        weakboxes.push_back(h);
      }
      else if(h.get_type() == hitbox::TYPE_ARMORBOX) {
        armorboxes.push_back(h);
      }
      else if(h.get_type() == hitbox::TYPE_SHIELDBOX) {
        shieldboxes.push_back(h);
      }
      else if(h.get_type() == hitbox::TYPE_PICKUPBOX) {
        pickupboxes.push_back(h);
      }
      else if(h.get_type() == hitbox::TYPE_VACUUMBOX) {
        vacuumboxes.push_back(h);
      }

  }

    //push the boxes into one big vector
    //ordered this way for display
    boxes.push_back(&vacuumboxes);
    boxes.push_back(&hurtboxes);

    boxes.push_back(&weakboxes);
    boxes.push_back(&hitboxes);
    boxes.push_back(&armorboxes);
    boxes.push_back(&shieldboxes);

    boxes.push_back(&pickupboxes);
}

void hittable::update() {
  drawable::update();

  //update hitboxes
  hittable::update_boxes();
}

bool hittable::collides(const hitbox& in_box, int type) const {
  //determine the type of hitbox to check
  if(type == hitbox::TYPE_HITBOX) {
    for(hitbox h : hitboxes) { if(h.collides(in_box)) { return true; } }
  }
  else if(type == hitbox::TYPE_HURTBOX) {
    for(hitbox h : hurtboxes) { if(h.collides(in_box)) { return true; } }
  }
  else if(type == hitbox::TYPE_WEAKBOX) {
    for(hitbox h : weakboxes) { if(h.collides(in_box)) { return true; } }
  }
  else if(type == hitbox::TYPE_ARMORBOX) {
    for(hitbox h : armorboxes) { if(h.collides(in_box)) { return true; } }
  }
  else if(type == hitbox::TYPE_SHIELDBOX) {
    for(hitbox h : shieldboxes) { if(h.collides(in_box)) { return true; } }
  }
  else if(type == hitbox::TYPE_PICKUPBOX) {
    for(hitbox h : pickupboxes) { if(h.collides(in_box)) { return true; } }
  }
  else if(type == hitbox::TYPE_VACUUMBOX) {
    for(hitbox h : vacuumboxes) { if(h.collides(in_box)) { return true; } }
  }

  return false;
}

bool hittable::collides(const hitline& in_line, int type) const {
  //determine the type of hitbox to check
  if(type == hitbox::TYPE_HITBOX) {
    for(hitbox h : hitboxes) { if(h.collides(in_line)) { return true; } }
  }
  else if(type == hitbox::TYPE_HURTBOX) {
    for(hitbox h : hurtboxes) { if(h.collides(in_line)) { return true; } }
  }
  else if(type == hitbox::TYPE_WEAKBOX) {
    for(hitbox h : weakboxes) { if(h.collides(in_line)) { return true; } }
  }
  else if(type == hitbox::TYPE_ARMORBOX) {
    for(hitbox h : armorboxes) { if(h.collides(in_line)) { return true; } }
  }
  else if(type == hitbox::TYPE_SHIELDBOX) {
    for(hitbox h : shieldboxes) { if(h.collides(in_line)) { return true; } }
  }
  else if(type == hitbox::TYPE_PICKUPBOX) {
    for(hitbox h : pickupboxes) { if(h.collides(in_line)) { return true; } }
  }
  else if(type == hitbox::TYPE_VACUUMBOX) {
    for(hitbox h : vacuumboxes) { if(h.collides(in_line)) { return true; } }
  }

  return false;
}

void hittable::update_boxes() {
  //called AFTER all other updates are done, in order to prevent
  //hitboxes from lagging behind the model

  //update location of the hitboxes, based on their positions
  for(std::vector<hitbox> *v : boxes) {
    for(hitbox &h : *v) {
      h.set_box_center(pos, last_angle);
    }
  }
}

void hittable::draw_boxes() const {

  for(std::vector<hitbox> *v : boxes) {
    for(hitbox &h : *v) {
      h.draw();
    }
  }
}
