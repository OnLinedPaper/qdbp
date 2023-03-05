#include "src/movers/drawable/mortal/gunner.h"
#include "src/entity_handler/entity_handler.h"

gunner::gunner(const std::string &path) :
  gunner(path, vec2d(0, 0), vec2d(0, 0))
{ }

gunner::gunner(const std::string &path, const vec2d &p, const vec2d &v) :
  mortal(path, p, v),

  //weapon id of -1 means no weapon at all, though warn the user, since a 
  //weapon-less gunner should really be a mortal instead
  weapon_id(xmlparse::get().get_xml_int(path + "/gunning/weapon_id", true, -1)),
  
  //ms multiplier, makes projectile live longer at higher numbers
  //TODO: add addition alongside the multiplier? same for below
  w_life_ms_mod(xmlparse::get().get_xml_float(
    path + "/gunning/life_tick_multi", true, 1
  )),

  //distance multiplier, make projectile go farther at higher numbers
  w_life_dist_mod(xmlparse::get().get_xml_float(
    path + "/gunning/life_dist_multi", true, 1
  )),

  //inaccuracy multiplier, make projectile LESS accurate at higher numbers
  //("0" makes the projectile perfectly accurate)
  w_inacc_mod(xmlparse::get().get_xml_float(
    path + "/gunning/inacc_multi", true, 1
  )),

  //velocity multiplier, makes projectile faster at higher numbers
  w_vel_mod(xmlparse::get().get_xml_float(
    path + "/gunning/velocity_multi", true, 1
  )),

  //adds the ability to pierce additional targets - note that 3 is max, and
  //values over 3 don't add any further benefit
  w_pierce_mod(xmlparse::get().get_xml_int(
    path + "/gunning/extra_piercing_count", true, 0
  )),

  //damage multiplier, makes projectile deal more damage at higher numbers
  w_damage_mod(xmlparse::get().get_xml_float(
    path + "/gunning/damage_multi", true, 1
  )),

  //firing speed delay multiplier, makes projectile fire faster at LOW numbers
  w_delay_mod(xmlparse::get().get_xml_float(
    path + "/gunning/firing_delay_multi", true, 1
  )),

  //shot angle, defaults to straight up (0, 0)
  shot_angle({0, 0}),

  //tick count since last shot - IS ADJUSTED when game ticks slower
  last_shot_time(t_frame::get().get_t())
{ }

void gunner::shoot(const vec2d &angle) {
  shot_angle = angle;
  shoot();
}

//meant to be overridden in child classes if they want to, say, 
//add heat or the like
//DOES NOT CHECK can_shoot() BEFORE FIRING, this is the responsibility
//of any children
void gunner::shoot() {
  e_handler::get().request_shot(
    this,
    weapon_id, get_pos(), get_vel(), shot_angle, 
    weapon::get_delay_from_id(weapon_id) * w_delay_mod,
    w_life_ms_mod, w_life_dist_mod, w_inacc_mod, w_vel_mod, w_pierce_mod, 
    w_damage_mod, get_col()
  );

  //now updates last shot time
  last_shot_time = t_frame::get().get_t();
}

bool gunner::can_shoot() {
 
  //get elapsed ticks...
  float elapsed_ticks = t_frame::get().get_t() - last_shot_time; 
  //...then scale elapsed time for any lag
  elapsed_ticks *= t_frame::get().t_adjust();

  //get delay between shots...
  float shot_delay = weapon::get_delay_from_id(weapon_id);
  //...and scale it with the delay modifier
  shot_delay *= w_delay_mod;

  //now check to see if enough time has passed to shoot
  //NOTE that if the game's been lagging, the entity handler will start
  //scaling firing speed to accommodate by spawning multiple projectiles
  //as a factor of how hard the game's lagging 
  //(see e_handler for more info)
  if(elapsed_ticks >= shot_delay) {
    return true;
  }

  return false;
}

void gunner::draw() const {
  mortal::draw();
}

void gunner::update() {
  mortal::update();
}
