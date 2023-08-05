#ifndef HITLINE_H_
#define HITLINE_H_

#include "src/vec2d/vec2d.h"
#include "types.h"

//hitlines are single-line entities meant to collide with hitboxes,
//and for the foreseeable future will be TYPE_HURTBOX

class hitline {
public:

  //constructor with 2 points
  hitline(const vec2d &s, const vec2d &e);
  //constructor with start point, angle, and length
  hitline(const vec2d &s, cint length, float in_angle);

  //determines whether two lines collide
  bool collides(const hitline &, cint * = NULL, cint * = NULL) const;

  const vec2d get_start() const { return start; }
  void set_start(const vec2d &v) { start = v; }
  const vec2d get_end() const { return end; }
  void set_end(const vec2d &v) { end = v; }

  cint get_len() const { return length; };

  void draw() const;


private:
#ifdef RENDER_NC
  char angle_to_char() const;
#endif
  cint calc_len() const;

  vec2d start;
  vec2d end;

  cint length;
};


#endif
