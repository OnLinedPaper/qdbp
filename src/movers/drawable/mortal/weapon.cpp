#include "src/movers/drawable/mortal/weapon.h"
#include "src/timeframe/timeframe.h"
#include "src/image/image_handler.h"
#include "src/utils/message.h"
#include "src/utils/rng.h"

#if defined RENDER_NC
#include "weapon_nc.cpp"
#endif

  std::map<uint8_t, float> weapon::id_to_heat;
  std::map<uint8_t, float> weapon::id_to_delay;
  std::map<uint8_t, std::string> weapon::id_to_path;

weapon::weapon(const std::string path) :
  mortal(path),
  type_id(xmlparse::get().get_xml_int(
    path + "/shot/unique_type_id"
  )),
  parent(NULL),
  life_tick(xmlparse::get().get_xml_float(
    path + "/shot/tick_lifespan"
  )),
  life_dist(xmlparse::get().get_xml_float(
    path + "/shot/dist_lifespan"
  )),
  inaccuracy(xmlparse::get().get_xml_float(
    path + "/shot/inaccuracy"
  )),
  con_vel(xmlparse::get().get_xml_float(
    path + "/shot/constant_vel"
  )),
  impact_damage(xmlparse::get().get_xml_float(
    path + "/damage/impact_damage"
  )),
  armor_pierce(xmlparse::get().get_xml_int(
    path + "/damage/armor_penetration"
  )),
  life_tick_mod(1),
  dist_tick_mod(1),
  impact_damage_mod(1),
  armor_pierce_mod(armor_pierce),
  bypass_shield_mod(false)
{ }

weapon::weapon(uint8_t id) :
  weapon(get_path_from_id(id))
{ }

weapon::~weapon()
{ }

float weapon::get_heat_from_id(uint8_t id) {
  if(id_to_heat.find(id) == id_to_heat.end()) {
    //id wasn't found - make entry to prevent errors
    id_to_heat.insert({id, 0});

    //send message to prevent silent failure
    msg::print_warn(
        ("weapon with id " + std::to_string(id) + " has no heat value assigned to it")
    );
  }

  return(id_to_heat[id]);
}

float weapon::get_delay_from_id(uint8_t id) {
  if(id_to_delay.find(id) == id_to_delay.end()) {
    //id wasn't found - make entry to prevent errors
    id_to_delay.insert({id, 1});

    //send message to prevent silent failure
    msg::print_warn(
        ("weapon with id " + std::to_string(id) + " has no delay value assigned to it")
    );
  }

  return(id_to_delay[id]);
}

const std::string weapon::get_path_from_id(uint8_t id) {
  if(id_to_path.find(id) == id_to_path.end()) {
    //id wasn't found - make entry to prevent errors
    id_to_path.insert({id, "BAD WEAPON ID"});

    //send message to prevent silent failure
    msg::print_warn(
      ("weapon with id " + std::to_string(id) + " has no path value assigned to it")
    );
  }

  return(id_to_path[id]);
}

void weapon::strike_target(mortal &target, int box_type) {
  //depends on weapon - the vast majority will just do damage, but more exotic
  //effects like healing or slowdowns may apply

  target.take_damage(impact_damage * impact_damage_mod, box_type, bypass_shield_mod);
  armor_pierce_mod -= 1;
  if(armor_pierce_mod < 0) {
    this->perish();
  }

  die_on_update();  //don't let high-piwercing projectiles pass through more than 1 target

  return;
}

void weapon::preload_weapon_data() {
  std::string path = "/movers/weapons";

  //get all weapon names
  std::vector<std::string> weap_names = xmlparse::get().get_all_child_tags(path);

  //cycle through all collected names and load their data
  //(assume all names are unique and warn the user if they aren't)
  for(std::string s : weap_names) {
    float id = xmlparse::get().get_xml_float(
      path + "/" + s + "/shot/unique_type_id"
    );

    //sanity check - does this id exist yet in the arrays?
    if(id_to_heat.find(id) != id_to_heat.end() 
        || id_to_delay.find(id) != id_to_delay.end()) {
      //this id is already in the array, warn the user and skip this iteration
      msg::print_warn("weapon with id " + std::to_string(id) + " and name " 
          + s + " already exists in memory! (is the name unique?)");
      continue;
    }

    float heat = xmlparse::get().get_xml_float(
      path + "/" + s + "/shot/heat_per_shot"
    );
    float delay = xmlparse::get().get_xml_float(
      path + "/" + s + "/shot/tick_shot_delay"
    );

    //load the values
    id_to_heat.insert({id, heat});
    id_to_delay.insert({id, delay});
    id_to_path.insert({id, path + "/" + s});
  }

  return;
}

#if defined RENDER_SDL
void weapon::draw() const {
  image_handler::get().draw_v2(image_name, pos[0], pos[1], last_angle, false, 0, team_col, 1);
}
#endif

void weapon::fire(
  const mortal *p,
  const vec2d &start_pos, const vec2d &start_vel, const vec2d &angle,
  float life_tick_mod, float life_dist_mod,
  float inacc_mod,
  float vel_mod,
  int pierce_mod, float damage_mod,
  const SDL_Color &c
) {
  parent = p;

  //reset stats
  this->reset();

  //spawn projectile
  set_pos(start_pos);

  //get a random vec
  vec2d randm((int)(rng::get().get_shot() % 63 - 31), (int)(rng::get().get_shot() % 63 - 31));

  //create velocity, using velocity of parent and weapon, plus some randomness
  vec2d add_vel = (
    (angle.normalize() * (1 - inaccuracy)) +
    (randm.normalize() * (inaccuracy))
  ).normalize() * (con_vel * vel_mod);

  set_vel(start_vel + add_vel);

  //save these for updating projectile
  this->life_tick_mod = life_tick * life_tick_mod;
  this->dist_tick_mod = life_dist * life_dist_mod;

  //save the color
  set_col(c);

  //reset armor piercing
  armor_pierce_mod = armor_pierce + pierce_mod;

  //bring projectile into collision, update, and draw cycles
  set_tangible(true);
  set_active(true);

  return; 
}

void weapon::update() {
  //check to see if projectile went too far or lived too long
  if(life_tick_mod <= 0 || dist_tick_mod <= 0) {
    this->perish();
  }
  else {
    mortal::update();

    //subtract time, compensating for lag - slower game ticks means faster 
    //projectile movement and quicker drain on life ticks
    life_tick_mod -= 
        t_frame::get().get_elapsed_t() * t_frame::get().t_adjust();

    //subtract distance, which has already had any lag factored in
    dist_tick_mod -= vel.magnitude();
  }
}
