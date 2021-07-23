#include "src/movers/drawable/mortal/debug_entities/debug_player.h"
#include "src/movers/drawable/mortal/weapon.h"

d_player::d_player(const std::string path) :
  player(path),
  weapon(
      "/movers/weapons/" + xmlparse::get().get_xml_string(
          path + "/first_weapon"
      )
  )
 
{ }

void d_player::move_up()
  { vel[1] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_player::move_dn()
  { vel[1] += vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_player::move_lf()
  { vel[0] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_player::move_rt()
  { vel[0] += vel_accel * t_frame::get().t_adjust(); moved = true; }

void d_player::boost(bool b) { boosted = b; }

uint8_t d_player::get_weapon_id() const { return 0; }

bool d_player::can_fire() {
  //make sure player isn't overheated, and last shot wasn't in cooldown

  if(!is_overheat()) {
    static float prev_t = 0;
    float curr_t = t_frame::get().get_t();

    if(weapon::get_delay_from_id(this->get_weapon_id()) < (curr_t - prev_t)) {
      prev_t = curr_t;
      return true;
    }
  }

  return false;
}

void d_player::update() { mortal::update(); }

void d_player::draw() const {

  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate_color_outline(
    image_name, dest_r.x, dest_r.y, 0, last_angle, team_col, true, get_health_percent() 
  );


  mortal::draw();
}
