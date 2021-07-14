#ifndef MORTAL_DEBUG_PLAYER_H_
#define MORTAL_DEBUG_PLAYER_H_

#include "src/movers/drawable/mortal/player.h"

class d_player : public player {
public:
  d_player(const std::string);

  void move_up();
  void move_dn();
  void move_lf();
  void move_rt();

  void boost(bool);
  uint8_t get_weapon_id() const;
  bool can_fire();
  const std::string get_weapon() const { return weapon; }

  void update() override;
  void draw() const override;

private:
  std::string weapon;
};

#endif
