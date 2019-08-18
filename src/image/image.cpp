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

image::image (const std::string name)
/*try*/ :
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

  fill_t_vec(name);
}
/*catch (std::string &s) {
  //something went wrong with image initialization
  msg::print_warn("couldn't load " + name);
  msg::print_alert("(loading a default texture...)");

  fill_t_vec("");

  msg::print_alert("(success)");
}*/

image::~image() {
  for(SDL_Texture *t : t_vec) {
    SDL_DestroyTexture(t);
  }
}

void image::fill_t_vec(const std::string &name) {

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

void image::draw_tile(double parallax) const {
  //draw the image in a tile-like format across the screen, shifting it
  //with player's movement according to parallax. parallax 1 means it's
  //"fixed" like an unmoving background.

  SDL_Rect dest_r;
  dest_r.x = (0 - viewport::get().get_tlc_x());
  dest_r.y = (0 - viewport::get().get_tlc_y());
  dest_r.w = dimensions[0];
  dest_r.h = dimensions[1];


  //check if viewport is too far to the left
  if(viewport::get().get_tlc_x() < 0) {
    //calculate HOW far to the left it needs to be shifted
    dest_r.x -= dimensions[0] * ((int)(0 - viewport::get().get_tlc_x()) / (int)(dimensions[0]) + 1);
  }

  //check if the viewport is too far to the right
  if(viewport::get().get_tlc_x() > dimensions[0]) {
    //calculate how far to the right it needs to be shifted
    //get the top left corner, then divide it by the tile width -
    //this allows us to tell how many "tiles" too far we are, and
    //adjust accordingly
    dest_r.x += dimensions[0] * ((int)(viewport::get().get_tlc_x()) / (int)(dimensions[0]));
  }

  //check if viewport is too far up
  if(viewport::get().get_tlc_y() < 0) {
    dest_r.y -= dimensions[1] * ((int)(0 - viewport::get().get_tlc_y()) / (int)(dimensions[1]) + 1);
  }

  //check if viewport is too far down
  if(viewport::get().get_tlc_y() > dimensions[1]) {
    dest_r.y += dimensions[1] * ((int)(viewport::get().get_tlc_y()) / (int)(dimensions[1]));
  }

  SDL_Rect draw_me = dest_r;


  //draw the tiles with nested for - they draw in stripes
  for(int i = dest_r.x; i < viewport::get().get_w(); i += dimensions[0]) {
    draw_me.x = i;
    for(int j = dest_r.y; j < viewport::get().get_h(); j += dimensions[0]) {
    draw_me.y = j;
      SDL_RenderCopyEx(render::get().get_r(), t_vec[0], NULL, &draw_me, 0, 0, SDL_FLIP_NONE);
    }
  }


}
