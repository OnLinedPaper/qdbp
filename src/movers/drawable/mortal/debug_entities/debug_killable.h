#ifndef MORTAL_DEBUG_KILLABLE_H_
#define MORTAL_DEBUG_KILLABLE_H_

#include "src/movers/drawable/mortal/mortal.h"

class d_killable : public mortal {
public:
  d_killable(const std::string&);

  void update() override;
  void draw() const override;
};

#endif
