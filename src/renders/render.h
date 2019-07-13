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

  double get_f() const { return frame; }
  void incr_f() { frame ++; }

  double get_ms() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count();
  }

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
