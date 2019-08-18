#include "src/environment/background/background.h"
#include "src/image/image_handler.h"
#include <string>

b_ground::b_ground(std::string name) : bg_name(name) { }
b_ground::~b_ground() { }

void b_ground::draw() const {
  image_handler::get().draw_tile(bg_name + "bg", 1);
}
