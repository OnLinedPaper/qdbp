#ifndef HITTABLE_H_
#define HITTABLE_H_

#include "src/movers/drawable/drawable.h"
#include "src/rect2d/hitbox/hitbox.h"
#include "src/rect2d/hitbox/hybrid_box.h"
#include "src/xml_parser/xmlparse.h"
#include <vector>

class hittable : public drawable {
public:
  hittable(const std::string path);
  hittable(const std::string path, const vec2d pos, const vec2d vel);
  virtual ~hittable() = default;

  void draw() const override;
  void draw_boxes() const;

  virtual bool collides(const hitbox &h, int type) const;
  virtual bool collides(const hitline &l, int type) const;
  virtual bool collides(const hy_box &hy, int type) const;
  virtual bool collides(const hittable *other, int other_type, int this_type) const;

  void set_pos(const vec2d p) { this->set_pos(p[0], p[1]); }
  void set_pos(float x, float y);

  void calibrate_boxes();

  virtual bool take_damage(float damage) { return false; }

  virtual void destroy() override { drawable::destroy(); }
  virtual void update() override;

protected:
  std::vector<hy_box> const &get_box_ref(int type) const;
 
  void update_boxes();

  std::vector<std::vector<hy_box> *> boxes;

  std::vector<hy_box> hitboxes;
  std::vector<hy_box> hurtboxes;
  std::vector<hy_box> weakboxes;
  std::vector<hy_box> armorboxes;
  std::vector<hy_box> shieldboxes;
  std::vector<hy_box> pickupboxes;
  std::vector<hy_box> vacuumboxes;

private:

  std::string team;
  SDL_Color specific_col;
};

#endif
