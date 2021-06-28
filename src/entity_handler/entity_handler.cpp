#include "entity_handler.h"
#include "src/environment/map/map_handler.h"
#include "src/movers/drawable/mortal/debug_entities/debug_player.h"
#include "src/movers/drawable/mortal/debug_entities/debug_killable.h"
#include "src/movers/drawable/mortal/debug_entities/debug_follower.h"
#include "src/movers/drawable/mortal/debug_entities/debug_hittable.h"
#include "src/utils/message.h"

//==== CONSTANT THINGS ========================================================

const unsigned char e_handler::UP = 1;
const unsigned char e_handler::DN = 2;
const unsigned char e_handler::LF = 4;
const unsigned char e_handler::RT = 8;

//==== INSTANCE THINGS ========================================================

e_handler::e_handler() :
  draw_debug_info(xmlparse::get().get_xml_int("/draw_debug_info"))
{ }

e_handler::~e_handler() {
  delete player;

  for(mortal *h : npe_all) { delete h; h = NULL; }
  for(weapon *w : shot_all) { delete w; w = NULL; }
}

//==== GENERIC THINGS =========================================================

void e_handler::preload_entity_data() {
  std::string path = "";
  std::vector<std::string> e_names;

  //preload killables
  path = "/movers/killable";
  e_names = xmlparse::get().get_all_child_tags(path);
  preload_specific_entities(path, e_names);

  //preload heatables
  path = "/movers/heatable";
  e_names = xmlparse::get().get_all_child_tags(path);
  preload_specific_entities(path, e_names);


  //preload hittables
  path = "/movers/hittable";
  e_names = xmlparse::get().get_all_child_tags(path);
  preload_specific_entities(path, e_names);

 
  for(const auto & [key, value] : entity_name_and_id) {
    std::cout << key << " " << value << std::endl;
  } 
}

void e_handler::preload_specific_entities(
    const std::string &path, const std::vector<std::string> &e_names
) {
  for(std::string s : e_names) {
    entity_name_and_id.insert(
      {s, xmlparse::get().get_xml_string(
        path + "/" + s + "/id"
      )}
    );
  }
}

int e_handler::get_entity_count_by_name(const std::string &name) {
  return get_entity_count_by_id(entity_name_to_id(name));
}

int e_handler::get_entity_count_by_id(const std::string &id) {
  if(entity_count_by_id.find(id) 
      == entity_count_by_id.end()) {
    return 0;
  }
  else {
    return(entity_count_by_id[id]);
  }
}

const std::string &e_handler::entity_name_to_id(const std::string &name) {
  if(entity_name_and_id.find(name) != entity_name_and_id.end()) {
    return entity_name_and_id[name];
  }
  else {
    //TODO: proper error handling here for bad data
    throw("S-H-I-T!");
  }
}

void e_handler::update_entities() {
  player->update();

  for(weapon *w : shot_all) { 
    if(w->is_active()) { w->update(); }
  }

  //update each entity - do this last in case others need
  //to be updated first
  for(mortal *h : npe_all) {
    if(h->is_active()) { 
      h->update(); 
      if(!h->is_active()) {
      //entity just died, remove it from counter
      entity_count_by_id[h->get_id()] -= 1;
      }
    }
  }

  //check collision
  check_entity_collision();
}

//check entity collision - note that this may change
//dramatically in the future, as it currently runs in O(n*m) time
//where n is (player) projectiles and m is (hostile) entities

//when checking for collision, check shields, then armor, then hit, then weak
//SHIELD boxes (different from health-related shields) take 0 damage, ARMOR
//takes 1/4 damage, HURT takes normal damage, WEAK takes 4x (values may change)

//if a projectile has armor penetration, as soon as it collides with a type of
//box, it immediately deals damage and checks the next weakest queue, bypassing
//all remaining boxes in that queue. striking ARMOR will immediately also check
//collision with HURT and then WEAK. each count of penetration punches deeper 
//into the ship. 
//(note: this usually is only noticable if the projectile is moving very fast)

//collision also does not currently differentiate based on team
//it also doesn't check for multiple collisions from the same box

//note that the EFFECT of the collision is left up to the weapon itself, to
//accommodate a wider variety of effects than just "oh it takes damage now".
//this may break encapsulation a bit, but i currently believe the benefits 
//will in the long run outweigh the risks.
void e_handler::check_entity_collision() {
  for(mortal *h : npe_all) {
    if(h->is_tangible()) {
      for(weapon *w : shot_all) {
        //check SHIELD boxes
        if(w->is_tangible() && check_category_collision(w, h, hitbox::TYPE_SHIELDBOX)) {
            w->strike_target(*h, hitbox::TYPE_SHIELDBOX);
        }
        
        //check ARMOR boxes
        if(w->is_tangible() && check_category_collision(w, h, hitbox::TYPE_ARMORBOX)) {
          w->strike_target(*h, hitbox::TYPE_ARMORBOX);
        }

        //check HURT boxes
        if(w->is_tangible() && check_category_collision(w, h, hitbox::TYPE_HURTBOX)) {
          w->strike_target(*h, hitbox::TYPE_HURTBOX);
        }

        //check WEAK boxes
        if(w->is_tangible() && check_category_collision(w, h, hitbox::TYPE_WEAKBOX)) {
          w->strike_target(*h, hitbox::TYPE_WEAKBOX);
        }
      }
    }
  }

  return;
}

