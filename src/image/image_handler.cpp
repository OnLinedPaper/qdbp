#include "image_handler.h"
#include <utility>
#include <algorithm>

//does this initialize images?? sure fuckin hope so
image_handler::image_handler() : images() { }

image_handler::~image_handler() {
  //remove the images
  images.clear();
}

void image_handler::draw_v2(const std::string name, float x, float y, float angle, 
    bool relative_to_screen, float frame_bump, const SDL_Color &c, float opacity) {
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_r_c_o_all(x, y, angle, relative_to_screen, frame_bump, c, opacity);
}

void image_handler::draw_tile(const std::string name, float parallax, float x_offset, float y_offset) {
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_tile(parallax, x_offset, y_offset);
}

void image_handler::DEBUG_draw_with_tile_overlay(const std::string shape_name, float x, float y, float angle, bool relative_to_screen, float frame_bump, const SDL_Color &c, float opacity, const std::string overlay_name, float parallax, float x_offset, float y_offset, int f_bump) {
  if(images.find(shape_name) == images.end()) { add_image(shape_name); }
  if(images.find(overlay_name) == images.end()) { add_image(overlay_name); }
  images.at(shape_name).DEBUG_draw_with_texture_overlay(x, y, angle, relative_to_screen, frame_bump, c, opacity, images.at(overlay_name).get_tx(f_bump), parallax, x_offset, y_offset);
}

void image_handler::add_image(std::string name) {
  //used emplace so i could avoid a default constructor call to image
  images.emplace(name, name);
}

void image_handler::get_col_from_team(const std::string team, SDL_Color &c) {
  if(strcmp(team.c_str(), "RED") == 0) {
    c.r = 160; c.g = 0; c.b = 0;
  }
  else if(strcmp(team.c_str(), "ORANGE") == 0) {
    c.r = 160; c.g = 96; c.b = 0;
  }
  else if(strcmp(team.c_str(), "YELLOW") == 0) {
    c.r = 192; c.g = 192; c.b = 0;
  }
  else if(strcmp(team.c_str(), "GREEN") == 0) {
    c.r = 0; c.g = 192; c.b = 0;
  }
  else if(strcmp(team.c_str(), "BLUE") == 0) {
    c.r = 64; c.g = 64; c.b = 192;
  }
  else if(strcmp(team.c_str(), "PURPLE") == 0) {
    c.r = 128; c.g = 0; c.b = 96;
  }
  else if(strcmp(team.c_str(), "BROWN") == 0) {
    c.r = 96; c.g = 64; c.b = 0;
  }
  else if(strcmp(team.c_str(), "WHITE") == 0) {
    c.r = 255; c.g = 255; c.b = 255;
  }
  else { c.r = 40; c.g = 40; c.b = 40; }
}

void image_handler::jitter_col(int strength, SDL_Color &c) {
  std::random_device rd;
  std::uniform_int_distribution<> distrib(-strength, strength);

  c.r += std::clamp((distrib(rd) * c.r) / 255, 0, 255);
  c.g += std::clamp((distrib(rd) * c.g) / 255, 0, 255);
  c.b += std::clamp((distrib(rd) * c.b) / 255, 0, 255);


}

//deprecated drawing functions
/*
void image_handler::draw_rotate_color(const std::string name, float x, float y,
    float frame_bump, float angle, const SDL_Color &c) 
{
  draw_rotate_color_opacity(name, x, y, frame_bump, angle, c, 1);
}

void image_handler::draw_rotate_color_opacity(const std::string name, float x, float y,
    float frame_bump, float angle, const SDL_Color &c, float opacity) 
{
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_r_c_o_all(x, y, angle, false, frame_bump, c, opacity);
}

void image_handler::draw_rotate_color_outline(const std::string name, 
    float x, float y, float frame_bump, float angle, 
    const SDL_Color &c, bool outline_color, float opacity)
{
  SDL_Color nc = {255, 255, 255};
  if(outline_color) { nc = c; }
  draw_rotate_color_opacity(name + "_outline", x, y, frame_bump, angle, nc, opacity);
  draw_rotate_color(name, x, y, frame_bump, angle, c);
}

void image_handler::draw_rotate(const std::string name, float x, float y, 
    float frame_bump, float angle) 
{
  //draw with no color mod
  static SDL_Color no_col = {255, 255, 255};
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
   }
   images.at(name).draw_r_c_o_all(x, y, angle, false, frame_bump, no_col, 1);
}

void image_handler::draw_r_c_o_relative(const std::string name, float x,
    float y, float frame_bump, float angle, const SDL_Color &c, float opacity)
  {
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_r_c_o_all(x, y, angle, true, frame_bump, c, opacity);
}
*/

