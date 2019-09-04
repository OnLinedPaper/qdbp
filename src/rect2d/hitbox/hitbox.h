#ifndef HITBOX_H_
#define HITBOX_H_

#include "src/rect2d/rect2d.h"
#include "src/vec2d/vec2d.h"

class hitbox : public rect2d {
public:
  static const int TYPE_HITBOX;
  static const int TYPE_HURTBOX;
  static const int TYPE_WEAKBOX;
  static const int TYPE_SHIELDBOX;

  hitbox(double size, vec2d offset, int type);

  void set_box_center(vec2d pos, double last_angle);

  void draw() const;

private:
  vec2d offset;
  int type;

};

#endif