bool e_handler::check_category_collision(weapon *w, mortal *h, int type) {
  //check collision of a specific type of box
  return(h->collides(w, hitbox::TYPE_HITBOX, type));
}

void e_handler::draw_entities() {
  player->draw();

  for(weapon *w : shot_all) { 
    if(w->is_active()) { w->draw(); }
  }
  for(mortal *h : npe_all) { 
    if(h->is_active()) { h->draw(); }
  }

  if(draw_debug_info) {
    player->draw_boxes();
    for(mortal *h : npe_all) { h->draw_boxes(); }
    for(weapon *w : shot_all) { w->draw_boxes(); }
  }
}

void e_handler::add_npe(const std::string name) {
  //if no position is specified, place this exactly atop the player,
  //with the player's vel
  //this is mostly for debugging purposes
  add_npe(name, get_player_pos(), get_player_vel());
}

void e_handler::add_npe(const std::string name,
    const vec2d pos, const vec2d vel) {
    //a function to add a hittable to the vector - note that this
    //shouldn't really be called from the engine, it should be
    //used "behind the scenes" for loading from a map or the like

    mortal *h = NULL;
    std::string type = xmlparse::get().get_xml_string(
      entity_xml_root + name + "/entity_type"
    );

    for(mortal *hi : npe_all) {
      if(type.compare(hi->get_type()) && !hi->is_active()) {
        //found an inactive instance of the entity type we want
        h = hi;
        break;
      }
    }

    if(!h) {
      //couldn't find an instance of this hittable that exists already

      if(!type.compare("d_follower"))
        { h = new d_follower(entity_xml_root + name); }
      else if(!type.compare("d_hittable"))
        { h = new d_hittable(entity_xml_root + name); }
      else if(!type.compare("d_killable"))
        { h = new d_killable(entity_xml_root + name); }
      else {
        msg::print_warn("didn't recognize entity type \"" + type + 
            "\" while trying to spawn entity \"" + name + "\""
        );
        msg::print_alert(
            "tried to check at " + entity_xml_root + name + "/entity_type"
        );
      }
    }

    if(h) { 
      h->set_pos(pos);
      h->set_vel(vel);
      npe_all.push_back(h);

      //increment entity count for tracking spawn limits
      if(entity_count_by_id.find(h->get_id()) == entity_count_by_id.end()) {
        entity_count_by_id.insert({h->get_id(), 1});
      } 
      else {
        entity_count_by_id[h->get_id()] += 1;
      }
    }
  }

//==== PLAYER THINGS ==========================================================

void e_handler::create_player(std::string s) {
  player = new d_player(entity_xml_root + s);
  player->set_pos(map_h::get().get_start_pos());
}

void e_handler::move_player(unsigned char c) {
  if(c & UP) { player->move_up(); }
  if(c & DN) { player->move_dn(); }
  if(c & LF) { player->move_lf(); }
  if(c & RT) { player->move_rt(); }
}

void e_handler::boost_player(bool b) {
  player->boost(b);
}

void e_handler::toggle_player_regen() {
  player->toggle_regen();
}

void e_handler::player_shoot(const vec2d angle) {
  //first thing to do is see if we can shoot at all
  if(!player->can_fire()) { return; }

  //second thing to do is get the projectile we're going to shoot
  uint8_t w_id = player->get_weapon_id();

  weapon *weap = NULL;
  for(weapon *w : shot_all) {
    if(w->is_type(w_id) && !w->is_active()) {
      //we found an inactive projectile that meets our criteria
      weap = w;
      break;
    }
  }
  if(weap == NULL) {
    weap = new weapon(player->get_weapon());
    shot_all.push_back(weap);
  }

  //third is to spawn / activate the weapon and send it on its way 
  weap->fire(player->get_pos(), player->get_vel(),
      angle, 1, 1, 1, 1, 1, 1, 1, player->get_col());

  //fourth is to add some heat to the player
  player->heat_up(weapon::get_heat_from_id(w_id)); 
}

float e_handler::get_player_heat_percent() {
  return player->get_heat_percent();
}

float e_handler::get_player_overheat_percent() {
  return player->get_overheat_percent();
}

bool e_handler::get_player_is_overheat() {
  return player->is_overheat();
}

float e_handler::get_player_health_percent() {
  return player->get_health_percent();
}

bool e_handler::get_player_is_regenerating() {
  return player->is_regen();
}

int e_handler::get_player_shield_segs() {
  return player->get_shields();
}

float e_handler::get_player_shield_percent() {
  return player->get_shield_percent();
}

void e_handler::teleport_player(const vec2d &p) {
  player->teleport(p);
}

void e_handler::teleport_player_new_map() {
  player->teleport(map_h::get().get_start_pos());
}

const vec2d e_handler::get_player_pos() {
  return player->get_pos();
}

const vec2d e_handler::get_player_vel() {
  return player->get_vel();
}
