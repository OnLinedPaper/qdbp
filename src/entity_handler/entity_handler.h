#ifndef ENTITY_HANDER_H_
#define ENTITY_HANDLER_H_

#include "src/movers/drawable/drawable.h"
#include "src/movers/drawable/hittable/heatable/heatable.h"
#include "src/movers/drawable/hittable/debug_hittable.h"
#include <vector>

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
  float get_player_heat_percent();
  float get_player_overheat_percent();
  void teleport_player(const vec2d &);
  void teleport_player_new_map();
  const vec2d get_player_pos();
  const vec2d get_player_vel();
  void player_aim(unsigned char);
  void player_shoot();

//==== GENERIC THINGS ==========================================================

  void set_draw_debug_info(bool b) { draw_debug_info = b; }
  bool get_draw_debug_info() const { return draw_debug_info; }

  void update_entities();
  void draw_entities();

  void add_npe(const std::string name, const std::string type);
  void add_npe(const std::string name, const std::string type,
    const vec2d pos, const vec2d vel);

private:
  e_handler();
  heatable *player;
  const std::string entity_xml_root = "/movers/";

  std::vector<hittable *> npe_all;

  bool draw_debug_info;

};

#endif
