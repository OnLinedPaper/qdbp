#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "src/movers/movable.h"
#include <SDL2/SDL.h>

class drawable : public movable {
public:
  void set_texture(SDL_Texture *tex) { t = tex; }
  SDL_Texture *get_texture() const { return t; }

protected:
  void update() { movable::update(); }

  virtual void load_texture() = 0;
  SDL_Texture* t;


};

#endif
