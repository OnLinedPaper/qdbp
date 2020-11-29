#ifndef DEBUG_HEATABLE_H_
#define DEBUG_HEATABLE_H_

#include "heatable.h"
#include "src/movers/drawable/hittable/weapon.h"

class d_heatable : public heatable {
public:
  d_heatable(const std::string);

  void move_up() override;
  void move_dn() override;
  void move_lf() override;
  void move_rt() override;

  virtual void boost(bool);
  virtual void shoot();
  virtual bool can_fire();
  virtual uint8_t get_weapon_id() const override;
  const std::string get_weapon() const override { return weapon; }

  void update() override;

  void draw() const override;

private:
  std::string weapon;

};

#endif
