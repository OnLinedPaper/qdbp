#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "src/movers/movable.h"
#include "src/image/image.h"
#include "src/image/image_handler.h"
#include "src/xml_parser/xmlparse.h"
#include <SDL2/SDL.h>

class drawable : public movable {
public:
  drawable(const std::string path);
  drawable(const std::string path, const vec2d vel, const vec2d pos);
  virtual ~drawable() = default;

  const SDL_Color &get_col() { return team_col; }
  void set_col(const SDL_Color &c) {
    team_col.r = c.r;
    team_col.g = c.g;
    team_col.b = c.b;
    team_col.a = c.a;
  }

protected:
  virtual void update() override;
  virtual void destroy() override { movable::destroy(); };
  void draw() const override;
  std::string image_name;

  std::string team_name;
  SDL_Color team_col;

  //this is so when the image is drawn, each one has a different
  //frame
  float frame_bump;

  float last_angle;


};

#endif
