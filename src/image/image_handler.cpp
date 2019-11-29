#include "image_handler.h"
#include <utility>

//does this initialize images?? sure fuckin hope so
image_handler::image_handler() : images() { }

image_handler::~image_handler() {
  //remove the images
  images.clear();
}

void image_handler::draw_rotate(const std::string name, float x, float y, float frame_bump, float angle) {
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_rotate(x, y, angle, frame_bump);
}

void image_handler::draw_tile(const std::string name, float parallax) {
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_tile(parallax);
}

void image_handler::add_image(std::string name) {
  //used emplace so i could avoid a default constructor call to image
  images.emplace(name, name);
}
