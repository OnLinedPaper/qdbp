#include "vec2d.h"
#include <iostream>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

//these operators let us return values with array notation
float &vec2d::operator [](int i) {
  //return a reference
  switch(i) {
    case 0:
      return x;
      break;
    case 1:
      return y;
      break;
    default:
      return max;
  }
}
float vec2d::operator [](int i) const {
  //return a const val
  switch(i) {
    case 0:
      return x;
      break;
    case 1:
      return y;
      break;
    default:
      return max;
  }
}

//-----------------------------------------------------------------------------
//scalar const operations
vec2d vec2d::operator+(float scalar_val) const {
  return vec2d(x + scalar_val, y + scalar_val);
}

vec2d vec2d::operator-(float scalar_val) const {
  return vec2d(x - scalar_val, y - scalar_val);
}

vec2d vec2d::operator*(float scalar_val) const {
  return vec2d(x * scalar_val, y * scalar_val);
}

vec2d vec2d::operator/(float scalar_val) const {
  if(scalar_val == 0) { throw "can't divide vector by 0!\n"; }
  return vec2d(x / scalar_val, y / scalar_val);
}

vec2d vec2d::operator-() const {
  return vec2d(x * -1, y * -1);
}


//-----------------------------------------------------------------------------
//vector const operations

vec2d vec2d::operator+(const vec2d &v) const {
  return vec2d(x + v[0], y + v[1]);
}

vec2d vec2d::operator-(const vec2d &v) const {
  return vec2d(x - v[0], y - v[1]);
}

void vec2d::operator+=(const vec2d &v) {
  x += v[0]; y += v[1];
}

void vec2d::operator-=(const vec2d &v) {
  x -= v[0]; y -= v[1];
}

//-----------------------------------------------------------------------------
//related to vector properties

float vec2d::magnitude() const {
  return sqrt((x * x) + (y * y));
}

float vec2d::magnitudeSquared() const {
  return ((x * x) + (y * y));
}

vec2d vec2d::normalize() const {
  //NOTE: will not normalize beyond .001, returns this
  float m = this->magnitude();
  if(m < .001 && m > -.001) {
    return *this;
  }
  return vec2d(x / m, y / m);
}

float vec2d::dot(const vec2d &v) const {
  return ((x * v[0]) + (y * v[1]));
}

vec2d vec2d::cap(float scalar_val) const {
  //"cap" a vector's magnitude at a certain value

  //don't "cap" if value is negative
  if(scalar_val < 0) {
    return *this;
  }

  if(this->magnitude() > scalar_val) {
    //too big
    return (this->normalize() * scalar_val);
  }
  else {
    //not too big
    return *this;
  }
}

vec2d vec2d::decay(float scalar_val) const {
  //"decay" a vector by removing a fraction of its magnitude
  float m = this->magnitude();
  if(m < 0.05 && m > -0.05) {
    return(vec2d(0,0));
  }
  return((this->normalize() * (m - (m * scalar_val))));
}

float vec2d::dist(const vec2d &v) const {
  //get the distance between 2 vec2d
  return sqrt(
    (v[0] - x) * (v[0] - x) +
    (v[1] - y) * (v[1] - y)
  );
}


//------------------------------------------------------------------------------
//compute angles from the vectors

float vec2d::angle_rad() const {
return(std::atan2(y, x));
}

float vec2d::angle_deg() const {
  //return an angle between 0 and 360
  return(int((this->angle_rad() * (180/PI) + 90) + 360) % 360);
}


//-----------------------------------------------------------------------------
//and then there's this one just hangin' around down here

std::ostream &operator<<(std::ostream &output, const vec2d &v) {
  output << "(" << v[0] << ", " << v[1] << ")";
  return output;
}
