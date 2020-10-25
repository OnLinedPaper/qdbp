#ifndef VEC2D_H_
#define VEC2D_H_

#include <iostream>
#include <float.h>

class vec2d {
  //a 2d vector class, for position and velocity
public:
  vec2d() : x(0), y(0) { }
  vec2d(float xin, float yin) : x(xin), y(yin) { }
  vec2d(const vec2d& v) : x(v[0]), y(v[1]) { }
  vec2d& operator=(const vec2d &v) {
    if(this != &v) {
      x = v[0];
      y = v[1];
    }
    return *this;
  }


  //these operators let us return values with array notation
  float &operator [](int i);
  float operator [](int i) const;

  //these return a vector modified in a scalar way
  vec2d operator+(float scalar_val) const;
  vec2d operator-(float scalar_val) const;
  vec2d operator*(float scalar_val) const;
  vec2d operator/(float scalar_val) const;
  vec2d operator-() const; //negation

  //these return a vector modified by another vector
  vec2d operator+(const vec2d &v) const;
  vec2d operator-(const vec2d &v) const;
  vec2d operator*(const vec2d &v) const;
  void operator+=(const vec2d &v);
  void operator-=(const vec2d &v);
  //not implementing divide, no way to do that safely since
  //x or y could be 0

  //these modify the vector in question itself in a scalar way
  //TODO: +=, -=, *=? may not even need these

  float magnitude() const;
  float magnitudeSquared() const;
  vec2d normalize() const; //NOTE: will not normalize beyond .001, returns this

  float dot(const vec2d &v) const;
  //vec2d cross(const vec2d &v) const; //it's a 2d game, this isn't implemented

  vec2d cap(float scalar_val) const;
  vec2d decay(float scalar_val) const;

  float dist(const vec2d &) const;



  float angle_rad() const;
  float angle_deg() const;


private:
  float x;
  float y;
  float max = FLT_MAX;
  float PI = 3.141592653589793238462643383279502884;
};

std::ostream &operator<<(std::ostream &output, const vec2d &v);


#endif
