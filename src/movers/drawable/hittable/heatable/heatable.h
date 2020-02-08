#ifndef HEATABLE_H_
#define HEATABLE_H_

#include "src/movers/drawable/hittable/hittable.h"

class heatable : public hittable {
public:
  heatable(const std::string path);
  virtual ~heatable() = default;

  void heat_up(float h) { heat += h; }
  void cool_dn(float c) { heat = (heat - c < 0 ? 0 : heat - c); }
  float get_heat() const { return heat; }
  float get_heat_percent();
  float get_overheat_percent();
  bool is_over_max() const { return heat > max_heat; }
  bool is_overheat() const { return overheated; };

  virtual void boost(bool);

  virtual void update() override;

private:
  float heat;
  float max_heat;
  float max_overheat;
  float boost_heat_per_tick;
  float cool_per_tick;

  bool overheated;
};

#endif
