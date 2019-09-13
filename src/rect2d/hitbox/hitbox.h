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

  hitbox(double size, vec2d &offset, int type);
  hitbox(std::string path);

  int str_to_type(std::string) const;
  std::string type_to_str(int) const;

  void set_box_center(vec2d &pos, double last_angle);

  bool collides(const hitbox &h) const;

  int get_type() const { return type; }

  void draw() const;

private:
  vec2d offset;
  int type;

};

std::ostream &operator<<(std::ostream &output, const hitbox &h);

#endif