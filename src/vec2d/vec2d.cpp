#include "vec2d.h"
#include <iostream>
#include <cmath>


//these operators let us return values with array notation
double &vec2d::operator [](int i) {
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
double vec2d::operator [](int i) const {
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
vec2d vec2d::operator+(double scalar_val) const {
  return vec2d(x + scalar_val, y + scalar_val);
}

vec2d vec2d::operator-(double scalar_val) const {
  return vec2d(x - scalar_val, y - scalar_val);
}

vec2d vec2d::operator*(double scalar_val) const {
  return vec2d(x * scalar_val, y * scalar_val);
}

vec2d vec2d::operator/(double scalar_val) const {
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

//-----------------------------------------------------------------------------
//related to vector properties

double vec2d::magnitude() const {
  return sqrt((x * x) + (y * y));
}

double vec2d::magnitudeSquared() const {
  return ((x * x) + (y * y));
}

vec2d vec2d::normalize() const {
  //NOTE: will not normalize beyond .001, returns this
  double m = this->magnitude();
  if(m < .001 && m > -.001) {
    return *this;
  }
  return vec2d(x / m, y / m);
}

double vec2d::dot(const vec2d &v) const {
  return ((x * v[0]) + (y * v[1]));
}

vec2d vec2d::cap(double scalar_val) const {
  //"cap" a vector's magnitude at a certain value

  if(this->magnitude() > scalar_val) {
    //too big
    return (this->normalize() * scalar_val);
  }
  else {
    //not too big
    return *this;
  }
}

vec2d vec2d::decay(double scalar_val) const {
  //"decay" a vector by removing a fraction of its magnitude
  double m = this->magnitude();
  if(m < 0.05 && m > -0.05) {
    return(vec2d(0,0));
  }
  return((this->normalize() * (m - (m * scalar_val))));
}


//------------------------------------------------------------------------------
//compute angles from the vectors

double vec2d::angle_rad() const {
return(std::atan2(y, x));
}

double vec2d::angle_deg() const {
  //return an angle between 0 and 360
  return(int((this->angle_rad() * (180/M_PI) + 90) + 360) % 360);
}


//-----------------------------------------------------------------------------
//and then there's this one just hangin' around down here

std::ostream &operator<<(std::ostream &output, const vec2d &v) {
  output << "(" << v[0] << ", " << v[1] << ")";
  return output;
}
