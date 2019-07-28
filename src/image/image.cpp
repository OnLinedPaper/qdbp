#include <SDL2/SDL.h>
#include "src/renders/render.h"
#include "src/xml_parser/xmlparse.h"
#include "image.h"
#include "src/timeframe/timeframe.h"
#include "src/viewport/viewport.h"
#include "src/utils/message.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>

using xmlp = xmlparse;

image::image (const std::string name) :
  dimensions(
    xmlp::get().get_xml_double(name + "/dimensions/width"),
    xmlp::get().get_xml_double(name + "/dimensions/height")
  ),
  pivot(
    xmlp::get().get_xml_double(name + "/dimensions/pivot_x"),
    xmlp::get().get_xml_double(name + "/dimensions/pivot_y")
  ),
  frames(xmlp::get().get_xml_double(name + "/dimensions/frames")),
  frame_delay(xmlp::get().get_xml_double(name + "/dimensions/frame_delay"))
{

  if(dimensions[0] <= 0) {
    std::string warn = name + "/dimensions/width is <= 0";
    msg::print_warn(warn);
    msg::print_alert("setting to 16");
    dimensions[0] = 16;
  }
  if(dimensions[1] <= 0) {
    std::string warn = name + "/dimensions/height is <= 0";
    msg::print_warn(warn);
    msg::print_alert("setting to 16");
    dimensions[1] = 16;
  }
  if(frames <= 0) {
    std::string warn = name + "/dimensions/frames is <= 0";
    msg::print_warn(warn);
    msg::print_alert("setting to 1 (no animation)");
    frames = 1;
  }
  if(frame_delay <= 0)
  {
    std::string warn = name + "/dimensions/frame_delay is <= 0";
    msg::print_warn(warn);
    msg::print_alert("setting to 0 (no animation)");
    frame_delay = 0;
  }

  //get some space for the frames
  t_vec.reserve(frames);

  //load an image, based on a given string - this string will be used on
  //imagedata.xml to retrieve image data, such as filename and dimensions
  SDL_Surface *full_surf = IMG_Load((xmlp::get().get_xml_string(name + "/file")).c_str());

  if(full_surf == NULL) {
    msg::print_warn("couldn't open image file \"" + xmlp::get().get_xml_string(name + "/file") + "\"!");
    msg::print_alert("(check imagedata.xml <" + name + "> - is the <file> path spelled right?)");
    msg::print_alert("(does " + xmlp::get().get_xml_string(name + "/file") + " exist?)");

    full_surf = SDL_CreateRGBSurface(0, dimensions[0], dimensions[1], 32, 0, 0, 0, 0);
    SDL_FillRect(full_surf, NULL, SDL_MapRGB(full_surf->format, 255, 0, 255));
  }

  //splice the image into pieces based on frames
  SDL_Rect clip;
  clip.x = 0;
  clip.y = 0;
  clip.w = dimensions[0];
  clip.h = dimensions[1];

  for(int i=0; i<frames; i++) {
    //make a surface to copy a sprite frame onto
    SDL_Surface *surf = SDL_CreateRGBSurface(
      0,
      dimensions[0], dimensions[1],
      full_surf->format->BitsPerPixel,
      full_surf->format->Rmask,
      full_surf->format->Gmask,
      full_surf->format->Bmask,
      full_surf->format->Amask
    );

    //offset properly
    clip.x = dimensions[0] * i;
    //copy the surface
    SDL_BlitSurface(full_surf, &clip, surf, NULL);

    SDL_Texture *t;
    if(surf == NULL) {
      fprintf(stderr, "couldn't load image! SDL_Error: %s\n", SDL_GetError());
    }
    else {
      t = SDL_CreateTextureFromSurface(render::get().get_r(), surf);
      if(t == NULL) {
        fprintf(stderr, "couldn't create texture! SDL_Error: %s\n", SDL_GetError());
      }
      //save the texture
      t_vec.push_back(t);
      //free the surface
    }
    SDL_FreeSurface(surf);
  }
  SDL_FreeSurface(full_surf);
}

image::~image() {
  for(SDL_Texture *t : t_vec) {
    SDL_DestroyTexture(t);
  }
}

void image::draw_rotate(double x_pos, double y_pos, double angle, double frame_bump) const {

  //check to see if the entity is anywhere on-screen - if it's not,
  //don't draw it, to save time
  if(
    x_pos + dimensions[0] < viewport::get().get_tlc_x() ||
    x_pos > viewport::get().get_brc_x() ||
    y_pos + dimensions[0] < viewport::get().get_tlc_y() ||
    y_pos > viewport::get().get_brc_y()
  ) { return; }

  SDL_Rect dest_r;
  dest_r.x = x_pos - (dimensions[0] / 2) - viewport::get().get_tlc_x();
  dest_r.y = y_pos - (dimensions[1] / 2) - viewport::get().get_tlc_y();
  dest_r.w = dimensions[0];
  dest_r.h = dimensions[1];

  SDL_Point *piv = NULL;
  if(pivot.magnitude() != 0) {
    piv = new SDL_Point();
    piv->x = pivot[0];
    piv->y = pivot[1];
  }

  //animate by frame
  //"frame_count" is how many frames have elapsed since the first frame
  //it's divided by frame delay to get the current frame to render= 0;
  int frame_to_render = 0;
  if(frame_delay != 0) {
    frame_to_render =
      int(t_frame::get().get_f()) //the current frame, from timeframe class
      %
      int((frames * frame_delay)) //total frames to complete one animation loop
      /
      frame_delay; //get the current frame

    //introduce a little randomness, predetermined at beginning
    frame_to_render = int(frame_to_render + frame_bump) % frames;
  }

  SDL_Texture *t = t_vec[frame_to_render];

  SDL_RenderCopyEx(render::get().get_r(), t, NULL, &dest_r, angle, piv, SDL_FLIP_NONE);

  delete piv;
  piv = NULL;
}
