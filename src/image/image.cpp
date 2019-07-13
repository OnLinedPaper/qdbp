#include <SDL2/SDL.h>
#include "src/renders/render.h"
#include "src/xml_parser/xmlparse.h"
#include "image.h"
#include <iostream>

using xmlp = xmlparse;

image::image (const std::string name) :
  dimensions(
    xmlp::get().get_xml_double(name + "/dimensions/width"),
    xmlp::get().get_xml_double(name + "/dimensions/height")
  ),
  frames(xmlp::get().get_xml_double(name + "/dimensions/frames")),
  curr_frame(0),
  frame_delay(xmlp::get().get_xml_double(name + "/dimensions/frame_delay"))
  {
  //load an image, based on a given string - this string will be used on
  //imagedata.xml to retrieve image data, such as filename and dimensions

  SDL_Surface *surf = IMG_Load((xmlp::get().get_xml_string(name + "/file")).c_str());
  SDL_Texture *t;
  if(surf == NULL) {
    fprintf(stderr, "couldn't load image! SDL_Error: %s\n", SDL_GetError());
  }
  else {
    t = SDL_CreateTextureFromSurface(render::get().get_r(), surf);
    if(t == NULL) {
      fprintf(stderr, "couldn't create texture! SDL_Error: %s\n", SDL_GetError());
    }
    t_vec.push_back(t);
    SDL_FreeSurface(surf);
  }
}

image::~image() {
  for(SDL_Texture *t : t_vec) {
    SDL_DestroyTexture(t);
  }
}

void image::draw(double x_pos, double y_pos) const {
  SDL_Rect dest_r;
  dest_r.x = x_pos;
  dest_r.y = y_pos;
  dest_r.w = dimensions[0];
  dest_r.h = dimensions[1];
  SDL_Texture *t = t_vec[0];

  SDL_RenderCopy(render::get().get_r(), t, NULL, &dest_r);
}
