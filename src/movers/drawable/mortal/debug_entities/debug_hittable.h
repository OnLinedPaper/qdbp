#ifndef MORTAL_DEBUG_HITTABLE_H_
#define MORTAL_DEBUG_HITTABLE_H_

#include "src/movers/drawable/mortal/mortal.h"

class d_hittable : public mortal {
public:
  d_hittable(const std::string);

  void update() override;
  void draw() const override;

};

#endif
