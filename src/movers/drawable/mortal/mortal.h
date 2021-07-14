#ifndef MORTAL_H_
#define MORTAL_H_

//the mortal class is a refactored combination of the hittable and killable
//classes. it contains hybrid boxes that can register hits, and tracks data 
//for health and shielding, as well. this is likely going to encompass every
//entity the player is designed to interact with. 

#include "src/movers/drawable/drawable.h"
#include "src/rect2d/hitbox/hitbox.h"
#include "src/rect2d/hitbox/hitline.h"
#include "src/rect2d/hitbox/hybrid_box.h"
#include "src/xml_parser/xmlparse.h"
#include <vector>

class mortal : public drawable {
public:
//==== GENERAL MOVABLE TRAITS =================================================

  mortal(const std::string &path);
  mortal(const std::string &path, const vec2d &pos, const vec2d &vel);
  virtual ~mortal() = default;

  //overridden here because the hitboxes must be updated each time the ship
  //jumps to a new location
  void set_pos(const vec2d &p) { this->set_pos(p[0], p[1]); }
  void set_pos(float x, float y);

  void draw() const override;
  void draw_boxes() const;

  virtual void update() override;

//==== HITBOX AND COLLISION ===================================================

  //takes an incoming box, line, hybrid, or even mortal, and checks for a 
  //collision with a specific type of hitbox
  virtual bool collides(const hitbox &h, int type) const;
  virtual bool collides(const hitline &l, int type) const;
  virtual bool collides(const hy_box &hy, int type) const;
  virtual bool collides(const mortal *other, int other_type, int this_type) const;

  bool is_tangible() const { return active && tangible; }
  void set_tangible(bool b) { tangible = b; }

  void calibrate_boxes();

//==== DAMAGE AND HEALTH ======================================================
  
  float get_health() const { return curr_health; }
  float get_health_percent() const { return curr_health / max_health; }
  float get_shields() const { return curr_shield_segments; }
  float get_shield_percent() const { return curr_shields / max_shields; }

  bool is_regen_h() const { return h_is_regenerating; }
  void toggle_regen_h() { h_is_regenerating = !h_is_regenerating; }
  bool is_regen_s() const { return s_is_regenerating; }
  void toggle_regen_s() { s_is_regenerating = !s_is_regenerating; }

  //damage scales depending on which box was struck
  virtual bool take_damage(float damage, int box_type_hit);

  //when health hits 0, the entity usually becomes intangible and begins a
  //death animation of some sort before becoming invisible. this handles
  //such an event.
  virtual void perish();

//=============================================================================
//=============================================================================

protected:
//==== HITBOX AND COLLISION ===================================================

  //returns a reference to a subset of total boxes
  std::vector<hy_box> const &get_box_ref(int type) const;
  
  void update_boxes();

  //holds the vectors of the other boxes for ease of updating
  std::vector<std::vector<hy_box> *> boxes;

  //boxes are split into 1 + 6 "types" 
  //- hitboxes deal damage and are commonly found on weapons
  //
  //- hurtboxes take normal damage and are commonly found on entities, and are
  //    usually ABSENT from weapons unless they're destroyable en route
  //- weakboxes take increased damage
  //- armorboxes take reduced damage
  //- shieldboxes take no damage (but still deplete shields if hit)
  //- pickupboxes are used for item entities that can be collected
  //- vacuumboxes are used to collide with pickupboxes to absorb them
  std::vector<hy_box> hitboxes;

  std::vector<hy_box> hurtboxes;
  std::vector<hy_box> weakboxes;
  std::vector<hy_box> armorboxes;
  std::vector<hy_box> shieldboxes;
  std::vector<hy_box> pickupboxes;
  std::vector<hy_box> vacuumboxes;

  //"tangibility" is an entity's ability to be struck. all inactive entities
  //are intangible - common other reasons for intangibility are death
  //animations, where the entity no longer blocks shots, but is still being 
  //rendered and updated and is thus not yet "inactive". 
  //intangible entities do not check for collision and cannot interact with
  //other hitboxes.
  bool tangible;

//==== DAMAGE AND HEALTH ======================================================

  //health operates on a "bars" principle.
  //if health is divided into, for example, 5 bars, the player can only 
  //regenerate up to the current bar, and if they fall below a bar's threshold,
  //they cannot passively regenerate up to it again.
  //however, players can only lose up to two bars at a time - massive damage
  //can remove the current bar and the next one, but no more than that. more 
  //bars means less passive regen but also less damage from massive attacks.
  //passive healing is toggled, and will, for players, generate "heat".
  float max_health;
  float curr_health;
  int health_segments;
  bool h_is_regenerating;
  float h_regen_rate;

  void do_health_regen();

  //shields operate on a similar "bars" principle, with a slight tweak.
  //ANY damage destroys 1 and only 1 bar of shield, be it small or massive.
  //shields constantly regenerate without generating heat. the last bar of
  //shield is much larger than the others, meaning if all shields are lost, 
  //they take much longer to recharge, but as long as at least 1 bar remains,
  //the remainder will return quickly.
  float max_shields = 100;  //always locked at 100 since shields are bar based
  float curr_shields = 100;  //percent based value for "recharging" behavior
  int max_shield_segments;  //total number of segments, can be 0
  int curr_shield_segments;  //current number of segments, can be 0
  bool s_is_regenerating;
  float s_regen_rate;  //how quick shields restore themselves
  float first_s_size;  //a number from 0 to 1 representing how much of the
                       //shield bar is taken up by the first segment 
  void do_shield_regen();

  //damage being dealt can be scaled depending on what box is hit. as a general
  //rule, weakboxes take extra damage and armorboxes take reduced damage. 
  //the damage taken by boxes is adjustable both on
  //initial spawn and later on during gameplay. in practice this means the 
  //player and some enemies can adjust how resistant they are to damage, and
  //more powerful enemies can scale their damage resistance based on the player

  //TODO: add these to an array for reduced code in take_damage?
  float weakbox_scale = 4;  //usually 4x damage in a weak box
  float hurtbox_scale = 1;  //usually 1x damage in a hurt box
  float armorbox_scale = .25;  //usually 1/4x damage in an armor box

private: 

};

#endif
