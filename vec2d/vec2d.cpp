#include "vec2d.h"


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
    }
  }
