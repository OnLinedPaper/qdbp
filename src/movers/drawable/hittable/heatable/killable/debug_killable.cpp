#include "debug_killable.h"
#include "src/movers/drawable/hittable/weapon.h"
d_killable::d_killable(const std::string path) :
  killable(path),
  weapon(
    "/movers/hittable/weapons/" + xmlparse::get().get_xml_string(path + "/first_weapon"
  ))

{ }

void d_killable::move_up()
  { vel[1] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_killable::move_dn()
  { vel[1] += vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_killable::move_lf()
  { vel[0] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_killable::move_rt()
  { vel[0] += vel_accel * t_frame::get().t_adjust(); moved = true; }

void d_killable::boost(bool b) { boosted = b; }


uint8_t d_killable::get_weapon_id() const { return 0; }

bool d_killable::can_fire() { 
  //make sure the player isn't overheated, and the last shot wasn't within
  //the "cooldown" time
  
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

void d_killable::update() { killable::update(); }

void d_killable::draw() const {

  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate_color(image_name, dest_r.x, dest_r.y, 0, last_angle, team_col);


  killable::draw();
}
