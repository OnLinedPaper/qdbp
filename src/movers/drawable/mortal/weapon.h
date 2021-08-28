#ifndef MORTAL_WEAPON_H_
#define MORTAL_WEAPON_H_

#include "src/movers/drawable/mortal/mortal.h"

/*
this is an independent projectile, and is handled differently than normal
mortals, with different data as well. most notably, these are recycled and
reused in memory instead of deleted and reallocated; on destruction, it is
removed from update and draw queues, and upon "creation" is given new data
*/

class weapon : public mortal {
public:
  weapon(const std::string path);
  weapon(uint8_t id);
  ~weapon();

  //these are universally accessible tables of basic weapon data
  static float get_heat_from_id(uint8_t);
  static float get_delay_from_id(uint8_t);
  static const std::string get_path_from_id(uint8_t);
  static void preload_weapon_data();

  void draw() const override;
  virtual void update() override;

  void fire(const mortal* parent,
    const vec2d &start_pos, const vec2d &start_vel,
    const vec2d &angle,
    float life_tick_mod,
    float life_dist_mod, float inacc_mod,
    float vel_mod, int pierce_mod,
    float damage_mod, const SDL_Color &);

  bool is_type(uint8_t type_id) const {
    return !(type_id ^ this->type_id);
  }

  virtual void strike_target(mortal &, int);
  float get_damage() { return impact_damage * impact_damage_mod; }

  void set_pos(const vec2d &p) { this->set_pos(p[0], p[1]); }
  void set_pos(float x, float y) { mortal::set_pos(x, y); }

private:

  //private maps let other classes access heat-per-shot and
  //delay-per-shot data
  static std::map<uint8_t, float> id_to_heat;
  static std::map<uint8_t, float> id_to_delay;
  static std::map<uint8_t, std::string> id_to_path;

  //the unique id of this weapon
  uint8_t type_id;
  //the mortal that fired this weapon
  const mortal *parent;

  //the number of ticks the shot can last before expiring
  float life_tick;
  //the distance the projectile can go before expiring
  float life_dist;
  //the random variance from point of aim at spawn
  float inaccuracy;

  //below are constant data pieces
  //projectile's base velocity
  float con_vel;
  //how much damage is dealt on hit
  float impact_damage;
  //how many box types it can hit
  int armor_pierce;

  //below are "modifiers" the projectile uses for its current "life"
  //time to live
  float life_tick_mod;
  //distance to live
  float dist_tick_mod;
  //damage modifier
  float impact_damage_mod;
  //extra piercing
  int armor_pierce_mod;

  //ability to bypass shields - NEVER available by default. EVER.
  bool bypass_shield_mod;

  //TODO: change to "/movers/mortal/weapon" or something?
  std::string xml_path = "/movers/hittable/";
};



#endif
