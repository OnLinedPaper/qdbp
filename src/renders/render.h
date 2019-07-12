#ifndef RENDER_H_
#define RENDER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

class render {

public:
  ~render();

  static render &get() {
    static render instance;
    return instance;
  }

  double get_f() { return frame; }
  void incr_f() { frame ++; }

  SDL_Window *get_w() { return w; };
  SDL_Renderer *get_r() { return r; };

private:

  SDL_Window *w;
  SDL_Renderer *r;

  SDL_Window *init_window();
  SDL_Renderer *init_renderer();

  double frame;

  render();
  render(const render&) = delete;
  render &operator=(const render&) = delete;

};

#endif
