#ifndef MAP_H_
#define MAP_H_

#include <deque>
#include "src/environment/chunk/chunk.h"
#include "src/environment/background/background.h"

class map {
public:
  map(std::string);
  ~map();

  void draw() const;
  const vec2d get_start_chunk() const { return start_chunk; }
  const vec2d get_start_pos() const;
  const vec2d convert_chunk_index(vec2d &pos) const;

  unsigned char check_rebuff(vec2d &curr_pos, vec2d &prev_pos) const;

  chunk &get_chunk(vec2d &coord) { return c_deque[index(coord[0], coord[1])]; }

private:
  int x_dim;
  int y_dim;

  vec2d start_chunk;

  b_ground bg;

  void validate();
  void init_c_deque();

  //will be a SINGLE array, with clever referenceing
  std::deque<chunk> c_deque;
  //get index of the map location
  size_t index(int x, int y) const { return (x + x_dim * y); }
  size_t index(double x, double y) const { return (x + x_dim * y); }
};



#endif
