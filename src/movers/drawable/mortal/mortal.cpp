#include "src/movers/drawable/mortal/mortal.h"
#include "src/rect2d/rect2d.h"
#include "src/entity_handler/entity_handler.h"
#include "src/utils/message.h"

//==== CONSTRUCTORS ===========================================================

mortal::mortal(const std::string &path) :
  mortal(path, vec2d(0, 0), vec2d(0, 0))
{ }

mortal::mortal(const std::string &path, const vec2d &p, const vec2d &v) :
  drawable(path, p, v),

  tangible(true),

  //get max health - default to 1 np, which creates the entity
  max_health(xmlparse::get().safe_get_xml_float(
    path + "/health/health_max", 1
  )),
  curr_health(max_health),

  //get health segments - default to 1, which makes a single bar
  health_segments(xmlparse::get().safe_get_xml_float(
    path + "/health/health_segments", 1
  )),

  is_regenerating(false),
  //get regen rate - default to 0, meaning no regen
  h_regen_rate(xmlparse::get().safe_get_xml_float(
    path + "/health/health_regen_rate", 0
  )),

  //get regen heat gen - default to 0, making regen free with no heat cost
  h_regen_heat_gen(xmlparse::get().safe_get_xml_float(
    path + "/health/health_regen_heat", 0
  )),

  //get max shield segments - default to 0, meaning no shields
  max_shield_segments(xmlparse::get().safe_get_xml_int(
    path + "/health/shield_segments", 0
  )),
  curr_shield_segments(max_shield_segments),

  //get regen rate - default to 0, meaning no shield regen
  s_regen_rate(xmlparse::get().safe_get_xml_float(
    path + "/health/shield_regen_rate", 0
  )),

  //get first shield size - arbitrarily make first size 0.5?
  //that'll make the first shield seg take half the bar
  first_s_size(xmlparse::get().safe_get_xml_float(
    path + "/health/shield_first_seg_size", 0.5
  )),

  //weak boxes take 4x damage by default
  weakbox_scale(xmlparse::get().safe_get_xml_float(
    path + "/health/scaling/weak", 4
  )),

  //hurtboxes take 1x damage by default
  hurtbox_scale(xmlparse::get().safe_get_xml_float(
    path + "/health/scaling/hurt", 1
  )),

  //armor takes 1/4x damage by default
  armorbox_scale(xmlparse::get().safe_get_xml_float(
    path + "/health/scaling/hurt", 1
  ))
{
  //load all hitboxes into their vectors
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

  //all the vectors are ready - push it into one big vector
  //(they're ordered this way so they draw nicely when debugging)
  boxes.push_back(&vacuumboxes);
  boxes.push_back(&hurtboxes);

  boxes.push_back(&weakboxes);
  boxes.push_back(&hitboxes);
  boxes.push_back(&armorboxes);
  boxes.push_back(&shieldboxes);

  boxes.push_back(&pickupboxes);

  //the boxes want to be updated and calibrated before they load properly
  update_boxes();
  calibrate_boxes();
}

//==== GENERAL TRAITS =========================================================

//hotboxes requrie recalibration if an entity moves to a new position, due to
//their trailing "lines". (if a box moves too fast, it trails a line behind it
//to aid collision; the recalibration prevents this line from causing issues)
void mortal::set_pos(float x, float y) {
  movable::set_pos(x, y);
  update_boxes();
  calibrate_boxes();
}

void mortal::draw() const {
  drawable::draw();
  if(e_handler::get().get_draw_debug_info()) {
    draw_boxes();
  }
}

void mortal::draw_boxes() const {
  for(std::vector<hy_box> *v : boxes) {
    for(hy_box h : *v) {
      h.draw();
    }
  }
}

void mortal::update() {
  drawable::update();

  //update the hitboxes
  mortal::update_boxes();
}

//==== HITBOX AND COLLISION ===================================================

//each of these collision functions checks one incoming collision-capable 
//object against all of this entity's collision-capable objects of a specific
//type, where "type" means a type of hitbox, i.e. hurtbox, armorbox, etc

//check this entity against a single box
bool mortal::collides(const hitbox &in_box, int type) const {
  std::vector<hy_box> boxes = get_box_ref(type);
  for(hy_box h : boxes) { if(h.collides(in_box)) { return true; } }
  
  return false;
}

//check this entity against a single line
bool mortal::collides(const hitline &in_line, int type) const {
  std::vector<hy_box> boxes = get_box_ref(type);
  for(hy_box h : boxes) { if(h.collides(in_line)) { return true; } }
  
  return false;
}

//check this entity against a single box and possibly also a single line
bool mortal::collides(const hy_box &in_hybox, int type) const {
  std::vector<hy_box> boxes = get_box_ref(type);
  for(hy_box h : boxes) { if(h.collides(in_hybox)) { return true; } }

  return false;
}

//check all collision-capable objects of one type of entity with all
//collision-capable objects of this entity, specifying the type of both
bool mortal::collides(const mortal *other, int other_type, int type) const {
  std::vector<hy_box> boxes = get_box_ref(type);
  for(hy_box h : boxes) { if(other->collides(h, other_type)) { return true; } }

  return false;
}

std::vector<hy_box> const &mortal::get_box_ref(int type) const {
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

void mortal::calibrate_boxes() {
  //after creating / recycling a hittable, the boxes are recalibrated to 
  //prevent them from drawing lines to their last position or "snapping" there
  //unexpectedly
  for(std::vector<hy_box> *v : boxes) {
    for(hy_box &h : *v) {
      h.calibrate();
    }
  }
}

void mortal::update_boxes() {
  //after every other update is done, update the boxes last so they don't lag
  //behind the model of the ship

  //update their locations based on their positions - the boxes have their own
  //update logic related to how fast they're moving
  for(std::vector<hy_box> *v : boxes) {
    for(hy_box &h : *v) {
      h.set_box_center(pos, last_angle);
    }
  }
}

//==== DAMAGE AND HEALTH ======================================================

bool mortal::take_damage(float damage, int box_type_hit) {
  if(damage < 0 || tangible == false) { return false; }

  if(box_type_hit == hitbox::TYPE_SHIELDBOX) {
    return false;
  }  
  else if(box_type_hit == hitbox::TYPE_WEAKBOX) {
    curr_health -= damage * weakbox_scale;
    return true;
  }
  else if(box_type_hit == hitbox::TYPE_HURTBOX) {
    curr_health -= damage * hurtbox_scale;
    return true;
  }
  else if(box_type_hit == hitbox::TYPE_ARMORBOX) {
    curr_health -= damage * armorbox_scale;
    return true;
  }

  return true;
}

void mortal::perish() {
  tangible = false;
  active = false;
}
