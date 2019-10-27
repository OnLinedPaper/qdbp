#include "entity_handler.h"
#include "src/environment/map/map_handler.h"

//==== CONSTANT THINGS ========================================================

const unsigned char e_handler::UP = 1;
const unsigned char e_handler::DN = 2;
const unsigned char e_handler::LF = 4;
const unsigned char e_handler::RT = 8;

//==== INSTANCE THINGS ========================================================

e_handler::e_handler() { }
e_handler::~e_handler() {
  delete player;
}

//==== GENERIC THINGS =========================================================

void e_handler::update_entities() {
  player->update();
}

void e_handler::draw_entities() {
  player->draw();
}

//==== PLAYER THINGS ==========================================================

void e_handler::create_player(std::string s) {
  player = new d_hittable(s);
  player->set_pos(map_h::get().get_start_pos());
}

void e_handler::move_player(unsigned char c) {
  if(c & UP) { player->move_up(); }
  if(c & DN) { player->move_dn(); }
  if(c & LF) { player->move_lf(); }
  if(c & RT) { player->move_rt(); }
}

void e_handler::teleport_player(const vec2d &p) {
  player->teleport(p);
}

void e_handler::teleport_player_new_map() {
  player->teleport(map_h::get().get_start_pos());
}

const vec2d e_handler::get_player_pos() {
  return player->get_pos();
}
