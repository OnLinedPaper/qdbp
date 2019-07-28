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
  
  void draw_rotate(double x, double y, double a, double frame_bump) const;

private:
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
