#ifndef IMAGE_H_
#define IMAGE_H_

#include <SDL2/SDL.h>
#include <vector>
#include "src/vec2d/vec2d.h"

//a class designed to store a texture to be drawn
class image {
public:

  image(const std::string);
  ~image();

  void draw_r_c_o_all(
      float x,                    //x value to draw at
      float y,                    //y value to draw at
      float angle,                //angle to rotate to
      bool relative_to_screen,    //uses screen units? if false, convert x, y to viewport units - 
                                  //  if true, use x, y units as screen units
      float frame_bump,           //which frame of the animation to start with
      const SDL_Color &mod,       //color to modulate the sprite with
      float opacity               //how opaque to make the image (0 = transparent, 1 = opaque)
  ) const;
/*
  //deprecated - there were too many of these things anyway
  void draw_rotate_color(float x, float y, float a, float frame_bump, const SDL_Color &mod) const;
  void draw_rotate_color_opacity(
      float x, float y, float z, float frame_bump, const SDL_Color &mod, float opacity) const;
*/
/*
  void draw_rotate(float x, float y, float a, float frame_bump) const;
*/
  void draw_tile(float parallax, float x_offset, float y_offset) const;
  const vec2d &get_dim() const { return dimensions; }

private:
  void fill_t_vec(const std::string &);

  image();
  image(const image&);
  image& operator=(const image&);

  vec2d dimensions;
  vec2d pivot;
  SDL_BlendMode blend;
  int frames;
  int frame_delay;

  std::vector<SDL_Texture *> t_vec;

};

#endif
