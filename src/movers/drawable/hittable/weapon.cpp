#include <random>

#include "weapon.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"

weapon::weapon(const std::string path) :
  hittable(path),
  heat_per_shot(xmlparse::get().get_xml_float(
    path + "/shot/heat_per_shot"
  )),
  ms_lifespan(xmlparse::get().get_xml_float(
    path + "/shot/ms_lifespan"
  )),
  dist_lifespan(xmlparse::get().get_xml_float(
    path + "/shot/dist_lifespan"
  )),
  inaccuracy(xmlparse::get().get_xml_float(
    path + "/shot/inaccuracy"
  )),
  life_ms_mod(1),
  life_dist_mod(1),
  con_vel(xmlparse::get().get_xml_float(
    path + "/shot/constant_vel"
  )),
  type_id(xmlparse::get().get_xml_int(
    path + "/shot/unique_type_id"
  ))
{ this->set_active(false); }

weapon::~weapon()
{ }

void weapon::draw() const {
  image_handler::get().draw_rotate_color(image_name, pos[0], pos[1], 0, last_angle, team_col);
}

//check to see if the weapon can fire (time based) and then
//return the heat required to shoot it
float weapon::fire(const vec2d start_pos, const vec2d start_vel, 
    const vec2d angle,
    float heat_mod, float life_ms_mod,
    float life_dist_mod, float inacc_mod,
    float vel_mod, const SDL_Color &c) {

 //spawn projectile 
  set_pos(start_pos);
  
  std::random_device rd;
  std::uniform_int_distribution<> distrib(-16, 16);

  //get a random vec
  vec2d randm(distrib(rd), distrib(rd));

  //now, create the additional speed it will use
  vec2d add_vel = (
      (angle.normalize() * (1 - inaccuracy)) +
      (randm.normalize() * (inaccuracy))
  ).normalize() * (con_vel * vel_mod);

  set_vel(start_vel + add_vel);

  //save these for when updating the projectile
  this->life_ms_mod = ms_lifespan * life_ms_mod;
  this->life_dist_mod = dist_lifespan * life_dist_mod;

  //save the color
  set_col(c);

 
  //bring this projectile into update and draw cycles
  set_active(true);

  return heat_per_shot * heat_mod;
}

void weapon::update() {
  //check to see if the projectile has outlived its distance or time
  if(life_ms_mod <= 0 || life_dist_mod <= 0) {
    set_active(false);
  }
  else {
    hittable::update();

    //subtract time
    life_ms_mod -= t_frame::get().get_elapsed();

    //subtract distance
    life_dist_mod -= vel.magnitude();
  }

}
