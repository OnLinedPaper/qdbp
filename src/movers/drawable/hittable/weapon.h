#ifndef WEAPON_H_
#define WEAPON_H_

#include "src/movers/drawable/hittable/hittable.h"

/*
this is intended to act as an independent projectile - upon firing a weapon,
one of these is generated and begins its movement.
the projectile lifetime and other things are to be decided
elsewhere and passed in, though these will have default values; current
theory is a default value is loaded and a multiplier is passed in. 
THESE WILL BE RECYCLED. it'd be a hassle to delete and reallocate these things
over and over, so upon "destruction" the projectile will just become invisible
and upon "creation" will be made visible and given new coordinates/locations.
*/

class weapon : public hittable {
public:
  weapon(const std::string path);
  ~weapon();

  void draw() const override;
  virtual void update() override;

  //returns the generated heat from this shot
  float fire(const vec2d start_pos, const vec2d start_vel, 
      const vec2d angle, 
      float heat_mod, float life_ms_mod, 
      float life_dist_mod, float inacc_mod,
      float vel_mod, const SDL_Color &);

  bool is_type(uint8_t type_id) const {
    return !(type_id ^ this->type_id);
  }

  void move_up() override {}
  void move_dn() override {}
  void move_lf() override {}
  void move_rt() override {}

  void set_pos(const vec2d p) { this->set_pos(p[0], p[1]); }
  void set_pos(float x, float y) { hittable::set_pos(x, y); }

private:
  //the heat generated per shot
  float heat_per_shot;
  //the delay between shots in ms
  float ms_lifespan;
  //the distance the projectile can travel before expiring
  float dist_lifespan;
  //the random variance the projectile has on spawn
  float inaccuracy;


  //below are "modifiers" the projectile will use for this current "life"

  //the velocity modifier is NOT saved since it's only used for firing 
  //float vel_mod;
  //heat modifier is NOT saved since it's only used on firing
  //float heat_mod;
  //the time it has to live
  float life_ms_mod;
  //the distance it has to live
  float life_dist_mod;
  //inaccuracy modifier is NOT saved since it's only used on firing
  //float inacc_mod;

  //the constant velocity of this projectile
  float con_vel;

  //this is used to identify the type of projectile
  uint8_t type_id;

  std::string xml_path = "/movers/hittable/";
};

#endif
