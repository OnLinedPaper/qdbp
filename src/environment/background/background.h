#ifndef BACKGROUND_H_
#define BACKGROUND_H_

#include <string>

class b_ground {
public:
  b_ground(std::string);
  ~b_ground();

  void shift_offset(double x, double y) { x_offset += x; y_offset += y; }

  void draw() const;

private:

  std::string bg_name;
  double x_offset;
  double y_offset;

};

#endif
