#include "src/movers/drawable/hittable/heatable/killable/killable.h"

class d_killable : public killable {
public:
  d_killable(const std::string);

  void move_up() override;
  void move_dn() override;
  void move_lf() override;
  void move_rt() override;

  virtual void boost(bool);
  virtual uint8_t get_weapon_id() const override; 
  virtual bool can_fire() override; 
  const std::string get_weapon() const override { return weapon; }

  void update() override;

  void draw() const override;

private:
  std::string weapon;

};
