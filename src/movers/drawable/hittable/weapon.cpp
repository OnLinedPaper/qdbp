#include <random>

#include "weapon.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include "src/utils/message.h"

  std::map<uint8_t, float> weapon::id_to_heat;
  std::map<uint8_t, float> weapon::id_to_delay;

weapon::weapon(const std::string path) :
  hittable(path),
  tick_lifespan(xmlparse::get().get_xml_float(
    path + "/shot/tick_lifespan"
  )),
  dist_lifespan(xmlparse::get().get_xml_float(
    path + "/shot/dist_lifespan"
  )),
  inaccuracy(xmlparse::get().get_xml_float(
    path + "/shot/inaccuracy"
  )),
  life_tick_mod(1),
  life_dist_mod(1),
  con_vel(xmlparse::get().get_xml_float(
    path + "/shot/constant_vel"
  )),
  type_id(xmlparse::get().get_xml_int(
    path + "/shot/unique_type_id"
  )),
  impact_damage(xmlparse::get().get_xml_float(
    path + "/damage/impact_damage"
  )),
  impact_mod(1),
  armor_piercing(xmlparse::get().get_xml_int(
    path + "/damage/armor_penetration"
  )),
  remaining_pierce(armor_piercing)
{ 
  this->set_active(false); 

}

weapon::~weapon()
{ }

float weapon::get_heat_from_id(uint8_t id) {
  if(id_to_heat.find(id) == id_to_heat.end()) {
    //id wasn't found - make an entry for it and throw an error so we don't
    //have this issue again
    id_to_heat.insert({id, 0});

    //send a warning that this id was not found
    msg::print_warn(
        ("weapon with id " + std::to_string(id) + " has no heat value assigned to it")
    );
  }

  return(id_to_heat[id]);
}

float weapon::get_delay_from_id(uint8_t id) {
  if(id_to_delay.find(id) == id_to_delay.end()) {
    //id wasn't found - make an entry for it and throw an error so we don't
    //have this issue again
    id_to_delay.insert({id, 1});

    //send a warning that this id was not found
    msg::print_warn(
      ("weapon with id " + std::to_string(id) + " has no delay value assigned to it")
    );
  }

  return(id_to_delay[id]);
}

void weapon::strike_target() {
  remaining_pierce -= 1;
  if(remaining_pierce < 0) {
    destroy();
  }
}

void weapon::preload_weapon_data() {
  std::string path = "/movers/hittable/weapons";

  //get all weapon names
  std::vector<std::string> weap_names = xmlparse::get().get_all_child_tags(path);

  //cycle through all collected names and load their data
  //(assume the user gave unique ids to each weapon)
  for(std::string s : weap_names) {
  
    float id = xmlparse::get().get_xml_float(
      path + "/" + s + "/shot/unique_type_id"
    );
    float heat = xmlparse::get().get_xml_float(
      path + "/" + s + "/shot/heat_per_shot"
    );
    float delay = xmlparse::get().get_xml_float(
      path + "/" + s + "/shot/tick_shot_delay"
    );

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
      //load heat

      if(id_to_heat.find(id) == id_to_heat.end()) {
      //id not found, insert it
      id_to_heat.insert({id, heat});
    }

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   
    //load shot delay

    if(id_to_delay.find(id) == id_to_delay.end()) {
      //if not found, insert it
      id_to_delay.insert({id, delay});
    }

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   

  }
  return;
}

void weapon::draw() const {
  image_handler::get().draw_rotate_color(image_name, pos[0], pos[1], 0, last_angle, team_col);
}

//return the heat required to shoot the weapon
float weapon::fire(const vec2d start_pos, const vec2d start_vel, 
    const vec2d angle,
    float heat_mod, float life_tick_mod,
    float life_dist_mod, float inacc_mod,
    float vel_mod, int pierce_mod, 
    float damage_mod, const SDL_Color &c) {

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
  this->life_tick_mod = tick_lifespan * life_tick_mod;
  this->life_dist_mod = dist_lifespan * life_dist_mod;

  //save the color
  set_col(c);

  //reset its armor piercing
  remaining_pierce = armor_piercing * pierce_mod;
 
  //bring this projectile into collision, update, and draw cycles
  set_tangible(true);
  set_active(true);

  return id_to_heat[type_id] * heat_mod;
}

void weapon::update() {
  //check to see if the projectile has outlived its distance or time
  if(life_tick_mod <= 0 || life_dist_mod <= 0) {
    set_active(false);
  }
  else {
    hittable::update();

    //subtract time, compensating for lag - if the game is ticking slowly, 
    //the projectile is moving faster, so kill it sooner
    life_tick_mod -= 
        t_frame::get().get_elapsed_t() * t_frame::get().t_adjust();

    //subtract distance
    life_dist_mod -= vel.magnitude();
  }

}
