#ifndef MORTAL_DEBUG_FOLLOWER_H_
#define MORTAL_DEBUG_FOLLOWER_H_

#include "src/movers/drawable/mortal/mortal.h"

class d_follower : public mortal {
public:
  d_follower(const std::string path);

  void set_player_pos(const vec2d &v) { player_pos = v; }

  virtual void update() override;
  void draw() const override;

private:
  vec2d player_pos;
};

#endif
