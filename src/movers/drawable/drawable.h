#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "src/movers/movable.h"
#include "src/image/image.h"
#include "src/image/image_handler.h"
#include "src/xml_parser/xmlparse.h"
#include <SDL2/SDL.h>

class drawable : public movable {
public:
  drawable(const std::string path) :
    drawable(path, vec2d(0,0), vec2d(0,0))
  { }
  drawable(const std::string path, const vec2d vel, const vec2d pos) :
    movable(path, vel, pos),
    image_name("/" + xmlparse::get().get_xml_string(path + "/textures/body")),
    team_name(xmlparse::get().get_xml_string(path + "/team")),
    frame_bump(rand()),
    last_angle(0)
  { 
    image_handler::get_col_from_team(team_name, team_col); 
    image_handler::jitter_col(20, team_col);
  }
  virtual ~drawable() = default;

protected:
  virtual void update() override;
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
