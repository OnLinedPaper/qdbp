#ifndef MAP_HANDLER_H_
#define MAP_HANDLER_H_

#include "src/environment/map/map.h"

//a singleton to handle map processing by all entities
class map_h {
public:
  ~map_h() { delete m; }

  static map_h &get() {
    static map_h instance;
    return instance;
  }

  void set_map(std::string s) { delete m; m = new map(s); }

  vec2d get_start_pos() const { return m->get_start_pos(); }
  vec2d get_start_chunk() const { return m->get_start_chunk(); }
  void draw() const { m->draw(); }

private:
  map_h() : m(NULL) { }

  map_h(const map_h &) = delete;
  map_h & operator=(const map_h &) = delete;


  map *m;


};

#endif
