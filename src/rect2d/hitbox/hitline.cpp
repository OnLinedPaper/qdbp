#include "hitline.h"
#include <math.h>
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/rect2d/rect2d.h"

hitline::hitline(const vec2d &s, const vec2d &e) :
  start(s),
  end(e)
{ }

hitline::hitline(const vec2d &s, float length, float in_angle) :
  start(s),
  end(s)
{
  //use angle and length to determine end of vector

  double PI = 3.141592653589793238462643383279502884;
  double angle = ((in_angle+180) * PI) / 180;

  //assume a line straight up, and then offset it by rotating
  end[0] = start[0] - (sin(angle) * length);
  end[1] = start[1] + (cos(angle) * length);
}

bool hitline::collides(const hitline &l) const {
  //adapted from https://stackoverflow.com/a/565282/7431860
  //and from https://github.com/pgkelley4/line-segments-intersect

  vec2d r = end - start;
  vec2d s = l.get_end() - l.get_start();

  float numerator = (l.get_start() - start).cross(r); 
  float denominator = r.cross(s);

  if(numerator == 0 && denominator == 0) {
    //colinear - cnvert to box and check for collision
    rect2d r1(start[0], start[1], end[0] - start[0], end[1] - start[1]); 
    return(r1.overlap(l));
  }
  else if(denominator == 0) {
    return false;
  }
  
  float u = numerator / denominator;
  float t = (l.get_start() - start).cross(s) / denominator;

  return (t >= 0 && t <= 1 && u >= 0 && u <= 1); 
}

void hitline::draw() const {

    //save color
    SDL_Color c;
    SDL_GetRenderDrawColor(render::get().get_r(), &(c.r), &(c.g), &(c.b), &(c.a));

    //set to red
    SDL_SetRenderDrawColor(render::get().get_r(), 255, 0, 0, 128);
    SDL_RenderDrawLine(
      render::get().get_r(),
      start[0] - viewport::get().get_tlc_x(),
      start[1] - viewport::get().get_tlc_y(),
      end[0] - viewport::get().get_tlc_x(),
      end[1] - viewport::get().get_tlc_y()
    );

    //restore color
    SDL_SetRenderDrawColor(render::get().get_r(), c.r, c.g, c.b, c.a);
}
