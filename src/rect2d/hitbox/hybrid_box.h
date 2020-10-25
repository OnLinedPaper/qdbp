#ifndef HYBRID_BOX_H_
#define HYBRID_BOX_H_

#include "src/rect2d/hitbox/hitbox.h"
#include "src/rect2d/hitbox/hitline.h"

class hy_box {
public:

  hy_box(float size, vec2d &offset, int type);
  hy_box(std::string path);

  int str_to_type(std::string) const;
  std::string type_to_str(int) const;

  void calibrate();
  void set_box_center(vec2d &pos, float last_angle);

  bool collides(const hy_box &hy) const;
  bool collides(const hitbox &hi) const;
  bool collides(const hitline &li) const;

  int get_type() const { return b.get_type(); }
  const hitbox &get_b() const { return b; }
  const hitline &get_l() const { return l; }
  float get_s() const { return size; }
  bool get_la() const { return line_active; }

  void draw() const;  


private:
  float size;
  
  hitbox b;
  hitline l;
  
  bool line_active;
};

std::ostream &operator<<(std::ostream &output, const hy_box &hy);

#endif
