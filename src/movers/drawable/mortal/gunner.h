#ifndef GUNNER_MORTAL_H_
#define GUNNER_MORTAL_H_

//the gunner class is a subclass of mortal that has the ability to shoot a
//weapon; that is, the ability to create a weapon entity (also mortal).
//this is mostly done to separate mortals that DO shoot from those that DON'T,
//a distinction i find nontrivial - part of the refactor that created mortal
//was coarse granularity between the two

#include "src/movers/drawable/mortal/mortal.h"

class gunner : public mortal {
public:
  gunner(const std::string &path);
  gunner(const std::string &path, const vec2d &pos, const vec2d &vel);
  virtual ~gunner() = default;

  //TODO: request that the entity handler fire a weapon from this position
  void shoot(); 

  //TODO: check if shooting is allowed, based on time since last shot
  //other children may check different factors (heat for player)
  bool can_shoot();

  uint8_t get_weapon_id() const { return weapon_id; }

  void draw() const override;
  virtual void update() override;

protected:

private:
  uint8_t weapon_id;
  float w_life_ms_mod;
  float w_life_dist_mod;
  float w_inacc_mod;
  float w_vel_mod;
  int w_pierce_mod;
  float w_damage_mod;
  float w_delay_mod;

  //used to store desired shot angle, commonly calculated in update()
  vec2d shot_angle;

  //used to avoid static vars in functions
  float last_shot_time;
};

#endif
