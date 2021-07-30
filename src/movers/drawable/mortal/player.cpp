#include "src/movers/drawable/mortal/player.h"
#include "src/movers/drawable/mortal/weapon.h"

player::player(const std::string &path) :
  player(path, vec2d(0, 0), vec2d(0, 0))
{ }

player::player(const std::string &path, const vec2d &p, const vec2d &v) :
  gunner(path, p, v),
  heat(0),
  max_heat(xmlparse::get().safe_get_xml_float(
    path + "/heat/max_heat", 100
  )),
  max_overheat(xmlparse::get().safe_get_xml_float (
    path + "/heat/max_overheat", 50
  )),
  boost_heat_per_tick(xmlparse::get().safe_get_xml_float(
    path + "/heat/boost_heat_per_tick", 0
  )),
  regen_heat_per_tick(xmlparse::get().safe_get_xml_float(
    path + "/heat/regen_heat_per_tick", 0
  )),
  cool_per_tick(xmlparse::get().safe_get_xml_float(
    path + "/heat/cool_per_tick", 1
  )),
  vent_cool_per_tick(xmlparse::get().safe_get_xml_float(
    path + "/heat/vent_heat_per_tick", 1
  )),
  vent_startup_tick_delay(xmlparse::get().safe_get_xml_float(
    path + "/heat/vent_delay_tick", 0
  )),
  boost_vel_multiplier(xmlparse::get().safe_get_xml_float(
    path + "/movement/vel_boost", 1
  )),
  boost_accel_multiplier(xmlparse::get().safe_get_xml_float(
    path + "/movement/vel_boost_accel", 1
  )),
  max_heat_mod(1),
  max_overheat_mod(1),
  boost_heat_per_tick_mod(1),
  regen_heat_per_tick_mod(1),
  cool_per_tick_mod(1),
  vent_cool_per_tick_mod(1),
  vent_startup_tick_delay_mod(1),
  boost_vel_multiplier_mod(1),
  boost_accel_multiplier_mod(1),
  is_overheat(false),
  is_burnout(false),
  is_vent(false),
  is_boost(false)
{ }

void player::shoot(const vec2d &angle) {
  //check if we're burnt out
  if(is_burnout) { return; }

  //check if we're limited by the fire rate
  if(!can_shoot()) { return; }

  //alright, shoot
  gunner::shoot(angle);

  //now heat up
  heat_up(weapon::get_heat_from_id(weapon_id));
}

void player::boost(bool b) {
  //can't boost when burnt out
  if(is_burnout) { is_boost = false; return; }

  is_boost = b;

  //adjust the velocity and acceleration modifiers in movable
  if(is_boost) {
    vel_cap_mod = boost_vel_multiplier * boost_vel_multiplier_mod;
    vel_accel_mod = boost_accel_multiplier * boost_accel_multiplier_mod;
  }
  else {
    vel_cap_mod = 1;
    vel_accel_mod = 1;
  }
}

float player::get_heat_frac() {
  return (heat < max_heat * max_heat_mod ? heat / (max_heat * max_heat_mod) : 1); 
}

float player::get_overheat_frac() { 
  if(!is_overheat) { return 0; }
  float frac = (heat - (max_heat * max_heat_mod)) / (max_overheat * max_overheat_mod); 
  return frac > 0 ? frac : 0;
}

void player::draw() const {
  gunner::draw();
}

void player::update() {

//---- heat checks ------------------------------------------------------------
  //check to see if we're overheated
  if(heat > max_heat * max_heat_mod) {
    is_overheat = true;
  }
  //check for burnout, take damage and start venting if we are
  if(!is_burnout && heat > max_heat * max_heat_mod + max_overheat * max_overheat_mod) {
    is_burnout = true;
    heat = max_heat * max_heat_mod + max_overheat * max_overheat_mod;
    //burnout always destroys current + next bar
    //this IS LETHAL for ships < 3 bars
    take_damage(max_health, -1, true);
  }

  //if overheated or burnt out, check if we're below threshold again
  //we can freely pass to and from overheat as long as we aren't burntout
  //but if we burnout then we're stuck until we hit 0
  //note that once overheated (100%) we have to drop below a threshold 
  //(90%) to prevent jittering when right at the boundary
  if(is_overheat && !is_burnout && heat < max_heat * max_heat_mod * .9) {
    is_overheat = false;
  }

  //if we ARE burnt out, can't drop that status till we hit 0
  if(is_burnout && heat <= 0) {
    if(heat <= 0) {
      is_overheat = false;
      is_burnout = false;
    }
    else {
      //force venting if we're burnt out
      is_vent = true;
    }
  }

  if(is_vent) {
    //TODO: vent delay
    //not allowed to boost or regen health when venting
    //(regenerating shields is fine)
    boost(false);
    if(is_regen_h()) { toggle_regen_h(); }
  }

//---- add or remove heat -----------------------------------------------------

  //let gunner (and mortal) run first so they can cancel regen if needed
  gunner::update();

  if(is_boost) { 
    heat_up(boost_heat_per_tick * boost_heat_per_tick_mod); 
  }

  if(is_regen_h()) {
    heat_up(regen_heat_per_tick * regen_heat_per_tick_mod);
  }

  if(is_vent) {
    heat_up(-1 * vent_cool_per_tick * vent_cool_per_tick_mod);
  }
  else {
    heat_up(-1 * cool_per_tick * cool_per_tick_mod);
  }

  if(heat < 0) { heat = 0; }

}
