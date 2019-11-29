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

  void draw_rotate(float x, float y, float a, float frame_bump) const;
  void draw_tile(float parallax) const;
  const vec2d &get_dim() const { return dimensions; }

private:
  void fill_t_vec(const std::string &);

  image();
  image(const image&);
  image& operator=(const image&);

  vec2d dimensions;
  vec2d pivot;
  int frames;
  int frame_delay;

  std::vector<SDL_Texture *> t_vec;

};

#endif
