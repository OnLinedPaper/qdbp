#ifndef ENTITY_HANDER_H_
#define ENTITY_HANDLER_H_

#include "src/movers/drawable/drawable.h"
#include "src/movers/drawable/hittable/heatable/heatable.h"
#include "src/movers/drawable/hittable/heatable/killable/killable.h"
#include "src/movers/drawable/hittable/weapon.h"
#include "src/movers/drawable/hittable/debug_hittable.h"
#include <vector>
#include <unordered_map>

class e_handler {
public:

//==== CONSTANT THINGS ========================================================
  static const unsigned char UP;
  static const unsigned char DN;
  static const unsigned char LF;
  static const unsigned char RT;

//==== INSTANCE THINGS ========================================================
  ~e_handler();

  static e_handler &get() {
    static e_handler instance;
    return instance;
  }

//==== PLAYER THINGS ==========================================================

  void create_player(std::string);
  void move_player(unsigned char);
  void boost_player(bool);
  void toggle_player_regen();
  void teleport_player(const vec2d &);
  void teleport_player_new_map();
  void player_shoot(const vec2d);

  float 	get_player_heat_percent();
  float 	get_player_overheat_percent();
  bool 		get_player_is_overheat();
  float 	get_player_health_percent();
  bool 		get_player_is_regenerating();
  int 		get_player_shield_segs();
  float 	get_player_shield_percent();
  const vec2d 	get_player_pos();
  const vec2d 	get_player_vel();

  //TODO: this is for debugging ONLY, do NOT use it in final code
  killable *	DEBUG_get_player() { return player; }

//==== GENERIC THINGS ==========================================================

  void set_draw_debug_info(bool b) { draw_debug_info = b; }
  bool get_draw_debug_info() const { return draw_debug_info; }

  void preload_entity_data();
  int get_entity_count_by_name(const std::string &);
  const std::string &entity_name_to_id(const std::string &);

  void update_entities();
  void check_entity_collision();
  void draw_entities();

  void add_npe(const std::string name);
  void add_npe(const std::string name, const vec2d pos, const vec2d vel);

private:
  e_handler();

  bool check_category_collision(weapon *w, hittable *h, int type);
  void preload_specific_entities(const std::string &, const std::vector<std::string> &);

  killable *player;
  const std::string entity_xml_root = "/movers/";

  std::vector<hittable *> npe_all;
  std::vector<weapon *> shot_all;

  std::unordered_map<std::string, int> entity_count_by_name;
  std::unordered_map<std::string, std::string> entity_name_and_id;

  bool draw_debug_info;

};

#endif
