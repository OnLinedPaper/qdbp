#ifndef DEBUG_HITTABLE_H
#define DEBUG_HITTABLE_H

#include "hittable.h"

class d_hittable : public hittable {
public:
  d_hittable(const std::string);

  void move_up() override;
  void move_dn() override;
  void move_lf() override;
  void move_rt() override;

  void update() override;

  void draw() const override;

};

#endif
