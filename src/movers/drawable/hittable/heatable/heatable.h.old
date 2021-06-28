#ifndef HEATABLE_H_
#define HEATABLE_H_

#include "src/movers/drawable/hittable/hittable.h"
#include "src/timeframe/timeframe.h"

class heatable : public hittable {
public:
  heatable(const std::string path);
  virtual ~heatable() = default;

  void heat_up(float h) { heat += h * t_frame::get().t_adjust(); }
  void cool_dn(float c) { 
    heat = (heat - (c * t_frame::get().t_adjust()) < 0 ? 0 : heat - (c * t_frame::get().t_adjust())); }
  float get_heat() const { return heat; }
  float get_heat_percent();
  float get_overheat_percent();
  bool is_over_max() const { return heat > max_heat; }
  bool is_overheat() const { return overheated; };

  virtual void boost(bool);
  virtual bool can_fire();
  virtual uint8_t get_weapon_id() const;
  virtual const std::string get_weapon() const { return ""; }

  virtual bool take_damage(float dmg) { return false; }

  void draw() const override;
  virtual void destroy() override { hittable::destroy(); }
  virtual void update() override;

protected:
  float heat;
  float max_heat;
  float max_overheat;
  float boost_heat_per_tick;
  float cool_per_tick;

  bool overheated;
};

#endif
