#include "hitline.h"
#include <math.h>
#include <algorithm>
#include <limits.h>
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/rect2d/rect2d.h"

#if defined RENDER_NC
#include "hitline_nc.cpp"
#endif

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

bool hitline::collides(const hitline &l, float *xout, float *yout) const {
  //adapted from https://stackoverflow.com/a/565282/7431860
  //and from https://github.com/pgkelley4/line-segments-intersect

  vec2d r = end - start;
  vec2d s = l.get_end() - l.get_start();

  float u_top = (l.get_start() - start).cross(r); 
  float denominator = r.cross(s);

  if(u_top == 0 && denominator == 0) {
    //colinear - check for overlap
    //determine distance between furthest points; if this is less than the sum
    //of the distances of the lines, they overlap
    float x_points[4] = {start[0], end[0], l.get_start()[0], l.get_end()[0]};
    float y_points[4] = {start[1], end[1], l.get_start()[1], l.get_end()[1]};
    std::sort(x_points, x_points + 4);
    std::sort(y_points, y_points + 4);

    hitline l_greatest({x_points[0], y_points[0]}, {x_points[3], y_points[3]});
    float greatest_len = l_greatest.get_len();
    float total_len = this->get_len() + l.get_len();
    if(greatest_len > total_len) {
      //disjoint
      return false;
    }
    else if(greatest_len < total_len) {
      //overlapping
      //cannot determine single point of intersection
      *xout = FLT_MAX; *yout = FLT_MAX;
      return true;
    }
    else {
      //toucking at the very ends 
      //the two middle points are identical and either can be used
      *xout = x_points[1];
      *yout = y_points[1];
      return true;
    }
  }
  else if(denominator == 0) {
    //parallel
    return false;
  }
  
  //TODO: minmax this to remove division
  float u = u_top / denominator;
  float t = (l.get_start() - start).cross(s) / denominator;

  if(!(t >= 0 && t <= 1 && u >= 0 && u <= 1)) { return false; }


  //collides from this point on and is not parallel or colinear

  //adapted from here https://stackoverflow.com/a/1968345/7431860
  //and here https://stackoverflow.com/a/565282

  //interception is at p + tr = q + us, where
  //p is this->start, r is this->end - this->start,
  //q is l->start, s is l->end - l->start, 
  //t and u are scalar values that are used to move along the line towards
  //r and s, respectively

  *xout = start[0] + t * (end[0] - start[0]);
  *yout = start[1] + t * (end[1] - start[1]);

  return true;
}

float hitline::get_len() const {
  return std::sqrt(std::pow((end[0] - start[0]), 2) + std::pow((end[1]-start[1]), 2));
}

#if defined RENDER_SDL
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
#endif

