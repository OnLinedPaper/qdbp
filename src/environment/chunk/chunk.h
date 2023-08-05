#ifndef CHUNK_H_
#define CHUNK_H_

#include "src/vec2d/vec2d.h"
#include "src/image/image.h"
#include "types.h"
#include <vector>

class chunk {
public:
  chunk(vec2d&);
  chunk(cint, cint);
  chunk(vec2d&, bool, bool, bool, bool);
  chunk(cint, cint, bool, bool, bool, bool);
  chunk(cint, cint, bool, bool, bool, bool, std::string);

  chunk(const chunk&);
  chunk &operator=(const chunk&);

  ~chunk();

  static const unsigned char IN;
  static const unsigned char UP;
  static const unsigned char DN;
  static const unsigned char LF;
  static const unsigned char RT;

  static const cint length;

  static const uint8_t INITIAL;
  static const uint8_t CLOSET;

  //reset a chunk's data
  void rechunk(cint, cint, bool, bool, bool, bool, std::string);

  //check if a given point is in a chunk - if not, check if it's above,
  //below, left, or right
  unsigned char chunk_pos(vec2d&) const;
  unsigned char chunk_pos(cint, cint) const;
  unsigned char chunk_rebuff(vec2d &) const;
  unsigned char chunk_rebuff_forced(vec2d &) const;

  //check if the edges are "borders", that is, if they touch nothing
  bool is_b_up() const { return border[0]; }
  bool is_b_dn() const { return border[1]; }
  bool is_b_lf() const { return border[2]; }
  bool is_b_rt() const { return border[3]; }

  void set_b_up(bool b) { border[0] = b; }
  void set_b_dn(bool b) { border[1] = b; }
  void set_b_lf(bool b) { border[2] = b; }
  void set_b_rt(bool b) { border[3] = b; }

  //if it's in bounds, set a barrier; else, un-set it
  void set_bound_b_up() { border[0] = in_bounds; }
  void set_bound_b_dn() { border[1] = in_bounds; }
  void set_bound_b_lf() { border[2] = in_bounds; }
  void set_bound_b_rt() { border[3] = in_bounds; }

  const vec2d get_midpoint() const { return vec2d(
    tlc[0] + length/2,
    tlc[1] + length/2
  );
  }

  void shift_tlc(cint x, cint y) { tlc[0] += x; tlc[1] += y; }

  bool get_in_bounds() const { return in_bounds; }
  void set_in_bounds(bool i) { in_bounds = i; }
  bool get_has_gate() const { return has_gate; }
  std::string get_gate_dest() const { return g_dest; }

  void add_gate(std::string dest, std::string name); 
  void remove_gate();

  //adds an entity spawning rule to the vector of spawn rules
  void add_spawn_rule (
      uint8_t, int, int, int, const std::string &, uint8_t, cint, cint, 
      cint, cint, cint, cint, float
  );
  
  //spawn any entities that are coded into the xml - other entities, such as
  //closet-spawned ones, are handled later
  //initial entities ignore spawn cap but are otherwise almost identical to
  //closets
  void spawn_initial_entities();
  //spawn any entities that are coded to appear after player arrival
  void spawn_closet_entities();

  void draw() const;
  void debug_draw() const;

private:

  void spawn_entities(uint8_t spawn_type);

  //a set of rules for spawning a specific type of entity
  struct spawn_rule {
    uint8_t spawn_type;           //0: initial, 1: closet
    int max_count;                //allowed active entities
    int total_count;              //total allowed spawns
    int spawned_count;            //count of successful spawns
    int tick_spawn_delay;         //delay before spawn happens
    float spawn_checkpoint;       //timer to start spawning closet entities
    std::string entity;           //abbreviated xml path to entity
    std::string id;               //standardized entity id
    uint8_t team;                 //see entity handler for team codes
    cint min_spawn_distance;     //min distance from player before spawn
    cint max_spawn_distance;     //max distance from player that allows spawn
    cint x_coord;                //x coordinate in the chunk to spawn entity at
    cint y_coord;                //y coordinate in the chunk to spawn entity at
    cint x_dir_comp;             //x direction component
    cint y_dir_comp;             //y direction component
    float vel_frac;               //fraction of max velocity the entity spawns with
  };

  vec2d tlc; //top-left corner
  bool border[4];

  chunk();

  std::string i1_name;
  std::string i2_name;
  float frame_bump;

  bool in_bounds;

  bool has_gate;
  std::string g_dest;
  std::string g_name;

  std::vector<spawn_rule *> spawn_rules;

};

#endif
