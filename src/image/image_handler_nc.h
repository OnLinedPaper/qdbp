#ifndef IMAGE_HANDLER_NC_H_
#define IMAGE_HANDLER_NC_H_

#if defined RENDER_NC

#include "src/vec2d/vec2d.h"

/*
an entirely separate image handler that runs on ncurses. different things 
being done, meant to only be called by other ncurses versions of the code.
mostly deals with drawing shapes on the screen.
*/

class image_handler {
public:
  static image_handler &get() {
    static image_handler instance;
    return instance;
  }

  ~image_handler();

  //ALL of these draw functions expect their inputs to be in WORLD COORDINATES

  //dras a single point at specified world coordinates
  void draw_point(const vec2d &, char);
 
  //draws a line from the first world coordinate to the second
  void draw_line(const vec2d &, const vec2d &, char);

  //draws a box from the first world coordiante to the second
  //can be drawn filled or empty
  void draw_box(const vec2d &, const vec2d &, bool, char);

  //draws a circle with a given radius
  //can be drawn filled or empty
  void draw_circle(const vec2d &, float, bool, char);

private:
  image_handler();
  image_handler(const image_handler&) = delete;
  image_handler &operator=(const image_handler&) = delete;
};

#endif
#endif
