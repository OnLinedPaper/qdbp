#ifndef DEBUG_FOLLOWER_H
#define DEBUG_FOLLOWER_H

#include "src/movers/drawable/hittable/hittable.h"

class d_follower : public hittable {
public:
  d_follower(const std::string path);

  void move_up() override;
  void move_dn() override;
  void move_lf() override;
  void move_rt() override;

  void set_player_pos(const vec2d &v) { player_pos = v; }

  virtual void update() override;
  void draw() const override;

private:
  vec2d player_pos;

};

#endif
