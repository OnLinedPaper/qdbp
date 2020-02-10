#ifndef HITTABLE_H_
#define HITTABLE_H_

#include "src/movers/drawable/drawable.h"
#include "src/rect2d/hitbox/hitbox.h"
#include "src/xml_parser/xmlparse.h"
#include <vector>

class hittable : public drawable {
public:
  hittable(const std::string path);
  virtual ~hittable() = default;

  void draw() const override;
  void draw_boxes() const;

  bool collides(const hitbox &h, int type) const;
  bool collides(const hitline &l, int type) const;

  virtual void update() override;

protected:
  void update_boxes();

  std::vector<std::vector<hitbox> *> boxes;

  std::vector<hitbox> hitboxes;
  std::vector<hitbox> hurtboxes;
  std::vector<hitbox> weakboxes;
  std::vector<hitbox> armorboxes;
  std::vector<hitbox> shieldboxes;
  std::vector<hitbox> pickupboxes;
  std::vector<hitbox> vacuumboxes;

private:

};

#endif
