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
  const vec2d convert_chunk_index(const vec2d &pos) const;

  //checks the boundaries of the current chunk and compares the source and 
  //destination positions to see if any barriers are crossed
  unsigned char check_rebuff(vec2d &curr_pos, vec2d &prev_pos) const;

  //checks both that there's a gate in this chunk and that they player's within
  //a certain distance from it - setting that to 200 units for now
  bool check_gate(const vec2d &);

  //gets the next map's name 
  std::string get_gate_dest(const vec2d &);

  chunk &get_chunk(vec2d coord) { return c_deque[index(coord[0], coord[1])]; }

  //when a map is created, it spawns in entities absed on xml rules
  void spawn_initial_entities();

private:
  int x_dim;
  int y_dim;

  vec2d start_chunk;

  b_ground bg;

  std::string name;

  void validate();
  void init_c_deque();
  void init_special_chunks();
  void check_barriers(int, int);

  //will be a SINGLE array, with clever referenceing
  std::deque<chunk> c_deque;
  //get index of the map location
  size_t index(int x, int y) const { return (x + x_dim * y); }
  size_t index(float x, float y) const { return (x + x_dim * y); }
  size_t index(const vec2d &v) const { return (v[0] + x_dim * v[1]); }
};



#endif
