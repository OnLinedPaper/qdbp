#include <deque>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include "map.h"
#include "src/xml_parser/xmlparse.h"
#include "src/utils/message.h"
#include "src/entity_handler/entity_handler.h"

const unsigned char map::DEQUE_A = 0;
const unsigned char map::DEQUE_B = 1;

//make an empty deque to the left and a normal active deque to the right.
map::map(std::string n) :
  acti_deque(DEQUE_A),
  inac_deque(DEQUE_B),
  bg(xmlparse::get().get_xml_string(n + "/background")),
  name(n),
  is_pather_gen(false)
{
  //deque a is always the first active deque, and b is the first inactive
  //create the empty b first - does not NEED to be empty actually
  dim_b = {4, 4};
  vec2d temp_dim = {0, 0};
  dim_inac = &temp_dim;
  init_c_deque(&c_deque_b, &dim_b);
  dim_inac = &dim_b;
  c_deque_inac = &c_deque_b;

  //now get the dimensions for a
  dim_a[0] = xmlparse::get().get_xml_int(name + "/dimensions/x_dim");
  dim_a[1] = xmlparse::get().get_xml_int(name + "/dimensions/y_dim");
  dim_acti = &dim_a;

  start_chunk = vec2d(
    xmlparse::get().get_xml_int(name + "/start_chunk/x_chunk"),
    xmlparse::get().get_xml_int(name + "/start_chunk/y_chunk")
  );
    end_chunk = vec2d(
    xmlparse::get().get_xml_int(name + "/end_chunk/x_chunk"),
    xmlparse::get().get_xml_int(name + "/end_chunk/y_chunk")
  );
  

  validate();

  //go init the new active map
  init_c_deque(&c_deque_a, &dim_a);
  c_deque = &c_deque_a;

  //check the left size border of the active deque for borders between it and
  //the inactive deque
  for(int i=0; i<(*dim_acti)[1]; i++) {
    check_barriers(0, i, true);
  }

  //set the sever point - the spot between the deques
  sever_point = chunk::length * (*dim_inac)[0];

  //initiate any special chunks
  init_special_chunks();
  //init the spawning rules for each chunk
  parse_spawn_rules();
}

map::~map() { }

//given a pather object, delete the left side of the map (trusting e_handler
//to have already dealt with anything lingering in it) and then build a new
//right side of the map
void map::extend_map(const pather &p) {


  //first, save some data from the inactive deque before it's deleted
  int shift_size = (*dim_inac)[0] * chunk::length;

  //next, clear out the inactive deque
//  (*c_deque_inac).clear();
  //(*dim_inac) = {0, 0};

  //next, shift the entire active deque to the left (the e_handler will later
  //deal with the "displaced" entities
  //also, check every chunk for portals and get rid of all of them
  for(int i=0; i<(*dim_acti)[1]; i++) {
    for(int j=0; j<(*dim_acti)[0]; j++) {
      (*c_deque)[index(j, i, /*c_deque,*/ dim_acti)].shift_tlc(-1 * shift_size, 0);
      (*c_deque)[index(j, i, /*c_deque,*/ dim_acti)].remove_gate();
    }
  }

  //next, stitch up the left side of the active deque
  for(int i=0; i<(*dim_acti)[1]; i++) {
    check_barriers(0, i, false);
  }


  //next, deactivate this deque
  flip_active_deque();


  //from this point on, the active deque is being created
  (*dim_acti)[0] = p.get_c();
  (*dim_acti)[1] = p.get_r();
  start_chunk = vec2d(0, p.get_start());
  end_chunk = vec2d((*dim_acti)[0]-1, (float)p.get_end());

  init_c_deque(c_deque, dim_acti);

  //after init, but before path read, quickly switch decks again to stitch up
  //the left side of the inactive deque
  flip_active_deque();
  flip_active_deque();

  parse_pather(p);

}

//flip the active and passive deques, and associated data
void map::flip_active_deque() {
  if(acti_deque == DEQUE_A) {

    acti_deque = DEQUE_B;
    inac_deque = DEQUE_A;

    c_deque = &c_deque_b;
    dim_acti = &dim_b;

    c_deque_inac = &c_deque_a;
    dim_inac = &dim_a;
  }
  else if (acti_deque == DEQUE_B) {

    acti_deque = DEQUE_A;
    inac_deque = DEQUE_B;

    c_deque = &c_deque_a;
    dim_acti = &dim_a;

    c_deque_inac = &c_deque_b;
    dim_inac = &dim_b;

  }
  else {
    std::string e_msg = "tried to set a nonexistent deque as active!";
    msg::get().print_error("map::flip_active_deque threw error: " + e_msg);
    throw e_msg;
  }
}

