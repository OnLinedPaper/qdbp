#ifndef ENTITY_HANDER_H_
#define ENTITY_HANDLER_H_

#include "src/movers/drawable/drawable.h"
#include "src/movers/drawable/mortal/mortal.h"
#include "src/movers/drawable/mortal/weapon.h"
#include "src/movers/drawable/mortal/player.h"
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

  void create_plr(std::string);
  void move_plr(unsigned char);
  void boost_plr(bool);
  void toggle_plr_vent();
  void toggle_plr_regen();
  void teleport_plr(const vec2d &);
  void teleport_plr_new_map();
  void plr_shoot(const vec2d &);

  float 	get_plr_heat_frac();
  float 	get_plr_overheat_frac();
  bool 		get_plr_is_overheat();
  bool    get_plr_is_burnout();
  bool    get_plr_is_vent();
  float 	get_plr_health_frac();
  bool 		get_plr_is_regenerating();
  int 		get_plr_shield_segs();
  float 	get_plr_shield_frac();
  const vec2d 	get_plr_pos();
  const vec2d 	get_plr_vel();

  mortal *DEBUG_get_plr(){return plr;}

//==== GENERIC THINGS ==========================================================

  void set_draw_debug_info(bool b) { draw_debug_info = b; }
  bool get_draw_debug_info() const { return draw_debug_info; }

  void preload_entity_data();
  void prep_for_map_change();
  void finish_map_change();
  int get_entity_count_by_id(const std::string &);
  int get_entity_count_by_name(const std::string &);
  const std::string &entity_name_to_id(const std::string &);

  void update_entities();
  void check_entity_collision();
  void draw_entities();

  void add_npe(const std::string &name);
  void add_npe(const std::string &name, const vec2d &pos, const vec2d &vel);
  void add_npe(mortal *h, const vec2d &pos, const vec2d &vel);
  void find_or_create_npe(const std::string &name, mortal **h);

  //called by a gunner (or child) who would like a shot to be fired
  void request_shot(
    const mortal *parent,
    uint8_t w_id,
    const vec2d &w_pos, const vec2d &w_vel, const vec2d &w_ang, 
    float delay_factor,
    float w_life_ms_mod, float w_life_dist_mod, 
    float w_inacc_mod, float w_vel_mod,
    int w_pierce_mod, float w_damage_mod,
    const SDL_Color &c
  );

private:
  e_handler();

  bool check_category_collision(weapon *w, mortal *h, int type);

  player *plr;
  const std::string entity_xml_root = "/movers/";

  std::vector<mortal *> npe_all;
  std::vector<weapon *> shot_all;

  std::unordered_map<std::string, int> entity_count_by_id;

  bool draw_debug_info;

};

#endif
