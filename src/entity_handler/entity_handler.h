#ifndef ENTITY_HANDER_H_
#define ENTITY_HANDLER_H_

#include "src/movers/drawable/drawable.h"
#include "src/movers/drawable/hittable/hittable.h"
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
  void teleport_player(const vec2d &);
  void teleport_player_new_map();
  const vec2d get_player_pos();

//==== GENERIC THINGS ==========================================================

  void update_entities();
  void draw_entities();


private:
  e_handler();
  hittable *player;



};

#endif
