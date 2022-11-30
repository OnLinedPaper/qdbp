#ifndef MAP_H_
#define MAP_H_

#include <deque>
#include "src/environment/chunk/chunk.h"
#include "src/environment/background/background.h"
#include "src/utils/pather.h"

class map {
public:
  map(std::string);
//  map(const pather &);
  ~map();

  void draw() const;
  const vec2d & get_start_chunk() const { return start_chunk; }
  const vec2d get_start_pos() const;
  const vec2d convert_chunk_index(const vec2d &pos) const;
  const vec2d & get_acti_dim() const { return (*dim_acti); }
  const vec2d & get_inac_dim() const { return (*dim_inac); }

  //extends the map outwards to the right and shrinks it from the left
  void extend_map(const pather &);

  //checks the boundaries of the current chunk and compares the source and 
  //destination positions to see if any barriers are crossed
  unsigned char check_rebuff(vec2d &curr_pos, vec2d &prev_pos);

  //checks both that there's a gate in this chunk and that they player's within
  //a certain distance from it - setting that to 200 units for now
  bool check_gate(const vec2d &);

  //gets the next map's name 
  std::string get_gate_dest(const vec2d &);

  chunk &get_chunk(const vec2d &coord); 

  //when a map is created, it spawns in entities absed on xml rules
  void spawn_initial_entities();

  //each game tick, the map tries to spawn in reocurring entities
  void spawn_closet_entities();

  int get_end() { return end_chunk[1]; }

  void shift_bg(const vec2d &v) { bg.shift_offset(v[0], v[1]); }

private:
  vec2d dim_a;
  vec2d dim_b;
  vec2d *dim_acti;
  vec2d *dim_inac;

  int sever_point;

  static const unsigned char DEQUE_A;
  static const unsigned char DEQUE_B;

  unsigned char acti_deque;
  unsigned char inac_deque;

  vec2d start_chunk;
  vec2d end_chunk;

  b_ground bg;

  std::string name;
  bool is_pather_gen;

  void validate();
//  void init_c_deque();
  void init_c_deque(std::deque<chunk> *, vec2d *);
  void init_special_chunks();
  void parse_pather(const pather &);
  void parse_spawn_rules();
  void check_barriers(int, int);

  //TODO: get active deque
  void flip_active_deque();
  std::deque<chunk> *c_deque;
  std::deque<chunk> *c_deque_inac;
  std::deque<chunk> c_deque_a;
  std::deque<chunk> c_deque_b;
  //get index of the map location
  std::deque<chunk> * deque_from_index(const vec2d &);
  vec2d * dim_from_index(const vec2d &);
  void truncate_chunk_index(vec2d &i);
  size_t index(int, int, std::deque<chunk> *, vec2d *) const;
  size_t index(float, float, std::deque<chunk> *, vec2d *) const;
  size_t index(const vec2d &, std::deque<chunk> *, vec2d *) const;
};



#endif
