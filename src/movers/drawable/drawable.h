#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "src/movers/movable.h"
#include "src/image/image.h"
#include "src/xml_parser/xmlparse.h"
#include <SDL2/SDL.h>

class drawable : public movable {
public:
  drawable(const std::string path) :
    movable(path),
    image_name("/" + xmlparse::get().get_xml_string(path + "/textures/body")),
    frame_bump(rand()),
    last_angle(0)
  { }
  virtual ~drawable() = default;

protected:
  virtual void update() override;
  std::string image_name;

  //this is so when the image is drawn, each one has a different
  //frame
  float frame_bump;

  float last_angle;


};

#endif
