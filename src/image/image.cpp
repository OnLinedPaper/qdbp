#include <SDL2/SDL.h>
#include "src/renders/render.h"
#include "src/xml_parser/xmlparse.h"
#include "image.h"
#include "src/timeframe/timeframe.h"
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
  frame_delay(xmlp::get().get_xml_double(name + "/dimensions/frame_delay")),
  frame_bump(rand() % frames)
{
  //get some space for the frames
  t_vec.reserve(frames);

  //load an image, based on a given string - this string will be used on
  //imagedata.xml to retrieve image data, such as filename and dimensions
  SDL_Surface *full_surf = IMG_Load((xmlp::get().get_xml_string(name + "/file")).c_str());

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

void image::draw_rotate(double x_pos, double y_pos, double angle) const {
  SDL_Point *piv = NULL;
  if(pivot.magnitude() != 0) {
    piv = new SDL_Point();
    piv->x = pivot[0];
    piv->y = pivot[1];
  }

  SDL_Rect dest_r;
  dest_r.x = x_pos - (dimensions[0] / 2);
  dest_r.y = y_pos - (dimensions[1] / 2);
  dest_r.w = dimensions[0];
  dest_r.h = dimensions[1];

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
    frame_to_render = (frame_to_render + frame_bump) % frames;
  }

  SDL_Texture *t = t_vec[frame_to_render];

  SDL_RenderCopyEx(render::get().get_r(), t, NULL, &dest_r, angle, piv, SDL_FLIP_NONE);

  delete piv;
  piv = NULL;

}

void image::draw_rotate(double x_pos, double y_pos, const vec2d v) const {
  draw_rotate(x_pos, y_pos, v.angle_deg());

}

void image::draw(double x_pos, double y_pos) const {
  draw_rotate(x_pos, y_pos, 0);
}
