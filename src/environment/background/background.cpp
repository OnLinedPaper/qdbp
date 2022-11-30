#include "src/environment/background/background.h"
#include "src/image/image_handler.h"
#include <string>

b_ground::b_ground(std::string name) : bg_name(name), x_offset(0), y_offset(0) { }
b_ground::~b_ground() { }

void b_ground::draw() const {
  if(!bg_name.empty()) {
    image_handler::get().draw_tile(bg_name + "bg", 0.1);//, x_offset, y_offset);
    image_handler::get().draw_tile(bg_name + "mg", 0.4);//, x_offset, y_offset);
    image_handler::get().draw_tile(bg_name + "fg", 1.2);//, x_offset, y_offset);
  }
}