//given a deque and a set of dimensions, initialize the deque
//note that this CURRENTLY EXPECTS THE CHUNK TO BE THE ACTIVE ONE
void map::init_c_deque(std::deque<chunk> *c_d, vec2d *d) {
  int up, dn, lf, rt;
  up = dn = lf = rt = 0;

  std::string default_type = "";
  
  //resize the deque to the appropriate size, keeping it as is if needed
  (*c_d).resize((*d)[0] * (*d)[1], {0, 0});

  for(int j=0; j<(*d)[1]; j++) {
    for(int i=0; i<(*d)[0]; i++) {
      up = dn = lf = rt = 0;
      //check for barriers
      if(i == 0) { lf = 1; }
      if(i == (*d)[0] - 1) { rt = 1; }
      if(j == 0) { up = 1; }
      if(j == (*d)[1] - 1) { dn = 1; }


      (*c_d)[index(i, j, /*c_d,*/ d)].rechunk(i+(*dim_inac)[0], j, up, dn, lf, rt, default_type);
    }
  }

  //request the deque be shrunken to size (note that this is implementation
  //specific and is not guaranteed to have any effect)
  (*c_d).shrink_to_fit(); 
}

/*
void map::init_c_deque() {
  //reserve space
  //c_map.reserve(x_dim * y_dim);
  int up, dn, lf, rt;
  up = dn = lf = rt = 0;

  std::string default_type = "";

  flip_active_deque();
  (*c_deque).clear();

  if(!is_pather_gen) {

    if(xmlparse::get().get_xml_string(
      name + "/generation/style").compare("userdef") == 0
    ) {
      default_type = xmlparse::get().get_xml_string(name+"/generation/default_chunk_type");
      //this is a userdef map
    }
  }
  else {
    default_type = "default";
  }

  for(int j=0; j<(*dim_acti)[1]; j++) {
    for(int i=0; i<(*dim_acti)[0]; i++) {
      up = dn = lf = rt = 0;
      //check for barriers
      if(i == 0) { lf = 1; }
      if(i == (*dim_acti)[0] - 1) { rt = 1; }
      if(j == 0) { up = 1; }
      if(j == (*dim_acti)[1] - 1) { dn = 1; }


      (*c_deque).emplace_back(i, j, up, dn, lf, rt, default_type);
    }
  }

  //request the deque be shrunken to size (note that this is implementation
  //specific and is not guaranteed to have any effect)
  (*c_deque).shrink_to_fit();
}
*/

//EXPECTS TO BE CALLED ON THE ACTIVE DEQUE
void map::init_special_chunks() {
  //make path, for convenience's sake
  std::string s_path = name + "/special_chunks";

  if(!xmlparse::get().check_path(s_path)) {
    //special chunks isn't in the xml path - return immediately
    return;
  }

  //get the special chunks
  std::vector<std::string> specials;
  xmlparse::get().get_all_child_tags(s_path, specials);

  //some variables we'll use
  int x = 0;
  int y = 0;
  std::string t = "";

  //some constants for the first function
  bool f = false;

  for(std::string s_c_name : specials) {
    x = xmlparse::get().get_xml_int(s_path + "/" + s_c_name + "/x_chunk");
    y = xmlparse::get().get_xml_int(s_path + "/" + s_c_name + "/y_chunk");
    t = xmlparse::get().get_xml_string(s_path + "/" + s_c_name + "/type");

    std::vector<std::string> traits;
    xmlparse::get().get_all_child_tags(s_path + "/" + s_c_name, traits);

    if(x < 0 || x >= (*dim_acti)[0] || y < 0 || y >= (*dim_acti)[1]) {
      //this chunk isn't valid - skip it
      msg::print_warn("bad coordinate for special chunk " + s_path + "/" +
        s_c_name + " (" + std::to_string(x) + "," + std::to_string(y) + ")");
      msg::print_alert("valid x coords: (0 to " + std::to_string((*dim_acti)[0]-1) +
        "); valid y coords: (0 to " + std::to_string((*dim_acti)[1]-1) + ")");
    }
    else {
      //update a chunk with this new one
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].rechunk(x + (*dim_inac)[0], y, f, f, f, f, t);

      //check the barriers
      check_barriers(x, y, true);

      //add the gate
      if(std::find(traits.begin(), traits.end(), "jump_gate") != traits.end()) {
        (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].add_gate(
          xmlparse::get().get_xml_string(s_path + "/" + s_c_name + "/jump_gate/destination"),
          xmlparse::get().get_xml_string(s_path + "/" + s_c_name + "/jump_gate/body")
        );
      }
    }
  }
}

