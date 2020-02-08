#ifndef DEBUG_HEATABLE_H_
#define DEBUG_HEATABLE_H_

#include "heatable.h"

class d_heatable : public heatable {
public:
  d_heatable(const std::string);

  void move_up() override;
  void move_dn() override;
  void move_lf() override;
  void move_rt() override;

  virtual void boost(bool);

  void update() override;

  void draw() const override;


};

#endif
