#ifndef IMAGE_H_
#define IMAGE_H_

#include <SDL2/SDL.h>

//a class designed to store a texture to be drawn
class image {
public:

  image(const std::string);
  ~image();

  void draw(double, double) const;

private:
  image();
  image(const image&);
  image& operator=(const image&);

  SDL_Texture t;

};

#endif
