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

//TODO: refactor other uses of these colors
const uint8_t e_handler::RED = 1;
const uint8_t e_handler::ORG = 2;
const uint8_t e_handler::YLW = 3;
const uint8_t e_handler::GRN = 4;
const uint8_t e_handler::BLU = 5;
const uint8_t e_handler::PRP = 6;
const uint8_t e_handler::BRN = 7;
const uint8_t e_handler::BLK = 8;
const uint8_t e_handler::WHT = 9;

//==== INSTANCE THINGS ========================================================

e_handler::e_handler() :
  draw_debug_info(xmlparse::get().get_xml_int("/draw_debug_info"))
{ }

e_handler::~e_handler() {
  delete plr;

  for(mortal *h : npe_all) { delete h; h = NULL; }
  for(weapon *w : shot_all) { delete w; w = NULL; }
}

//==== GENERIC THINGS =========================================================

//if this is called, the map handler has decided it's time to change maps. if
//entities aren't ready for this, it can lead to a whole host of issues, the 
//most common of which is great numbers of them being out of bounds. 
//first, deactivate all weapon projectiles; they don't ever persist between 
//maps.
//then, deactivate the entities that need to be deactivated; this mostly
//encompasses everything that's not a player ally.
void e_handler::prep_for_map_change() {
  for(weapon *w : shot_all) {
    w->set_active(false);
  }
  for(mortal *m : npe_all) {
    //deactivate anything that doesn't follow through a portal
    if(!m->get_follow_thru_portals()) {
      m->set_active(false);
      entity_count_by_id[m->get_id()] -= 1;
    }
  }
}

