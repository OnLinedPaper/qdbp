#include "debug_heatable.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include "src/rect2d/hitbox/hitbox.h"

d_heatable::d_heatable(const std::string path) :
  heatable(path),
  weapon(
    "/movers/weapons/" + xmlparse::get().get_xml_string(path + "/first_weapon"
  ))
{ }

void d_heatable::move_up()
  { vel[1] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_heatable::move_dn()
  { vel[1] += vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_heatable::move_lf()
  { vel[0] -= vel_accel * t_frame::get().t_adjust(); moved = true; }
void d_heatable::move_rt()
  { vel[0] += vel_accel * t_frame::get().t_adjust(); moved = true; }


void d_heatable::boost(bool b) { boosted = b; }


bool d_heatable::can_fire() { 
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

uint8_t d_heatable::get_weapon_id() const { return 0; }

void d_heatable::update() { heatable::update(); }

void d_heatable::draw() const {

  SDL_Rect dest_r;

  dest_r.x = pos[0];
  dest_r.y = pos[1];
  dest_r.w = 128;
  dest_r.h = 128;

  image_handler::get().draw_rotate_color(image_name, dest_r.x, dest_r.y, 0, last_angle, team_col);


  heatable::draw();
}