//==== pather-guided world generation =========================================
void map::parse_pather(const pather &p) {
  bool f = false; 

  //generate the maze structure
  for(int y=0; y<p.get_r(); y++) {
    for(int x=0; x<p.get_c(); x++) {
      if(p.at(x, y) == 0) {
        (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].rechunk(x+(*dim_inac)[0], y, f, f, f, f, "default_impassible");
      }
      check_barriers(x, y, true);
    }
  }

  //place the gate - always at the "point of greatest interest", which is
  //to say, the point furthest from the main path
  std::vector<p_o_i> points;
  p.get_far_point(points, 1);
  (*c_deque)[index(points[0].x, points[0].y, /*c_deque,*/ dim_acti)].add_gate("NONE", "debug_gate");
}

//==== spawn rule parsing =====================================================

void map::parse_spawn_rules() {
  //collect information about spawning data for each chunk, and pass it along
  //to the chunks in question
  std::string s_path = name + "/entity_spawning";

  if(!xmlparse::get().check_path(s_path)) {
    //there's no spawn rules for this map (what?) so return immediately
    return;
  }

  //get the spawn rules
  std::vector<std::string> rules;
  xmlparse::get().get_all_child_tags(s_path, rules);

  int x, y = 0;
  float x_coord, y_coord = 0;
  float x_dir_comp, y_dir_comp = 0;
  float vel_frac = 0;

  std::string spawn_type = "";
  int max_count = 0;
  int total_count = 0;
  int tick_spawn_delay = 0;
  std::string entity = "";
  std::string team = "";
  float min_spawn_distance = 0;
  float max_spawn_distance = 0;

  for(std::string s_r_name : rules) {

//---- x/y coordinates of the chunk -------------------------------------------

    //get the xy index of the chunk for the deque and validate it
    x = xmlparse::get().get_xml_int(s_path + "/" + s_r_name + "/x_chunk");
    y = xmlparse::get().get_xml_int(s_path + "/" + s_r_name + "/y_chunk");

    if(x < 0 || x >= (*dim_acti)[0] || y < 0 || y >= (*dim_acti)[1]) {
      //this chunk isn't valid - skip it
      msg::print_warn("bad chunk coordinate for spawn rule " + s_path + "/" +
        s_r_name + " (" + std::to_string(x) + "," + std::to_string(y) + ")");
      msg::print_alert("valid x coords: (0 to " + std::to_string((*dim_acti)[0]-1) +
        "); valid y coords: (0 to " + std::to_string((*dim_acti)[1]-1) + ")");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

    if(!(*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].get_in_bounds()) {
      //this chunk isn't in bounds
      msg::print_warn("chunk for spawn rule " + s_path + "/" +
        s_r_name + " (" + std::to_string(x) + "," + std::to_string(y) + 
        ") is marked as \"out of bounds\"");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

//---- x/y coordinates in the chunk -------------------------------------------

    //get the x/y coordinates where we'll spawn the entity in the chunk
    //- this can be a float value between 1 and 999, corresponding to the 1000
    //  unit size of a chunk. the center of the entity will be placed here.
    //- passing -1 in either field will result in a random coordinate being 
    //  used at spawn time
    //TODO: switch this up to use chunk::length instead of "1000" - current chunk length is 500!
    x_coord = xmlparse::get().get_xml_float(s_path + "/" + s_r_name + "/x_coord");
    y_coord = xmlparse::get().get_xml_float(s_path + "/" + s_r_name + "/y_coord");
  
    if(x_coord != -1 && (x_coord < 1 || x_coord > 999)) {
      msg::print_warn("bad entity spawning x coordinate \"" + 
        std::to_string(x_coord) + "\"");
      msg::print_alert("value should be between 0 and 999, or -1 for a random x coordinate");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

    if(y_coord != -1 && (y_coord < 1 || y_coord > 999)) {
      msg::print_warn("bad entity spawning y coordinate \"" + 
        std::to_string(y_coord) + "\" for rule " + s_path + "/" + s_r_name);
      msg::print_alert("value should be between 0 and 999, or -1 for a random y coordinate");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

//---- x and y components for entity direction --------------------------------

  //get the x and y components of entity velocity - note that these are NOT the
  //actual velocity that will be used, instead
  //- a velocity component of 0 will result in a random direction; for entities
  //  that are meant to stand still, vel_frac should be 0

  x_dir_comp = xmlparse::get().get_xml_float(s_path + "/" + s_r_name + "/x_dir_component");
  y_dir_comp = xmlparse::get().get_xml_float(s_path + "/" + s_r_name + "/y_dir_component");

  //these don't really need to be validated; they're used as direction
  //components as opposed to the actual velocity itself, and will be normalized
  //before being used

//---- fraction of max vel ----------------------------------------------------

  //get the fraction of max velocity this will spawn with
  //- this cannot be lower than 0, and generally shouldn't be higher than 2,
  //  BUT a value of -1 will result in a random velocity (within acceptable
  //  limits for that entity)

  vel_frac = xmlparse::get().get_xml_float(s_path + "/" + s_r_name + "/vel_frac");

  if(vel_frac != -1 && vel_frac < 0) {
    msg::print_warn("bad velocity component \"" + std::to_string(vel_frac) +
      "\" for rule " + s_path + "/" + s_r_name);
    msg::print_alert("velocity should ideally be between 0 and 1, or -1 for a random velocity");
    msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
    continue;
  }
  if(vel_frac > 2) {
    msg::print_warn("rule " + s_path + "/" + s_r_name + " is spawning an entity moving at " +
      std::to_string(vel_frac) + " times its normal max velocity.");
    msg::print_alert("this may result in unexpected movement behavior, especially if the entity does not have normal velocity decay rules.");
  }

//---- spawn type ------------------------------------------------------------- 

    //get the type of spawn (initial or closet)
    //- initial entities are created only when the map is, and do not reappear
    //- closet entities spawn repeatedly after creation
    spawn_type = xmlparse::get().get_xml_string(s_path + "/" + s_r_name + "/spawn_type"); 

    if(spawn_type.compare("initial") && spawn_type.compare("closet")) {
      //this field didn't match known types
      msg::print_warn("unrecognized spawn type \"" + spawn_type + "\" for rule " +
        s_path + "/" + s_r_name);
      msg::print_alert("valid types are \"initial\" or \"closet\"");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

//---- max count --------------------------------------------------------------
    
    //get the max count (int > 0)
    //- for closet entities (and some map entities), there's a maximum amount
    //  that can exist on the map at any given time
    max_count = xmlparse::get().get_xml_int(s_path + "/" + s_r_name + "/max_count");

    if(max_count <= 0) {
      msg::print_warn("bad max entity count \"" + std::to_string(max_count) + 
        "\" for rule " + s_path + "/" + s_r_name);
      msg::print_alert("max count must be greater than 0");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

//---- total count ------------------------------------------------------------

    //get the total count (int > 0 OR -1)
    //- for closet entities (and some map entities), a certain number are 
    //  spawnable: if given a nonzero positive number, they will spawn until
    //  they run out, and if -1 is given they will spawn indefinitely
    total_count = xmlparse::get().get_xml_int(s_path + "/" + s_r_name + "/total_count");

    if(total_count == 0 || total_count < -1) {
      msg::print_warn("bad total entity count \"" + std::to_string(total_count) +
        "\" for rule " + s_path + "/" + s_r_name);
      msg::print_alert("total count should be either greater than 0 to spawn a fixed number of entites, or -1 to spawn entities continuously.");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

//---- tick spawn delay -------------------------------------------------------

    //get the tick spawn delay (int > 0)
    //- number of ticks before the entity spawns
    //- IGNORED by initial entities as they're supposed to "already
    //  be there" on player spawn
    //- for closet entities, the timer begins to tick immediately upon arrival
    //  in the map, and will begin ticking again when there is an availability,
    //  meaning the delay must fully elapse before next spawn
    tick_spawn_delay = xmlparse::get().get_xml_int(s_path + "/" + s_r_name + "/tick_spawn_delay");

    if(tick_spawn_delay < 0) {
      msg::print_warn("bad spawn delay \"" + std::to_string(tick_spawn_delay) +
        "\" for rule" + s_path + "/" + s_r_name);
      msg::print_alert("spawn delay should be at least 0");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

//---- entity -----------------------------------------------------------------

    //get the entity (string - xml path)
    //- the path of the entity to spawn, as would be passed to entity handler
    entity = xmlparse::get().get_xml_string(s_path + "/" + s_r_name + "/entity");

    if(!xmlparse::get().check_path("/movers/mortal/" + entity)) {
      msg::print_warn("bad entity path \"/movers/mortal/" + entity + "\" for rule " +
        s_path + "/" + s_r_name);
      msg::print_alert("is the entity path spelled correctly?");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }

//---- team -------------------------------------------------------------------

    //get the team (string - RED ORANGE YELLOW GREEN BLUE PURPLE BROWN WHITE)
    //- the team to spawn the entity on
    team = xmlparse::get().get_xml_string(s_path + "/" + s_r_name + "/team");

    if(team.compare("RED") && team.compare("ORANGE") && 
        team.compare("YELLOW") && team.compare("GREEN") && team.compare("BLUE") 
        && team.compare("PURPLE") && team.compare("BROWN")) {
      if(team.compare("WHITE")) {
        msg::print_warn("bad team \"" + team + "\" for rule " + s_path + "/" 
          + s_r_name);
        msg::print_alert(
          "valid teams are RED ORANGE YELLOW GREEN BLUE PURPLE BROWN"
        );
        msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
        continue;
      }
      else {
        msg::print_warn("rule " + s_path + "/" + s_r_name + " is spawning a white team entity");
        msg::print_alert("this team is typically reserved for debugging purposes.");
      }
    }

//---- min spawn distance -----------------------------------------------------

    //get the min spawn distance (float)
    //- the minimum distance away the player must be to spawn these entities
    //- initial entities typically use low values, since the player's position 
    //  is known on map spawn... usually. if an initial entitiy is too close,
    //  it WILL fail to spawn, and make no more attempts
    //- closet entities use this to make sure they don't spawn on top of the
    //  player when they kill another entity, and should use a value at least
    //  as large as the distance between the furthest edges of their hitboxes
    min_spawn_distance = xmlparse::get().get_xml_float(s_path + "/" + 
        s_r_name + "/min_spawn_distance");

    if(min_spawn_distance < 500) {
      if(min_spawn_distance < 0) {
        msg::print_warn("bad min spawn distance \"" + 
          std::to_string(min_spawn_distance) + "\" for rule " + s_path + "/" + 
          s_r_name);
        msg::print_alert("distance should be greater than 0, and ideally greater than 500 to prevent entities from spawning too close to the player");
        msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
        continue;
      }
      else {
        msg::print_warn("rule " + s_r_name + 
          " is spawning an entity at min distance " + 
          std::to_string(min_spawn_distance));
        msg::print_alert("this may spawn very close to the player");
      }
    }

//---- max spawn distance -----------------------------------------------------

  //get the max spawn distance (float)
  //- the maximum distance a player can be, else entity is too far to spawn
  //- initial entities ALWAYS ignore this, as it will often prevent
  //  them from spawning altogether
  //- closet entities use this to make sure they don't spawn somewhere super
  //  remote, depriving a player of the chance to fight the entity
  max_spawn_distance = xmlparse::get().get_xml_float(s_path + "/" + s_r_name +
      "/max_spawn_distance");

  //max spawn dist of -1 means there is no "maximum" and the entity can always
  //spawn regardless of player position
  if(max_spawn_distance != -1) {
    if(max_spawn_distance < min_spawn_distance) {
      msg::print_warn("bad max spawn distance \"" +
        std::to_string(max_spawn_distance) + "\" for rule " + s_path + "/" +
        s_r_name);
      msg::print_alert("max distance (" + std::to_string(max_spawn_distance) +
        ") should not be less than min distance (" + 
        std::to_string(min_spawn_distance) + ") or the entity will not spawn.");
      msg::print_alert("note: a value of -1 will remove the max distance.");
      msg::print_alert("skipping rule " + s_path + "/" + s_r_name);
      continue;
    }
  }

//---- add rule to chunk ------------------------------------------------------

    (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].add_spawn_rule(
      (!spawn_type.compare("initial") ? chunk::INITIAL : chunk::CLOSET),
      max_count,
      total_count,
      tick_spawn_delay,
      entity,
      e_handler::team_str_to_int(team),
      min_spawn_distance,
      max_spawn_distance,
      x_coord,
      y_coord,
      x_dir_comp,
      y_dir_comp,
      vel_frac
    );
  }
}

//=============================================================================

//ALWAYS ASSUMES THIS IS BEING CALLED ON THE ACTIVE DEQUE
void map::check_barriers(int x, int y, bool check_interactions) {
  //a change has been made to this chunk - check all nearby chunks
  //and add or remove barriers as needed

  //first, check to see if this is on the edge of the map
  //if it's in bounds, add a barrier - else, don't
  if(x == 0) { (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_bound_b_lf(); }
  if(y == 0) { (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_bound_b_up(); }
  if(x == (*dim_acti)[0]-1) { (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_bound_b_rt(); }
  if(y == (*dim_acti)[1]-1) { (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_bound_b_dn(); }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //next, check to see if existing chunks need their barriers modified
  //use XOR (^) to check them
  //- if both are in or out of bounds- no barriers
  //- if one is and one isn't- set it accordingly
  //BOTH get barriers, but only in-bounds chunks draw them. this avoids a
  //literal corner case where it's possible to fly into an out of bounds
  //chunk by perfectly hitting its corner

  //check left chunk
  if(x > 0) {
    if((*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].get_in_bounds() ^ (*c_deque)[index(x-1, y, /*c_deque,*/ dim_acti)].get_in_bounds()) {
      //they are not the same
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_lf(true);
      (*c_deque)[index(x-1, y, /*c_deque,*/ dim_acti)].set_b_rt(true);

    }
    else {
      //they are the same - remove barriers
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_lf(false);
      (*c_deque)[index(x-1, y, /*c_deque,*/ dim_acti)].set_b_rt(false);
    }
  }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //check right chunk
  if(x < (*dim_acti)[0] - 1) {
    //check right chunk
    if((*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].get_in_bounds() ^ (*c_deque)[index(x+1, y, /*c_deque,*/ dim_acti)].get_in_bounds()) {
      //they are not the same
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_rt(true);
      (*c_deque)[index(x+1, y, /*c_deque,*/ dim_acti)].set_b_lf(true);

    }
    else {
      //they are the same - remove barriers
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_rt(false);
      (*c_deque)[index(x+1, y, /*c_deque,*/ dim_acti)].set_b_lf(false);
    }
  }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //check up chunk
  if(y > 0) {
    if((*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].get_in_bounds() ^ (*c_deque)[index(x, y-1, /*c_deque,*/ dim_acti)].get_in_bounds()) {
      //they are not the same
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_up(true);
      (*c_deque)[index(x, y-1, /*c_deque,*/ dim_acti)].set_b_dn(true);

    }
    else {
      //they are the same - remove barriers
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_up(false);
      (*c_deque)[index(x, y-1, /*c_deque,*/ dim_acti)].set_b_dn(false);
    }
  }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //check down chunk
  if(y < (*dim_acti)[1] - 1) {
    //check right chunk
    if((*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].get_in_bounds() ^ (*c_deque)[index(x, y+1, /*c_deque,*/ dim_acti)].get_in_bounds()) {
      //they are not the same
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_dn(true);
      (*c_deque)[index(x, y+1, /*c_deque,*/ dim_acti)].set_b_up(true);

    }
    else {
      //they are the same - remove barriers
      (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_dn(false);
      (*c_deque)[index(x, y+1, /*c_deque,*/ dim_acti)].set_b_up(false);
    }
  }


  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
  //SPECIAL - check leftmost wall against the inactive deque
  if(check_interactions && x == 0) {
    //check if there actually is a chunk to the left (not guaranteed as deques might be different sizes)
    if (index(x, y, /*c_deque_inac,*/ dim_inac) != SIZE_MAX) {
      if ((*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].get_in_bounds() ^ (*c_deque_inac)[index((int)(*dim_inac)[0]-1, (int)y, /*c_deque_inac,*/ dim_inac)].get_in_bounds())
      {
        (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_bound_b_lf();
        (*c_deque_inac)[index((int)((*dim_inac)[0]-1), (int)y, /*c_deque_inac,*/ dim_inac)].set_bound_b_rt();
      }
      else {
        (*c_deque)[index(x, y, /*c_deque,*/ dim_acti)].set_b_lf(false);
        (*c_deque_inac)[index((int)(*dim_inac)[0]-1, (int)y, /*c_deque_inac,*/ dim_inac)].set_b_rt(false);
      }
    }
  }

  //-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
}

const vec2d map::get_start_pos() const {
  //check if chunk is in bounds, or find one if it isn't

  if((*c_deque)[index(start_chunk[0], start_chunk[1], /*c_deque,*/ dim_acti)].get_in_bounds()) {
    //it's in bounds
    return((*c_deque)[index(start_chunk[0], start_chunk[1], /*c_deque,*/ dim_acti)].get_midpoint());
  }
  else {
    //it's NOT in bounds
    msg::print_warn("start chunk (" + std::to_string((int)start_chunk[0]) + "," + std::to_string((int)start_chunk[1]) + ") is out of bounds");

    //find one that's in bounds
    for(int j=0; j<(*dim_acti)[1]; j++) {
      for(int i=0; i<(*dim_acti)[0]; i++) {
        if((*c_deque)[index(i, j, /*c_deque,*/ dim_acti)].get_in_bounds()) {
          //this chunk is in bounds

          return((*c_deque)[index(i, j, /*c_deque,*/ dim_acti)].get_midpoint());
        }
      }
    }

    //no in-bounds chunks were found
    std::string error = name + " has no chunks that are in bounds!";
    msg::print_error("map::get_start_pos threw error: " + error);
    throw(error);
  }
}

//convert a set of game coordinates to a set of map chunk coordinates
//this does NOT PERFORM ERROR CHECKING. if given a coordinate system
//outside the map, it will still return an index.
const vec2d map::convert_chunk_index(const vec2d &pos) const {
  return(vec2d(int(pos[0] / chunk::length), int(pos[1] / chunk::length)));
}

//given an index vector, determine the deque it's in
std::deque<chunk> * map::deque_from_index(const vec2d &i) {
  if(i[0] < (*dim_inac)[0]) { return c_deque_inac; }
  else { return c_deque; } 
}

//given an index vector, get the dimensions of the deque it's in
vec2d * map::dim_from_index(const vec2d &i) {
  if(i[0] < (*dim_inac)[0]) { return dim_inac; }
  else { return dim_acti; } 
}

//given a chunk index, truncate it to the chunk it's in
void map::truncate_chunk_index(vec2d &i) {
  if(i[0] < (*dim_inac)[0]) { return; }
  else {
    //shift this to the left
    i[0] -= (*dim_inac)[0];
  }
}

//convert a set of chunk coordinates to an actual chunk
chunk & map::get_chunk(const vec2d &coord) {
  //determine which side of the map this coordinate is on
  if(coord[0] < (*dim_inac)[0]) {
    //it's in the inactive deque
    return((*c_deque_inac)[index(coord[0], coord[1], /*c_deque_inac,*/ dim_inac)]);
  }  
  else {
    //it's in the active deque
    return (*c_deque)[index(coord[0] - (*dim_inac)[0], coord[1], /*c_deque,*/ dim_acti)]; 
  }
}

//given an x position, a y position, a deque, and its dimensions, convert
//the position pair into an actual index where the chunk can be located.
size_t map::index(int x, int y, /*std::deque<chunk> *c_d,*/ vec2d *d) const {
  //sanity checking
  if(x >= (*d)[0] || y >= (*d)[1] || x < 0 || y < 0) { return SIZE_MAX; }

  return (x + (*d)[0] * y);
}
size_t map::index(float x, float y, /*std::deque<chunk> *c_d,*/ vec2d *d) const {
  return index((int)x, (int)y, /*c_d,*/ d);
}
size_t map::index(const vec2d &v, /*std::deque<chunk> *c_d,*/ vec2d *d) const {
  return index(v[0], v[1], /*c_d,*/ d);
}

unsigned char map::check_rebuff(vec2d &curr_pos, vec2d &prev_pos) {

  //check the last chunk that was occupied, and return the position
  //relative to that chunk

  //check to see if the player is about to leave the map entirely - there are
  //no valid chunks on its borders to check rebuff on

  unsigned char rm = 0;
  if(curr_pos[0] < 0) { rm = rm|chunk::LF; }
  if(curr_pos[1] < 0) { rm = rm|chunk::UP; }
  if(curr_pos[0] >= (dim_a[0] + dim_b[0]) * chunk::length) { rm = rm|chunk::RT; }
  if(curr_pos[1] >= dim_a[1] * chunk::length && curr_pos[1] >= dim_b[1] * chunk::length) { rm = rm|chunk::DN; }

  if(rm) { 
    return rm; 
  }

  //check to see if the new chunk is valid - previously, through an oversight,
  //it was possible to leave the map through a chunk that has no barriers
  //if the corner was hit at EXACTLY the right angle to travel into the new
  //chunk
  vec2d old_chunk = convert_chunk_index(prev_pos);
  std::deque<chunk> * old_chunk_deque = deque_from_index(old_chunk);
  vec2d * old_chunk_deque_dim = dim_from_index(old_chunk); 
  truncate_chunk_index(old_chunk);

  vec2d new_chunk = convert_chunk_index(curr_pos);
  std::deque<chunk> * new_chunk_deque = deque_from_index(new_chunk);
  vec2d * new_chunk_deque_dim = dim_from_index(new_chunk); 
  truncate_chunk_index(new_chunk);

  //it's a valid chunk
  //check the chunk we're trying to leave
  unsigned char r = (*old_chunk_deque)[
    index(old_chunk[0], old_chunk[1], /*old_chunk_deque,*/ old_chunk_deque_dim)
  ].chunk_rebuff(curr_pos);
  //if no collision, check the chunk we're trying to enter
  if(!r) {
    unsigned char rc = (*new_chunk_deque)[
      index(new_chunk[0], new_chunk[1], /*new_chunk_deque,*/ new_chunk_deque_dim)
    ].chunk_rebuff(prev_pos);
    if(rc) {
      //bitwise logic to make the corner rebuff properly
      r = r | (~rc);
    }
  }
  if(!r) {
    //the new chunk is not blocked by barriers, or is this same chunk
    //check if it's in bounds

    //first, check to see if the "index" is even in the deque: lag spikes
    //can make entities fly so far past the bounds it will look for a chunk
    //past the end of the deque
    //next, once we're sure the chunk exists, check to see if it's "in bounds"
    if(
      (index(new_chunk[0], new_chunk[1], /*new_chunk_deque,*/ new_chunk_deque_dim) == SIZE_MAX)
    ) {
      //this is out of bounds - force a rebuff
      r = (*old_chunk_deque)[index(
          old_chunk[0],
          old_chunk[1],
          /*old_chunk_deque,*/
          old_chunk_deque_dim
        )].chunk_rebuff_forced(curr_pos);
    }
  }

  return (r);

return false;
}

bool map::check_gate(const vec2d &pos) {
  //check if there's a gate in this chunk
  if(get_chunk(convert_chunk_index(pos)).get_has_gate()) {
    //check if we're close enough to the gate to jump
    return (get_chunk(convert_chunk_index(pos)).get_midpoint().dist(pos) < 200);
  }
  return false;
}

std::string map::get_gate_dest(const vec2d &pos) {
  return(get_chunk(convert_chunk_index(pos)).get_gate_dest());
}

//when a map is initialized, spawn entities according to whatever xml rules
//exist. note that this is for spawning hard-coded entities; soft-coded ones
//such as monster closets will be managed later in the update function.
void map::spawn_initial_entities() {
  for(int i=0; i<(*dim_acti)[1] * (*dim_acti)[0]; i++) {
    //allow each chunk to spawn its entities
    (*c_deque)[i].spawn_initial_entities();
  }
}

//after the map is initialized, try to spawn reoccurring entities according to
//whatever xml rules exist. these are the soft-coded monster closet entities 
//that are spawned once the player is interacting with the map.
void map::spawn_closet_entities() {
  for(int i=0; i<(*dim_acti)[1] * (*dim_acti)[0]; i++) {
    //allow each chunk to spawn its entities
    (*c_deque)[i].spawn_closet_entities();
  }
}

void map::draw() const {
try {
  //draw the bg first
  bg.draw();

  //delegate this to each chunk's draw
  for(int j=0; j<(*dim_acti)[1]; j++) {
    for(int i=0; i<(*dim_acti)[0]; i++) {
      (*c_deque)[index(i, j, /*c_deque,*/ dim_acti)].draw();
    }
  }
  //same here
  for(int j=0; j<(*dim_inac)[1]; j++) {
    for(int i=0; i<(*dim_inac)[0]; i++) {
      (*c_deque_inac)[index(i, j, /*c_deque_inac,*/ dim_inac)].draw();
    }
  }
} catch (std::string e_msg) {
  msg::get().print_error("map::draw threw error: " + e_msg);
  throw(e_msg);
}
}

void map::validate() {
  std::ostringstream errors;

  if((*dim_acti)[0] <=0 ) {
    msg::print_warn("bad map dimension; (*dim_acti)[0] must be > 0");
    msg::print_alert("setting to 1");
    (*dim_acti)[0] = 1;
  }
  if((*dim_acti)[1] <= 0) {
    msg::print_warn("bad map dimension; (*dim_acti)[1] must be > 0");
    msg::print_alert("setting to 1");
    (*dim_acti)[1] = 1;
  }
  if(start_chunk[0] <= -1 || start_chunk[0] >= (*dim_acti)[0]) {
    msg::print_warn("bad start position; x_chunk " +
    std::to_string(start_chunk[0]) +
    " is not in map - range is 0 to " +
    std::to_string(((*dim_acti)[0] - 1)));
    msg::print_alert("setting to 0");
    start_chunk[0] = 0;
  }
  if(start_chunk[1] <= -1 || start_chunk[1] >= (*dim_acti)[1]) {
    msg::print_warn("bad start position; y_chunk " +
    std::to_string(start_chunk[1]) +
    " is not in map - range is 0 to " +
    std::to_string(((*dim_acti)[1] - 1)));
    msg::print_alert("setting to 0");
    start_chunk[1] = 0;
  }

}

