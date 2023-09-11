#include "src/movers/drawable/mortal/player.h"
#include "src/movers/drawable/mortal/weapon.h"

player::player(const std::string &path) :
  player(path, vec2d(0, 0), vec2d(0, 0))
{ }

player::player(const std::string &path, const vec2d &p, const vec2d &v) :
  gunner(path, p, v),
  heat(0),
  max_heat(xmlparse::get().get_xml_float(
    path + "/heat/max_heat", true, 100
  )),
  max_overheat(xmlparse::get().get_xml_float (
    path + "/heat/max_overheat", true, 50
  )),
  boost_heat_per_tick(xmlparse::get().get_xml_float(
    path + "/heat/boost_heat_per_tick", true, 0
  )),
  regen_heat_per_tick(xmlparse::get().get_xml_float(
    path + "/heat/regen_heat_per_tick", true, 0
  )),
  cool_per_tick(xmlparse::get().get_xml_float(
    path + "/heat/cool_per_tick", true, 1
  )),
  vent_cool_per_tick(xmlparse::get().get_xml_float(
    path + "/heat/vent_heat_per_tick", true, 1
  )),
  vent_startup_tick_delay(xmlparse::get().get_xml_float(
    path + "/heat/vent_delay_tick", true, 0
  )),
  boost_vel_multiplier(xmlparse::get().get_xml_float(
    path + "/movement/vel_boost", true, 1
  )),
  boost_accel_multiplier(xmlparse::get().get_xml_float(
    path + "/movement/vel_boost_accel", true, 1
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
  is_boost(false),
  last_vent_toggle_tick(0)
{ }

void player::shoot(const vec2d &angle) {
  //check if we're burnt out OR venting
  if(is_burnout || is_vent) { return; }

  //check if we're limited by the fire rate
  if(!can_shoot()) { return; }

  //alright, shoot
  gunner::shoot(angle);

  //now heat up
  heat_up(weapon::get_heat_from_id(weapon_id));
}


void player::start_boost() {
  vel_cap_mod = boost_vel_multiplier * boost_vel_multiplier_mod;
  vel_accel_mod = boost_accel_multiplier * boost_accel_multiplier_mod;
  is_boost = true;
}

void player::stop_boost() {
  vel_cap_mod = 1;
  vel_accel_mod = 1;
  is_boost = false;
}

void player::boost(bool b) {
  //can't boost when burnt out
  if(is_burnout) { 
    stop_boost();
    return; 
  }

  //adjust the velocity and acceleration modifiers in movable
  if(b) {
    start_boost();
  }
  else {
    stop_boost();
  }
}

void player::toggle_vent() {
  //TODO: add visual indicator when venting
  //can't vent when there's no heat
  if(heat <= 0) { is_vent = false; return; }

  //can't STOP venting if burnt out
  if(is_burnout) { is_vent = true; return; }

  //if we're venting, stop
  if(is_vent) { is_vent = false; }
  //if we're NOT venting, set the last toggle tick and wait for the update
  //function to start the venting
  else { last_vent_toggle_tick = t_frame::get().get_t(); }
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
  if(!is_overheat && heat > max_heat * max_heat_mod) {
    //we just overheated - handle it here
    is_overheat = true;
  }

  //check for burnout, take damage and start venting if we are
  if(!is_burnout && heat > max_heat * max_heat_mod + max_overheat * max_overheat_mod) {
    //we just burnt out - handle it here
    is_burnout = true;

    //cap heat at max
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

  //if we're overheated but not burnt out, it's easy to cool off
  if(is_overheat && !is_burnout && heat < max_heat * max_heat_mod * .9) {
    is_overheat = false;
  }

  //if we ARE burnt out, though, can't drop that status till we hit 0, and
  //systems are affected
  if(is_burnout) {
    if(heat <= 0) {
      //cooled enough
      is_overheat = false;
      is_burnout = false;
    }
    else {
      //force immediate venting
      is_vent = true;
    }
  }

  //if venting was requested (last_vent_toggle_tick != 0), check to see if 
  //enough time has passed to let us start venting
  if(last_vent_toggle_tick && 
      last_vent_toggle_tick 
      + (vent_startup_tick_delay * vent_startup_tick_delay_mod)
      < t_frame::get().get_t()) 
  {
    //we've passed the time we needed to wait
    last_vent_toggle_tick = 0;
    is_vent = true;
  }

  //if we're at no heat, deactivate venting
  if(heat <= 0 && is_vent) {
    toggle_vent();
  }

  //venting forcibly disables boosting and health regen (shields unaffected)
  if(is_vent) {
    stop_boost();
    if(is_regen_h()) { toggle_regen_h(); }
  }

//---- add or remove heat -----------------------------------------------------

  //let gunner (and mortal) run first - they may want to deactivate health
  //regen if the player is fully healed
  gunner::update();

  //boosting generates heat
  if(is_boost) { 
    heat_up(boost_heat_per_tick * boost_heat_per_tick_mod * t_frame::get().t_adjust() * t_frame::get().get_t_dilate()); 
  }

  //regenerating generates heat
  if(is_regen_h()) {
    heat_up(regen_heat_per_tick * regen_heat_per_tick_mod * t_frame::get().t_adjust() * t_frame::get().get_t_dilate());
  }

  //venting removes heat fast...
  if(is_vent) {
    heat_up(-1 * vent_cool_per_tick * vent_cool_per_tick_mod * t_frame::get().t_adjust() * t_frame::get().get_t_dilate());
  }
  //...but it will dissipate on its own naturally
  else {
    heat_up(-1 * cool_per_tick * cool_per_tick_mod * t_frame::get().t_adjust() * t_frame::get().get_t_dilate());
  }

  //clamp heat at 0
  if(heat < 0) { heat = 0; }

}
