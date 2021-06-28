#include "hittable.h"
#include "src/rect2d/rect2d.h"
#include "src/rect2d/hitbox/hitbox.h"
#include "src/rect2d/hitbox/hitline.h"
#include "src/movers/drawable/hittable/hittable.h"
#include "src/entity_handler/entity_handler.h"
#include "src/rect2d/hitbox/hybrid_box.h"
#include "src/utils/message.h"

hittable::hittable(const std::string path) :
  hittable(path, vec2d(0,0), vec2d(0,0))
{ }

hittable::hittable(const std::string path, const vec2d p, const vec2d v) :
  drawable(path, p, v),
  team(xmlparse::get().get_xml_string(path + "/team"))
{
  //load all hitboxes
  for(std::string p : xmlparse::get().get_all_child_tags(path + "/hitboxes")) {
    hy_box h(path + "/hitboxes/" + p);

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

  update_boxes();
  calibrate_boxes();
}

void hittable::update() {
  drawable::update();

  //update hitboxes
  hittable::update_boxes();
}

bool hittable::collides(const hitbox& in_box, int type) const {
  //determine the type of hitbox to check
  std::vector<hy_box> boxes = get_box_ref(type);
  for(hy_box h : boxes) { if(h.collides(in_box)) { return true; } }

  return false;
}

bool hittable::collides(const hitline& in_line, int type) const {
  //determine the type of hitbox to check
  std::vector<hy_box> boxes = get_box_ref(type);
  for(hy_box h : boxes) { if(h.collides(in_line)) { return true; } }

  return false;
}

bool hittable::collides(const hy_box& in_box, int type) const {
  //determine the type of hitbox to check
  std::vector<hy_box> boxes = get_box_ref(type);
  for(hy_box h : boxes) { if(h.collides(in_box)) { return true; } }

  return false;
}


bool hittable::collides(
  const hittable *other, 
  int other_type, 
  int this_type
) const {

  std::vector<hy_box> boxes = get_box_ref(this_type);
  for(hy_box h : boxes) {
    if(other->collides(h, other_type)) { return true; }
  }
  
  return(false);  
}

void hittable::set_pos(float x, float y) {
  movable::set_pos(x, y);
  update_boxes();
  calibrate_boxes();
}

void hittable::calibrate_boxes() {
  //called AFTER initial creation / recycling of a hittable, in order to reset
  //the lines on the hybrid boxes
  for(std::vector<hy_box> *v : boxes) {
    for(hy_box &h : *v) {
      h.calibrate();
    }
  }
}

//given an int representing a hitbox type, return a ref to the vector
//containing said hitbox type
std::vector<hy_box> const &hittable::get_box_ref(int type) const {
  if(type == hitbox::TYPE_HITBOX) {
    return(hitboxes);
  }
  else if(type == hitbox::TYPE_HURTBOX) {
    return(hurtboxes);
  }
  else if(type == hitbox::TYPE_WEAKBOX) {
    return(weakboxes);
  }
  else if(type == hitbox::TYPE_ARMORBOX) {
    return(armorboxes);
  }
  else if(type == hitbox::TYPE_SHIELDBOX) {
    return(shieldboxes);
  }
  else if(type == hitbox::TYPE_PICKUPBOX) {
    return(pickupboxes);
  }
  else if(type == hitbox::TYPE_VACUUMBOX) {
    return(vacuumboxes);
  }
  else {
    msg::print_error("requested invalid hitbox type!");
    throw("bad box type");
  } 
}

void hittable::update_boxes() {
  //called AFTER all other updates are done, in order to prevent
  //hitboxes from lagging behind the model

  //update location of the hitboxes, based on their positions
  for(std::vector<hy_box> *v : boxes) {
    for(hy_box &h : *v) {
      h.set_box_center(pos, last_angle);
    }
  }
}

void hittable::draw() const {
  if(e_handler::get().get_draw_debug_info()) {
    draw_boxes();
  }
}

void hittable::draw_boxes() const {

  for(std::vector<hy_box> *v : boxes) {
    for(hy_box &h : *v) {
      h.draw();
    }
  }
}
