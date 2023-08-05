#ifndef VEC2D_H_
#define VEC2D_H_

#include <iostream>
#include <float.h>
#include <string>
#include "types.h"

class vec2d {
  //a 2d vector class, for position and velocity
public:
  vec2d() : x(0), y(0) { }
  //vec2d(float xin, float yin) : x((cint)xin), y((cint)yin) { }
  vec2d(cint xin, cint yin) : x(xin), y(yin) { }
  vec2d(const vec2d& v) : x(v[0]), y(v[1]) { }
  vec2d& operator=(const vec2d &v) {
    if(this != &v) {
      x = v[0];
      y = v[1];
    }
    return *this;
  }


  //these operators let us return values with array notation
  cint &operator [](cint i);
  cint operator [](cint i) const;

  //these return a vector modified in a scalar way
  vec2d operator+(cint scalar_val) const;
  vec2d operator-(cint scalar_val) const;
  vec2d operator*(cint scalar_val) const;
  vec2d operator/(cint scalar_val) const;
  vec2d operator-() const; //negation

  //these return a vector modified by another vector
  vec2d operator+(const vec2d &v) const;
  vec2d operator-(const vec2d &v) const;
  vec2d operator*(const vec2d &v) const;
  void operator+=(const vec2d &v);
  void operator-=(const vec2d &v);
  //not implementing divide, no way to do that safely since
  //x or y could be 0

  bool operator==(const vec2d &v) const;

  //these modify the vector in question itself in a scalar way

  cint magnitude() const;
  cint magnitudeSquared() const;

  //normalize is a 2 part function to avoid rounding errors with ints.
  //the first one is called on a vector to scale it up by NORMALZIE_FACTOR
  //and then normalize it, and the second scales it back down.
  //THESE MUST BE CALLED BEFORE ANY NON MULTIPLICATION OPERATIONS!
  //they can only be deferred until the end of a multiplication chain.
  vec2d normalizeStart() const; //NOTE: will not normalize beyond 1, returns this
  vec2d normalizeFinish() const;

  cint dot(const vec2d &v) const;
  cint cross(const vec2d &v) const;

  vec2d cap(cint scalar_val) const;
  vec2d decay(cint scalar_val) const;

  cint dist(const vec2d &) const;



  float angle_rad() const;
  cint angle_deg() const;

  std::string to_string() const;

  static const cint NORMALIZE_FACTOR;

private:
  cint x;
  cint y;
  cint max = CINT_MAX;
  float PI = 3.141592653589793238462643383279502884;
};

std::ostream &operator<<(std::ostream &output, const vec2d &v);


#endif
