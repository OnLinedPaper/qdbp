#ifndef MORTAL_PLAYER_H_
#define MORTAL_PLAYER_H_

//the player class is an extension of the mortal class, and is meant to more
//completely encapsulate the player's behaviors without leaking them into 
//other classes. 

#include "src/movers/drawable/mortal/gunner.h"

class player : public gunner {
public:
  player(const std::string &path);
  player(const std::string &path, const vec2d &pos, const vec2d &vel);
  virtual ~player() = default;

  virtual void move_up() = 0;
  virtual void move_dn() = 0;
  virtual void move_lf() = 0;
  virtual void move_rt() = 0;

  void shoot(const vec2d &);

  void boost(bool b);
  void heat_up(float h) { heat += h; }
  float get_heat_frac();
  float get_overheat_frac();

  bool is_overheated() { return is_overheat; }
  bool is_burntout() { return is_burnout; }

  void draw() const override;
  virtual void update() override;


protected:

//==== HEAT ===================================================================  

  //the player possesses a unique system, "heat" - this is basically a resource
  //pool that depletes as the player perorms actions, such as shooting or 
  //regenerating. heat naturally reduces itself over time, and can be forcibly
  //removed through a few means, such as powerups

  //the following actions increase heat:
  // - moving at "boosted" speed (high)
  // - firing a weapon (med-high)
  // - regenerating health (low)
  // - activating a portal (low)

  //the following actions do not generate heat:
  // - moving at normal speed
  // - regenerating shields 

  //the following actions reduce heat:
  // - "venting" heat (ship cannot perform actions besides moving)

  //players also have a unique mechanic, "overheat". the max heat is not
  //actually a hard limiter; players can overheat their ships as well, pushing
  //their systems past redline. this causes some consequences in gameplay,
  //typically involving sensitive systems that can't work when overheated, and
  //if the player exceeds their overheat as well, their ship takes damage and
  //"burns out", forcibly activates venting until ALL heat is gone

//---- basics -----------------------------------------------------------------

  float heat;                       //the total amount of heat in the ship
  float max_heat;                   //max normal heat, starts overheating after
  float max_overheat;               //burnout occurs if it reaches this point

  float boost_heat_per_tick;        //heat gen for boosting
  float regen_heat_per_tick;        //heat gen for regenerating

  float cool_per_tick;              //heat lost naturally over time
  float vent_cool_per_tick;         //heat lost through venting
  float vent_startup_tick_delay;    //time delay before venting starts

  float boost_vel_multiplier;       //speed multiplier when boosted
  float boost_accel_multiplier;     //acceleration multiplier when boosted

//---- modifiers --------------------------------------------------------------

  float max_heat_mod;               //can heat more before overheat
  float max_overheat_mod;           //can heat more before burnout

  float boost_heat_per_tick_mod;    //higher numbers, more heat when boosting
  float regen_heat_per_tick_mod;    //higher numbers, more heat when regen

  float cool_per_tick_mod;          //higher numbers, more cooling per tick
  float vent_cool_per_tick_mod;     //higher numbers, more cooling per tick
  float vent_startup_tick_delay_mod;  //higher numbers, more time before vent

  float boost_vel_multiplier_mod;   //higher numbers, faster speed
  float boost_accel_multiplier_mod;   //higher numbers, faster accel

//---- booleans ---------------------------------------------------------------

  bool is_overheat;                 //past max_heat, hasn't reached burnout
  bool is_burnout;                  //exceeded max_overheat and is burnt out
  bool is_vent;                     //is venting heat
  bool is_boost;                    //is boosting, which raises velocity cap

private:

};

#endif
