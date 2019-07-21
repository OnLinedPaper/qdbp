#ifndef MAP_H_
#define MAP_H_

#include <deque>
#include "src/environment/chunk/chunk.h"

class map {
public:
  map(std::string);
  ~map();

  void draw() const;
  vec2d get_start_chunk() const { return start_chunk; }
  vec2d get_start_pos() const;

private:
  int x_dim;
  int y_dim;

  vec2d start_chunk;

  void init_c_deque();

  //will be a SINGLE array, with clever referenceing
  std::deque<chunk> c_deque;
  //get index of the map location
  size_t index(int x, int y) const { return (x + x_dim * y); }
  size_t index(double x, double y) const { return (x + x_dim * y); }
};



#endif
