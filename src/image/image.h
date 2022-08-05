#ifndef IMAGE_H_
#define IMAGE_H_

#include <SDL2/SDL.h>
#include <vector>
#include "src/vec2d/vec2d.h"

//a class to be passed into draw_r_c_o_all
class rcoa_struct {
public: 
  rcoa_struct(SDL_Texture *, float, float, float);
  ~rcoa_struct();
  //an overlay texture, selected by image_handler
  SDL_Texture *ot;
  float parallax, x_offset, y_offset;
private:
  rcoa_struct();
  rcoa_struct(const rcoa_struct&);
  rcoa_struct& operator=(const rcoa_struct&);
};

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
      float opacity,              //how opaque to make the image (0 = transparent, 1 = opaque)
      rcoa_struct *rcoa_s = NULL  //data for overlay tiling
  ) const;

  void draw_tile(float parallax, float x_offset, float y_offset) const;
/*
  //deprecated - there were too many of these things anyway
  void draw_rotate_color(float x, float y, float a, float frame_bump, const SDL_Color &mod) const;
  void draw_rotate_color_opacity(
      float x, float y, float z, float frame_bump, const SDL_Color &mod, float opacity) const;
  void draw_rotate(float x, float y, float a, float frame_bump) const;
*/
//  void DEBUG_draw_with_texture_overlay(float x, float y, float angle, bool relative_to_screen, float frame_bump, const SDL_Color &c, float opacity, rcoa_struct *rcoa_s);

  void init_shader();

  const vec2d &get_dim() const { return dimensions; }
  SDL_Texture *get_tx(int f) const { return t_vec.at(f % frames); }

private:
  void fill_t_vec(const std::string &);

  //takes an existing dest_r and redefines it in the context of a new dest_r
  //more specifically: when an image needs an overlay, a new texture is created at its render location
  //and used as a renderer for it and the overlay. multiple rects are needed as a result
  void redefine_dest_r(SDL_Rect &new_context, SDL_Rect &dest_r);

  image();
  image(const image&);
  image& operator=(const image&);

  vec2d dimensions;
  vec2d pivot;
  SDL_BlendMode blend;
  int frames;
  int frame_delay;

  //vector of textures to hold each animation frame
  std::vector<SDL_Texture *> t_vec;
  //shader-like texture for applying a pattern on top of this iamge's shape. lazy-loaded
  SDL_Texture *shader_t;
};

#endif
