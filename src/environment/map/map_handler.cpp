#include "map_handler.h"

void map_h::set_map(std::string s) {
  //TODO: warn entity handler to remove all entities, and then
  //make a whole new map
  delete m;
  m = new map(s);
}

void map_h::jump() {
  //check the player's position - if it's near a jump gate, try to
  //jump to a new map
  return;
}