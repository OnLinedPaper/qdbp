#ifndef RENDER_NC_H_
#define RENDER_NC_H_

#if defined RENDER_NC

/*
an entirely separate renderer that runs on ncurses. all the same stuff, but
implemented in different ways.
*/

#include <ncurses.h>
#include "src/vec2d/vec2d.h"

class render {

public:
  ~render();
  
  static render &get() {
    static render instance;
    return instance;
  }

  char * const get_r() { return r; } 

  void nc_render();

private:

  WINDOW *w;  //ncurses window
  char *r; //array of characters that holds the next thing to be drawn on
           //screen - all "drawing" is done by changing chars in this array
  bool nc_render_this_frame; //used as a saafety check to block rendering after
                             //a window resize
  void nc_check_stale_win_size(); //check window size to prevent segfaults
  void draw_blinky(); //qdbp


  render();
  render(const render&) = delete;
  render &operator=(const render&) = delete;
};

#endif
#endif
