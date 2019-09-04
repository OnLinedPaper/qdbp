#ifndef HITTABLE_H_
#define HITTABLE_H_

#include "src/movers/drawable/drawable.h"
#include "src/rect2d/hitbox/hitbox.h"
#include "src/xml_parser/xmlparse.h"
#include <vector>

class hittable : public drawable {
public:
  hittable(const std::string path) :
    drawable(path)
  { }

  void draw_boxes() const;

protected:
  void update() override;
  void update_boxes();

  std::vector<std::vector<hitbox>> boxes;

  std::vector<hitbox> hitboxes;
  std::vector<hitbox> hurtboxes;
  std::vector<hitbox> weakboxes;
  std::vector<hitbox> shieldboxes;

private:

};

#endif
