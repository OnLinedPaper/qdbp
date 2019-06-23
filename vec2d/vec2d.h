#ifndef VEC2D_H_
#define VEC2D_H_

#include <iostream>

class vec2d {
  //a 2d vector class, for position and velocity
public:
  vec2d() : x(0), y(0) { }
  vec2d(double xin, double yin) : x(xin), y(yin) { }
  vec2d(const vec2d& v) : x(v[0]), y(v[1]) { }
  vec2d& operator=(const vec2d &v) {
    if(this != &v) {
      x = v[0];
      y = v[1];
    }
    return *this;
  }


  //these operators let us return values with array notation
  double &operator [](int i);
  double operator [](int i) const;

  //these return a vector modified in a scalar way
  vec2d operator+(double scalar_val) const;
  vec2d operator-(double scalar_val) const;
  vec2d operator*(double scalar_val) const;
  vec2d operator/(double scalar_val) const;
  vec2d operator-() const; //negation

  //these return a vector modified by another vector
  vec2d operator+(const vec2d &v) const;
  vec2d operator-(const vec2d &v) const;
  vec2d operator*(const vec2d &v) const;
  //not implementing divide, no way to do that safely since
  //x or y could be 0

  //these modify the vector in question itself in a scalar way
  //TODO: +=, -=, *=? may not even need these

  double magnitude() const;
  double magnitudeSquared() const;
  vec2d normalize() const; //NOTE: will not normalize beyond .001, returns this

  double dot(const vec2d &v) const;
  //vec2d cross(const vec2d &v) const; //it's a 2d game, this isn't implemented

  vec2d cap(double scalar_val) const;
  




private:
  double x;
  double y;
};

std::ostream &operator<<(std::ostream &output, const vec2d &v);


#endif
