#include "killable.h"
#include "src/timeframe/timeframe.h"

killable::killable(const std::string path) :
  heatable(path),
  max_health(xmlparse::get().get_xml_float( 
    path + "/health/health_max"
  )),
  curr_health(max_health),
  health_segments(xmlparse::get().get_xml_int(
    path + "/health/health_segments"
  )),
  is_regenerating(false),
  h_regen_rate(xmlparse::get().get_xml_float(
    path + "/health/health_regen_rate"
  )),
  h_regen_heat_gen(xmlparse::get().get_xml_float(
    path + "/health/health_regen_heat"
  )),
  damage_resist(1),
  max_shield_segments(xmlparse::get().get_xml_int(
    path + "/health/shield_segments"
  )),
  curr_shield_segments(max_shield_segments),
  s_regen_rate(xmlparse::get().get_xml_float(
    path + "/health/shield_regen_rate"
  )),
  first_s_size(xmlparse::get().get_xml_float(
    path + "/health/shield_first_seg_size"
  ))
{ }

bool killable::take_damage(float dmg) {
  if(curr_shield_segments > 0) {
    if(curr_shield_segments == 1) {
      //remove all shields on last segment
      curr_shields = 0;
    }
    else {
      curr_shields -= (max_shields - (max_shields * first_s_size)) / (max_shield_segments - 1);
    }

    curr_shield_segments -= 1;
  }
  else {
    curr_health -= dmg / damage_resist;
  }
  return true;
}

void killable::update() {
  heatable::update();
  
  if(curr_health <= 0) {
    destroy();
    return;
  }

  //check for health regen
  if(is_regenerating && !is_overheat()) {
    //ship can regenerate - can the health?
    float seg_each = max_health / health_segments;
    float seg_max = max_health;

    //get the max we will generate for this segment
    while(seg_max - seg_each >= curr_health) { seg_max -= seg_each; }

    if(curr_health != seg_max) {
      //add health
      curr_health += h_regen_rate * t_frame::get().t_adjust();
      //clamp at segment max
      curr_health = (curr_health > seg_max ? seg_max : curr_health);

      //heat the ship
      heat_up(h_regen_heat_gen * t_frame::get().t_adjust());
    }
    else {
      //turn regen off if health is maxed
      is_regenerating = false;
    }
  }
  else {
    //turn regen off if overheated
    is_regenerating = false;
  }

  //do shield regen
  if(curr_shield_segments < max_shield_segments && !is_overheat()) {
    //regen shields
    curr_shields += s_regen_rate * t_frame::get().t_adjust();
    curr_shields = (curr_shields > max_shields ? max_shields : curr_shields);

    if(curr_shield_segments == 0 && curr_shields >= (max_shields * first_s_size)) {
      curr_shield_segments += 1; 
    }
    else if((curr_shields - (max_shields * first_s_size)) >= 
        ((max_shields - (max_shields * first_s_size)) / (max_shield_segments - 1)) 
        * (curr_shield_segments)) 
    {
      curr_shield_segments += 1;
    }
  }
} 

void killable::draw() const {
  heatable::draw();
}

