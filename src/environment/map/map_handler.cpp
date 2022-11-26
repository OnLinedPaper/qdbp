#include "map_handler.h"
#include "src/entity_handler/entity_handler.h"
#include "src/utils/pather.h"

void map_h::set_map(std::string s) {
  //warn the entity handler to wipe existing entities, and then 
  //change the map
  e_handler::get().prep_for_map_change();

  delete m;
  m = new map(s);

  //teleport the player to the new location and handle the rest
  e_handler::get().finish_map_change();

  //spawn any entities the map starts with
  m->spawn_initial_entities();

}

void map_h::set_map() {
  //warn e_handler for a jump - wipe existing entities
  //TODO: change this to shift entities
  e_handler::get().prep_for_map_change();

  delete m;

  pather p(8, 12);
  p.path(4, 0.4);
  m = new map(p);

  //teleport player to new location, and spawn other npes near it
  e_handler::get().finish_map_change();
}

void map_h::jump() {
  //check the player's position - if it's near a jump gate, try to
  //jump to a new map
  return;
}

bool map_h::try_jump() {

  if(m->check_gate(e_handler::get().get_plr_pos())) {
    //the player's close enough to the gate to be able to make the jump
    //change the map to its new location
    //TODO: instead of changing map, make this into the keygate and
    //spawn a new map via pather
    set_map();
    //set_map("/" + m->get_gate_dest(e_handler::get().get_plr_pos()));
    return true;
  }
  return false;
}

void map_h::update() {
  m->spawn_closet_entities();
}
