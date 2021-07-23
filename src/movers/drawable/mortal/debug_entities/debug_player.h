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


  void update() override;
  void draw() const override;

private:
};

#endif
