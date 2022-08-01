#ifndef RENDER_H_
#define RENDER_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <ncurses.h>
#include "src/vec2d/vec2d.h"

class render {

public:
  static const uint8_t R_SDL;
  static const uint8_t R_NCURSES;

  ~render();

  static render &get() {
    static render instance;
    return instance;
  }

  static void signal_handler_SIGWINCH(int signum);

  //returns the graphics mode; defaults to 0, SDL, but can be set to 1, ncurses
  uint8_t mode() { return graphics_mode; }

  SDL_Window *get_w() { return w; };
  SDL_Renderer *get_r() { return r; };

  void shade_display(float shade);

  void nc_render();
  int nc_get_l() { return LINES; }
  int nc_get_c() { return COLS; }

  char * const nc_get_dv() { return draw_vals; }

  //draw a box on the screen at the specified location, allowing the renderer to handle the rest
  void nc_draw_box(const vec2d &tlc, const vec2d &brc, char c);

private:

  uint8_t graphics_mode;

  SDL_Window *w;
  SDL_Renderer *r;
  WINDOW *w_nc;
  char *draw_vals;

  SDL_Window *init_SDL_window();
  SDL_Renderer *init_SDL_renderer();

  void init_ncurses_window();

  void draw_blinky() const;
  void nc_check_for_stale_win_size();


  bool nc_render_this_frame;
  render();
  render(const render&) = delete;
  render &operator=(const render&) = delete;

};

#endif
