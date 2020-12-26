#ifndef HITBOX_H_
#define HITBOX_H_

#include "src/rect2d/rect2d.h"
#include "src/vec2d/vec2d.h"
#include "src/rect2d/hitbox/hitline.h"

class hitbox : public rect2d {
public:
  static const int TYPE_HITBOX;
  static const int TYPE_HURTBOX;
  static const int TYPE_WEAKBOX;
  static const int TYPE_ARMORBOX;
  static const int TYPE_SHIELDBOX;
  static const int TYPE_PICKUPBOX;
  static const int TYPE_VACUUMBOX;

  hitbox(float size, const vec2d &offset, int type);
  hitbox(std::string path);

  int str_to_type(std::string) const;
  std::string type_to_str(int) const;

  void set_box_center(const vec2d &pos, float last_angle);

  bool collides(const hitbox &h) const;
  bool collides(const hitline &l) const;

  int get_type() const { return type; }

  void draw() const;

private:
  vec2d offset;
  int type;

};

std::ostream &operator<<(std::ostream &output, const hitbox &h);

#endif