//if this is called, the map handler has ACTUALLY changed the map. once it's
//loaded...
//first, teleport the player to the map's starting position
//then, teleport any entities that have remained active to the player's
//position (or nearby), as though they "jumped" too. 
//then, if any entities have the ability to follow a player through a
//portal, process them and their behaviors (pop out on delay? appear randomly
//on the map? who knows? nothing does this yet.)
void e_handler::finish_map_change() {
  //check if there's a player yet - none on startup
  if(plr) {
    plr->set_pos(map_h::get().get_start_pos());
  }

  for(mortal *m : npe_all) {
    if(m->is_active()) {
      m->set_pos(get_plr_pos());
    }
  }
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

void e_handler::update_entities() {
  plr->update();

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
//where n is projectiles and m is entities

//i've made the design decision - weapons are the ONLY entites with hitboxes,
//and mortals are NOT going to interact with one another via hitboxes. any 
//mortal that can deal "contact damage" or any weapon projectile that can be
//"shot down" will be handled the same: a mortal with a weapon directly 
//attached to it. 

//as far as "teams" go, this will be left to the weapon itself to determine; 
//if a weapon can track a player, it can selectively target the player too, and
//passively ignore other entities in its strike_target() function. HOPEFULLY,
//limiting this to JUST determining if entities bump one another should make
//the design process much easier to manage.

//when checking for collision, check shields, then armor, then hit, then weak
//SHIELD boxes (different from health-related shields) take 0 damage, ARMOR
//takes 1/4 damage, HURT takes normal damage, WEAK takes 4x (values may change)

//if a projectile has armor penetration, as soon as it collides with a type of
//box, it immediately deals damage and checks the next weakest queue, bypassing
//all remaining boxes in that queue. striking ARMOR will immediately also check
//collision with HURT and then WEAK. each count of penetration punches deeper 
//into the ship. 
//(note: this usually is only noticable if the projectile is moving very fast)

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
  plr->draw();

  for(weapon *w : shot_all) { 
    if(w->is_active()) { w->draw(); }
  }
  for(mortal *h : npe_all) { 
    if(h->is_active()) { h->draw(); }
  }

  if(draw_debug_info) {
    plr->draw_boxes();
    for(mortal *h : npe_all) { h->draw_boxes(); }
    for(weapon *w : shot_all) { w->draw_boxes(); }
  }
}

void e_handler::add_npe(const std::string &name) {
  //if no position is specified, place this exactly atop the player,
  //with the player's vel
  //this is mostly for debugging purposes
  add_npe(name, get_plr_pos(), get_plr_vel());
}

void e_handler::add_npe(const std::string &name,
    const vec2d &pos, const vec2d &vel) {

  mortal *h = NULL;
  find_or_create_npe(name, &h);
  add_npe(h, pos, vel);

}

void e_handler::add_npe(mortal *h, 
    const vec2d &pos, const vec2d &vel) {
  //a function to add a hittable to the vector - note that this
  //shouldn't really be called from the engine, it should be
  //used "behind the scenes" for loading from a map or the like

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
  else {
    msg::get().print_warn("failed to add entity! passed mortal was NULL");
  }
}

void e_handler::find_or_create_npe(const std::string &name,
    mortal **h) {
  if(*h) {
    delete(*h);
  }
  std::string type = xmlparse::get().get_xml_string(
    entity_xml_root + name + "/entity_type"
  );

  for(mortal *hi : npe_all) {
    if(type.compare(hi->get_type()) && !hi->is_active()) {
      //found an inactive instance of the entity type we want
      *h = hi;
      break;
    }
  }

  if(!*h) {
    //couldn't find an instance of this hittable that exists already

    if(!type.compare("d_follower"))
      { *h = new d_follower(entity_xml_root + name); }
    else if(!type.compare("d_hittable"))
      { *h = new d_hittable(entity_xml_root + name); }
    else if(!type.compare("d_killable"))
      { *h = new d_killable(entity_xml_root + name); }
    else {
      msg::print_warn("didn't recognize entity type \"" + type + 
          "\" while trying to spawn entity \"" + name + "\""
      );
      msg::print_alert(
          "tried to check at " + entity_xml_root + name + "/entity_type"
      );
    }
  }

  if(!*h) { msg::get().print_warn("failed to find or create entity with name \"" + name + "\"!"); }
}

//when a gunner wants to shoot its weapon, this function is called. all
//determination of whether it CAN shoot the weapon is its responsibility. 
//request_shot takes data about the shot and uses it to create (or reactivate)
//a weapon entity, handle its data, and launch it at its target.
//if the game is lagging significantly, request_shot adjusts itself to fire 
//multiple projectiles as a factor of how hard the game has lagged since the
//last shot. 
void e_handler::request_shot (
  const mortal *parent,
  uint8_t w_id, const vec2d &w_pos, const vec2d &w_vel, const vec2d &w_ang, 
  float delay_factor, 
  float w_life_tick_mod, float w_life_dist_mod, float w_inacc_mod,
  float w_vel_mod, int w_pierce_mod, float w_damage_mod, const SDL_Color &c
) {
  //TODO: lag compensation
  weapon *weap = NULL; 
  for(weapon *w : shot_all) {
    if(w->is_type(w_id) && !w->is_active()) {
      weap = w;
      break;
    }
  }
  if(weap == NULL) {
    weap = new weapon(w_id);
    shot_all.push_back(weap);
  }

  weap->fire(parent, w_pos, w_vel, w_ang, w_life_tick_mod, w_life_dist_mod, w_inacc_mod, w_vel_mod, w_pierce_mod, w_damage_mod, c);
}

uint8_t e_handler::team_str_to_int(const std::string &s) {
  if(!s.compare("RED")) { return e_handler::RED; }
  if(!s.compare("ORG") || !s.compare("ORANGE")) { return e_handler::ORG; }
  if(!s.compare("YLW") || !s.compare("YELLOW")) { return e_handler::YLW; }
  if(!s.compare("GRN") || !s.compare("GREEN"))  { return e_handler::GRN; }
  if(!s.compare("BLU") || !s.compare("BLUE"))   { return e_handler::BLU; }
  if(!s.compare("PRP") || !s.compare("PURPLE")) { return e_handler::PRP; }
  if(!s.compare("BRN") || !s.compare("BROWN"))  { return e_handler::BRN; }
  if(!s.compare("BLK") || !s.compare("BLACK"))  { return e_handler::BLK; }
  if(!s.compare("WHT") || !s.compare("WHITE"))  { return e_handler::WHT; }
  return -1;
}

std::string e_handler::team_int_to_str(int i) {
  switch(i) {
    case(e_handler::RED):
      return "RED";
    case(e_handler::ORG):
      return "ORG";
    case(e_handler::YLW):
      return "YLW";
    case(e_handler::GRN):
      return "GRN";
    case(e_handler::BLU):
      return "BLU";
    case(e_handler::PRP):
      return "PRP";
    case(e_handler::BRN):
      return "BRN";
    case(e_handler::BLK):
      return "BLK";
    case(e_handler::WHT):
      return "WHT";
    default:
      return "ERR";
  }
}

//==== PLAYER THINGS ==========================================================

void e_handler::create_plr(std::string s) {
  plr = new d_player(entity_xml_root + s);
  plr->set_pos(map_h::get().get_start_pos());
}

void e_handler::move_plr(unsigned char c) {
  if(c & UP) { plr->move_up(); }
  if(c & DN) { plr->move_dn(); }
  if(c & LF) { plr->move_lf(); }
  if(c & RT) { plr->move_rt(); }
}

void e_handler::boost_plr(bool b) {
  plr->boost(b);
}

void e_handler::toggle_plr_vent() {
  plr->toggle_vent();
}

void e_handler::toggle_plr_regen() {
  plr->toggle_regen_h();
}

void e_handler::plr_shoot(const vec2d &angle) {
  //get player to request a shot
  plr->shoot(angle);
}

float e_handler::get_plr_heat_frac() {
  return plr->get_heat_frac();
}

float e_handler::get_plr_overheat_frac() {
  return plr->get_overheat_frac();
}

bool e_handler::get_plr_is_overheat() {
  return plr->is_overheated();
}

bool e_handler::get_plr_is_burnout() {
  return plr->is_burntout();
}

bool e_handler::get_plr_is_vent() {
  return plr->is_venting();
}

float e_handler::get_plr_health_frac() {
  return plr->get_health_frac();
}

bool e_handler::get_plr_is_regenerating() {
  return plr->is_regen_h();
}

int e_handler::get_plr_shield_segs() {
  return plr->get_shields();
}

float e_handler::get_plr_shield_frac() {
  return plr->get_shield_frac();
}

void e_handler::teleport_plr(const vec2d &p) {
  plr->teleport(p);
}

void e_handler::teleport_plr_new_map() {
  plr->teleport(map_h::get().get_start_pos());
}

const vec2d e_handler::get_plr_pos() {
  return plr->get_pos();
}

const vec2d e_handler::get_plr_vel() {
  return plr->get_vel();
}
