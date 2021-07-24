#include "src/movers/drawable/mortal/player.h"

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
    path + "/heat/boost_heat", 0
  )),
  regen_heat_per_tick(xmlparse::get().safe_get_xml_float(
    path + "/heat/regen_heat", 0
  )),
  cool_per_tick(xmlparse::get().safe_get_xml_float(
    path + "/heat/cool_heat", 1
  )),
  vent_cool_per_tick(xmlparse::get().safe_get_xml_float(
    path + "/heat/vent_heat", 1
  )),
  vent_startup_tick_delay(xmlparse::get().safe_get_xml_float(
    path + "/heat/vent_delay", 0
  )),
  max_heat_mod(1),
  max_overheat_mod(1),
  boost_heat_per_tick_mod(1),
  regen_heat_per_tick_mod(1),
  cool_per_tick_mod(1),
  vent_cool_per_tick_mod(1),
  vent_startup_tick_delay_mod(1),
  is_overheat(false),
  is_burnout(false),
  is_vent(false),
  is_boost(false)
{ }

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
  if(heat > max_overheat * max_overheat_mod) {
    is_burnout = true;
    heat = max_overheat * max_overheat_mod;
    //burnout always destroys current + next bar
    //this IS LETHAL for ships < 3 bars
    take_damage(max_health, -1);
  }

  //if overheated or burnt out, check if we're below threshold again
  //we can freely pass to and from overheat as long as we aren't burntout
  //but if we burnout then we're stuck until we hit 0
  if(is_overheat && !is_burnout && heat < max_heat * max_heat_mod) {
    is_overheat = false;
  }
  if(is_burnout && heat <= 0) {
    is_overheat = false;
    is_burnout = false;
  }
  else {
    //force venting if we're burnt out
    is_vent = true;
  }

  if(is_vent) {
    //not allowed to boost or regen health when venting
    //(regenerating shields is fine)
    is_boost = false;
    if(is_regen_h()) { toggle_regen_h(); }
  }

//---- add or remove heat -----------------------------------------------------

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

  gunner::update();
}
