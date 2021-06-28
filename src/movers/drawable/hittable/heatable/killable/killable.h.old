#ifndef KILLABLE_H_
#define KILLABLE_H_

#include "src/movers/drawable/hittable/heatable/heatable.h"

class killable : public heatable {
public:
  killable(const std::string path);
  virtual ~killable() = default;

  float get_health() const { return curr_health; }
  float get_health_percent() const { return curr_health / max_health; }
  int get_shields() const { return curr_shield_segments; }
  float get_shield_percent() const { return curr_shields / max_shields; }

  bool is_regen() const { return is_regenerating; }
  void toggle_regen() { is_regenerating = !is_regenerating; }

  bool take_damage(float dmg);

  void draw() const override;
  virtual void destroy() override { heatable::destroy(); }
  virtual void update() override;

protected:
  //health operates on a "bars" principle.
  //if health is divided into, for example, 5 bars, the player can only 
  //regenerate up to the current bar, and if they fall below a bar's threshold,
  //they cannot passively regenerate up to it again.
  //however, players can only lose up to two bars at a time - massive damage
  //can remove the current bar and the next one, but no more than that. more 
  //bars means less passive regen but also less damage from massive attacks.
  //passive healing is toggled, and generates heat.
  float max_health;
  float curr_health;
  int health_segments;
  bool is_regenerating;
  float h_regen_rate;
  float h_regen_heat_gen;
  float damage_resist;

  //shields operate on a similar "bars" principle, with a slight tweak.
  //ANY damage destroys 1 and only 1 bar of shield, be it small or massive.
  //shields constantly regenerate without generating heat. the last bar of
  //shield is much larger than the others, meaning if all shields are lost, 
  //they take much longer to recharge, but as long as at least 1 bar remains,
  //the remainder will return quickly.
  float max_shields = 100;  //always locked at 100 since shields are bar based
  float curr_shields = 100;  
  int max_shield_segments;  //total number of segments, can be 0
  int curr_shield_segments;  //current number of segments, can be 0
  float s_regen_rate;  //how quick shields restore themselves
  float first_s_size;  //a number from 0 to 1 representing how much of the
                       //shield bar is taken up by the first segment 

private:



};

#endif
