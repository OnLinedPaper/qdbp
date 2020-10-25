#include <iostream>
#include "vec2d.h"

int main(void) {

  vec2d vec(-3, 4);
  vec2d ved(4, .5);

  std::cout << "dist " << vec.dist(ved) << "\n;";
}
