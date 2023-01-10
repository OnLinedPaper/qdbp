#ifndef RENDER_NC_H_
#define RENDER_NC_H_

#if defined RENDER_NC

/*
an entirely separate renderer that runs on ncurses. all the same stuff, but
implemented in different ways.
*/

#include <ncurses.h>

class render {

public:
  ~render();
  
  static render &get() {
    static render instance;
    return instance;
  }

  char * const get_r() { return r; } 

private:

  char *r; //array of characters that holds the next thing to be drawn on
           //screen - all "drawing" is done by changing chars in this array


  render();
  render(const render&) = delete;
  render &operator=(const render&) = delete;
};

#endif
#endif
