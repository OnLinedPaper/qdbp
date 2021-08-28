#include "src/environment/background/background.h"
#include "src/image/image_handler.h"
#include "src/renders/render.h"
#include <string>

b_ground::b_ground(std::string name) : bg_name(name) { }
b_ground::~b_ground() { }

void b_ground::draw() const {
  if(render::get().mode() == render::R_SDL) {
    if(!bg_name.empty()) {
      image_handler::get().draw_tile(bg_name + "bg", 0.1);
      image_handler::get().draw_tile(bg_name + "mg", 0.4);
      image_handler::get().draw_tile(bg_name + "fg", 1.2);
    }
  }
  else if(render::get().mode() == render::R_NCURSES) {
    //draw tile as a blank set of spaces
    image_handler::get().draw_nc_bg();
  }
}
