#ifndef RENDER_H_
#define RENDER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <chrono>

class render {

public:
  ~render();

  static render &get() {
    static render instance;
    return instance;
  }

  SDL_Window *get_w() { return w; };
  SDL_Renderer *get_r() { return r; };

  void shade_display(float shade);

private:

  SDL_Window *w;
  SDL_Renderer *r;

  SDL_Window *init_window();
  SDL_Renderer *init_renderer();

  render();
  render(const render&) = delete;
  render &operator=(const render&) = delete;

};

#endif
