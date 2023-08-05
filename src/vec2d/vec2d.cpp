#include "vec2d.h"
#include "src/utils/message.h"
#include <iostream>
#include <cmath>

#define _USE_MATH_DEFINES
#include <math.h>

const cint vec2d::NORMALIZE_FACTOR = 16;

//"vec2d" represents a vector in 2d space whose start point is 0,0 and whose
//end point is x,y. 

//these operators let us return values with array notation
cint &vec2d::operator [](cint i) {
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
cint vec2d::operator [](cint i) const {
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
vec2d vec2d::operator+(cint scalar_val) const {
  return vec2d(x + scalar_val, y + scalar_val);
}

vec2d vec2d::operator-(cint scalar_val) const {
  return vec2d(x - scalar_val, y - scalar_val);
}

vec2d vec2d::operator*(cint scalar_val) const {
  return vec2d(x * scalar_val, y * scalar_val);
}

vec2d vec2d::operator/(cint scalar_val) const {
  if(scalar_val == 0) { 
    msg::print_error("vec2d::operator/ attempted to divide by zero!");
    throw "can't divide vector by 0!\n"; 
  }
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


bool vec2d::operator==(const vec2d &v) const {
  return (x == v[0] && y == v[1]);
}

//-----------------------------------------------------------------------------
//related to vector properties

cint vec2d::magnitude() const {
  return (cint)sqrt((x * x) + (y * y));
}

cint vec2d::magnitudeSquared() const {
  return ((x * x) + (y * y));
}

//normalizes, BUT uses NORMALIZE_FACTOR as the zero point instead of 1
vec2d vec2d::normalizeStart() const {
  //NOTE: will not normalize beyond .001, returns this
  cint m = this->magnitude();
  if(m < scale_factor && m > scale_factor) {
    return *this;
  }
  return vec2d((x << NORMALIZE_FACTOR) / m, (y << NORMALIZE_FACTOR) / m);
}

vec2d vec2d::normalizeFinish() const {
  return vec2d((x >> NORMALIZE_FACTOR), (y >> NORMALIZE_FACTOR));
}

//vector dot product represents the area of the parallelogram existing in a
//perpendicular plane whose sides are this and v; a dot product of 0 means the
//vectors are perpendicular
cint vec2d::dot(const vec2d &v) const {
  return ((x * v[0]) + (y * v[1]));
}

//vector cross product represents the area of the parallelogram existing in a 
//parallel plane whose sides are this and v; a cross product of 0 means the 
//vectors are colinear
cint vec2d::cross(const vec2d &v) const {
  return ((x * v[1]) - (y * v[0]));
}

vec2d vec2d::cap(cint scalar_val) const {
  //"cap" a vector's magnitude at a certain value

  //don't "cap" if value is negative
  if(scalar_val < 0) {
    return *this;
  }

  if(this->magnitude() > scalar_val) {
    //too big
    return (this->normalizeStart() * scalar_val).normalizeFinish();
  }
  else {
    //not too big
    return *this;
  }
}

vec2d vec2d::decay(cint scalar_val) const {
  //"decay" a vector by removing a fraction of its magnitude
  cint m = this->magnitude();
  if(m < 5 && m > -5) {
    return(vec2d(0,0));
  }
  return((this->normalizeStart() * (
    (m - (m * scalar_val)/scale_factor) > 0 ? (m - (m * scalar_val)/scale_factor) : 0)
  )).normalizeFinish();
}

cint vec2d::dist(const vec2d &v) const {
  //get the distance between 2 vec2d
  return (cint)sqrt(
    (v[0] - x) * (v[0] - x) +
    (v[1] - y) * (v[1] - y)
  );
}


//------------------------------------------------------------------------------
//compute angles from the vectors

float vec2d::angle_rad() const {
return(std::atan2(y, x));
}

cint vec2d::angle_deg() const {
  //return an angle between 0 and 360
  return(cint((this->angle_rad() * (180/PI) + 90) + 360) % 360);
}


//-----------------------------------------------------------------------------
//and then there's this one just hangin' around down here

std::string vec2d::to_string() const {
  return("{" + std::to_string(x) + ", " + std::to_string(y) + "}");
}

std::ostream &operator<<(std::ostream &output, const vec2d &v) {
  output << "(" << v[0] << ", " << v[1] << ")";
  return output;
}
