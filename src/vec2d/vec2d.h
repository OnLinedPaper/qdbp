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
  vec2d normalize() const; //NOTE: will not normalize beyond .001, returns this

  cint dot(const vec2d &v) const;
  cint cross(const vec2d &v) const;

  vec2d cap(cint scalar_val) const;
  vec2d decay(cint scalar_val) const;

  cint dist(const vec2d &) const;



  float angle_rad() const;
  cint angle_deg() const;

  std::string to_string() const;

private:
  cint x;
  cint y;
  cint max = CINT_MAX;
  float PI = 3.141592653589793238462643383279502884;
};

std::ostream &operator<<(std::ostream &output, const vec2d &v);


#endif
