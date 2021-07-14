#ifndef MORTAL_PLAYER_H_
#define MORTAL_PLAYER_H_

//the player class is an extension of the mortal class, and is meant to more
//completely encapsulate the player's behaviors without leaking them into 
//other classes. 

#include "src/movers/drawable/mortal/mortal.h"

class player : public mortal {
public:
  player(const std::string &path);
  player(const std::string &path, const vec2d &pos, const vec2d &vel);
  virtual ~player() = default;

  virtual void move_up() = 0;
  virtual void move_dn() = 0;
  virtual void move_lf() = 0;
  virtual void move_rt() = 0;

  void draw() const override;
  virtual void update() override;

protected:
  
//heat, shield heat gen, health heat gen, overheat, weapon, 

private:

};

#endif
