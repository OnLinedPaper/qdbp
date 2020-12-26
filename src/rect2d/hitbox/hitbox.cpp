#include "hitbox.h"
#include "src/vec2d/vec2d.h"
#include <math.h>
#include "src/xml_parser/xmlparse.h"

const int hitbox::TYPE_HITBOX = 0;
const int hitbox::TYPE_HURTBOX = 1;
const int hitbox::TYPE_WEAKBOX = 2;
const int hitbox::TYPE_ARMORBOX = 3;
const int hitbox::TYPE_SHIELDBOX = 4;
const int hitbox::TYPE_PICKUPBOX = 5;
const int hitbox::TYPE_VACUUMBOX = 6;

hitbox::hitbox(float size, const vec2d &offs, int t) :
  rect2d(0, 0, size, size),
  offset(offs),
  type(t)
{ }

hitbox::hitbox(std::string path) :
  rect2d(
    0,
    0,
    xmlparse::get().get_xml_float(path + "/size"),
    xmlparse::get().get_xml_float(path + "/size")
  ),
  offset(
    xmlparse::get().get_xml_float(path + "/x_offset"),
    xmlparse::get().get_xml_float(path + "/y_offset")
  ),
  type(
    str_to_type(xmlparse::get().get_xml_string(path + "/type"))
  )
{ }

void hitbox::set_box_center(const vec2d &pos, float last_angle) {
  //adjust rectangle's position based on last angle and offset
  float PI = 3.141592653589793238462643383279502884;
  float angle = (last_angle * PI) / 180;

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

bool hitbox::collides(const hitbox& h) const {

  return this->overlap(h);
}

bool hitbox::collides(const hitline &l) const {

  return this->overlap(l);
}

int hitbox::str_to_type(std::string s) const {
  if(s.compare("HIT") == 0) { return TYPE_HITBOX; }
  if(s.compare("HURT") == 0) { return TYPE_HURTBOX; }
  if(s.compare("WEAK") == 0) { return TYPE_WEAKBOX; }
  if(s.compare("ARMOR") == 0) { return TYPE_ARMORBOX; }
  if(s.compare("SHIELD") == 0) { return TYPE_SHIELDBOX; }
  if(s.compare("PICKUP") == 0) { return TYPE_PICKUPBOX; }
  if(s.compare("VACUUM") == 0) { return TYPE_VACUUMBOX; }
  return -1;
}

std::string hitbox::type_to_str(int type) const {
  if(type == TYPE_HITBOX) { return "HIT"; }
  if(type == TYPE_HURTBOX) { return "HURT"; }
  if(type == TYPE_WEAKBOX) { return "WEAK"; }
  if(type == TYPE_ARMORBOX) { return "ARMOR"; }
  if(type == TYPE_SHIELDBOX) { return "SHIELD"; }
  if(type == TYPE_PICKUPBOX) { return "PICKUP"; }
  if(type == TYPE_VACUUMBOX) { return "VACUUM"; }
  return "INVALID TYPE";
}

void hitbox::draw() const {
  //color the boxes based on their type
  if(type == TYPE_HURTBOX) { rect2d::draw(0, 200, 0); }
  if(type == TYPE_HITBOX) { rect2d::draw(255, 0, 0); }
  if(type == TYPE_WEAKBOX) { rect2d::draw(255, 255, 0); }
  if(type == TYPE_ARMORBOX) { rect2d::draw(255, 128, 0); }
  if(type == TYPE_SHIELDBOX) { rect2d::draw(0, 255, 255); }
  if(type == TYPE_PICKUPBOX) { rect2d::draw(255, 128, 255); }
  if(type == TYPE_VACUUMBOX) { rect2d::draw(0, 0, 128); }

}

std::ostream &operator<<(std::ostream &output, const hitbox &h) {
  output << "tlc: (" << h.get_tlc()[0] << ", " << h.get_tlc()[1] << ")  size: " << h.get_dims()[0] << "  type: " << h.type_to_str(h.get_type());
  return output;
}
