#ifndef VEC2D_H_
#define VEC2D_H_


class vec2d {
  //a 2d vector class, for position and velocity
public:
  vec2d() : x(0), y(0) { }
  vec2d(double xin, double yin) : x(xin), y(yin) { }
  vec2d(vec2d& v) : x(v[0]), y(v[1]) { }


  //these operators let us return values with array notation
  double &operator [](int i);
  double operator [](int i) const;

private:
  double x;
  double y;
};




#endif
