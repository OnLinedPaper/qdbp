#define XML_TOOL_

#include <iostream>
#include <fstream>
#include <string>

#include "xmlparse.h"

struct spawn_rule {
  int x_chunk, y_chunk, spawn_type, max_count, total_count, tick_spawn_delay, team = -1;
  float x_coord, y_coord, x_dir_component, y_dir_component, vel_frac, min_spawn_distance, max_spawn_distance = -1;
  std::string rule_name, entity_name = "";
};

struct special_chunk {
  int x_chunk, y_chunk, type, has_gate = -1;
  std::string destination, body = "";
};

int CURR_TREE = -1;
const int CHUNKTREE = 0;
const int GAMETREE = 1;
const int IMAGETREE = 2;
const int MAPTREE = 3;

const std::string CHUNKPATH = "resources/chunkdata.xml";
const std::string GAMEPATH =  "resources/gamedata.xml";
const std::string IMAGEPATH = "resources/imagedata.xml";
const std::string MAPPATH =   "resources/mapdata.xml";

std::string get_string_input(bool nonempty);
bool get_yn_input();
int get_int_input(bool nonnegative, bool nonzero, bool half_on_blank = false, int base_value = 0, bool neg_one_allowed = false);
int get_int_input_range(int lower, int upper);
float get_float_input(bool nonnegative, bool nonzero, bool neg_one_allowed = false);
float get_float_input_range(float lower, float upper, bool special_value_allowed = false, float special_value = 0);

void re_load_xml_trees();

void add_to_chunk_tree();
void add_to_game_tree();
void add_to_image_tree();
void add_to_map_tree();

void print_to_file();

int main(void) {

  int choice = 0;

  do {
    std::cout << 
      "1 - (re)load xml tree\n"
      "2 - print current tree\n"
      "3 - add to a tree\n"
      "4 - save a tree\n"

      "0 - quit\n"
    << std::endl;
    std::cin >> choice;

    if(choice == 1) {
      re_load_xml_trees();
    }
    else if(choice == 2) {
      xmlparse::get().print_tree();
    }
    else if(choice == 3) {
      switch(CURR_TREE) {
        case CHUNKTREE:
          add_to_chunk_tree(); break;
        case GAMETREE:
          add_to_game_tree(); break;
        case IMAGETREE:
          add_to_image_tree(); break;
        case MAPTREE:
          add_to_map_tree(); break;
        default:
          std::cout << "no valid tree currently loaded." << std::endl;
      }
    }
    else if(choice == 4) {
      print_to_file();
    }

  } while(choice != 0);

  return 0;
}

//=============================================================================
//get a string, and don't return it till it's full, unless otherwise specified

std::string get_string_input(bool nonempty) {
  std::string input = "";
  do {
    getline(std::cin, input);
  } while(input.empty() && nonempty);
  
  return input;
}

//=============================================================================
//get an int - if it's allowed to be blank, it'll return half the base value

int get_int_input(bool nonnegative, bool nonzero, bool half_on_blank /*= false*/, int base_value /*= 0*/, bool neg_one_allowed /*= false*/) {
  std::string input = "";
  int retval = 0; 

  do {
    getline(std::cin, input);

    if(input.empty() && half_on_blank) { return base_value / 2; }

    try {
      retval = std::stoi(input); 
      if(nonnegative && retval < 0) {
        if(!(neg_one_allowed && retval == -1)) { input = ""; }
      }
      if(nonzero && retval == 0) { input = ""; }
    } catch (std::invalid_argument) { input = ""; }
  } while (input.empty());

  return retval;
}

//=============================================================================

int get_int_input_range(int lower, int upper) {
  std::string input = "";
  int retval = 0;

  do {
    getline(std::cin, input);

    try {
      retval = std::stoi(input);
      if(retval < lower || retval > upper) { input = ""; }
    } catch (std::invalid_argument) { input = ""; }
  } while(input.empty());
  
  return retval;
}

//=============================================================================

float get_float_input(bool nonnegative, bool nonzero, bool neg_one_allowed /*= false*/) {
  std::string input = "";
  float retval = 0;

  do {
    getline(std::cin, input);

    try {
      retval = std::stof(input);
      if(nonnegative && retval < 0) {
        if(!(neg_one_allowed && retval == -1)) { input = ""; }
      }
      if(nonzero && retval == 0) { input = ""; }
    } catch (std::invalid_argument) { input = ""; }
  } while(input.empty());

  return retval;
}

//=============================================================================

float get_float_input_range(float lower, float upper, bool special_value_allowed /*= false*/, float special_value /*= 0*/) {
  std::string input = "";
  float retval = 0;

  do {
    getline(std::cin, input);

    try {
      retval = std::stof(input);
      if(retval < lower || retval > upper) {
        if(!(special_value_allowed && retval == special_value)) { input = ""; }
      }
    } catch (std::invalid_argument) { input = ""; }
  } while(input.empty());

  return retval;
}

//=============================================================================

bool get_yn_input() {
  std::string input = "";
  do {
    getline(std::cin, input);
  } while(input.compare("y") && input.compare("n"));
  return(input.compare("y") == 0);
}

//=============================================================================

void re_load_xml_trees() {
  //load one tree from existing files.
  //THIS WILL DESTROY UNSAVED CHANGED.
  int choice = 0;
  std::cout << "this will destroy any unsaved changes! continue? (1/0)" << std::endl;
  std::cin >> choice;
  if(!choice) { return; }

  std::string path = "";
  std::cout << 
    ". load which tree?\n"
    ". 1 - chunk data\n"
    ". 2 - game data\n"
    ". 3 - image data\n"
    ". 4 - map data\n"

    ". 0 - abort\n"
  << std::endl;     
  std::cin >> choice;

  switch(choice) {
    case 1:
      path = CHUNKPATH;
      CURR_TREE = CHUNKTREE;
      break;
    case 2:
      path = GAMEPATH;
      CURR_TREE = GAMETREE;
      break;
    case 3:
      path = IMAGEPATH;
      CURR_TREE = IMAGETREE;
      break;
    case 4:
      path = MAPPATH;
      CURR_TREE = MAPTREE;
      break;

    case 0:
      return;
    default:
      std::cout << "invalid choice: " << choice << std::endl;
      return;
  }

  xmlparse::get().destroy_tree();
  xmlparse::get().build_tree(path);
}

//=============================================================================

void add_to_chunk_tree() { 

}

//=============================================================================

void add_to_game_tree() { 

}

//=============================================================================
/*
  <file> nonempty string .png </file>
  <dimensions>
    <width> positive nonzero int <width>
    <height> positive nonzero int <height>

    <pivot_x> int, blank for half width </pivot_x>
    <pivot_y> int, blank for half height </pivot_y>
    <frames> positive nonzero int, blank for 1 </frames>
    <frame_delay> positive nonzero int, blank for 1 </frame_delay>
  </dimensions>
  <blend> optional: ADD, MULT, NONE </blend>
*/

void add_to_image_tree() {
  std::string input, entry_name, file_name = "";
  int width, height, pivot_x, pivot_y, frames, frame_delay, blend = 0;

  std::cout << "entry name (nonempty string): ";
  entry_name = get_string_input(true);  


  std::cout << "file name (nonempty string w/out .png): ";
  file_name = get_string_input(true);

  std::cout << "width (positive nonzero int): ";
  width = get_int_input(true, true);

  std::cout << "height (positive nonzero int): ";
  height = get_int_input(true, true);

  std::cout << "pivot_x (int, blank for half): ";
  pivot_x = get_int_input(false, false, true, width);

  std::cout << "pivot_y (int, blank for half): ";
  pivot_y = get_int_input(false, false, true, height);


  std::cout << "frame count (positive nonzero int): ";
  frames = get_int_input(true, true);

  std::cout << "frame delay (positive nonzero int): ";
  frame_delay = get_int_input(true, true);


  std::cout << "add blending? (1/0): ";
  std::cin >> blend;
  if(blend) {
    std::cout << "blend mode NONE BLEND ADD MOD (1/2/3/4) ";
    blend = get_int_input_range(1, 4);
  }

  //add to tree
  xmlparse::get().get_root()->insert_child(entry_name, "/");
  xmlparse::get().get_root()->insert_child("file", "/" + entry_name + "/", file_name);
  xmlparse::get().get_root()->insert_child("dimensions", "/" + entry_name + "/");
  xmlparse::get().get_root()->insert_child("width", "/" + entry_name + "/dimensions/", std::to_string(width));
  xmlparse::get().get_root()->insert_child("height", "/" + entry_name + "/dimensions/", std::to_string(height));
  xmlparse::get().get_root()->insert_child("pivot_x", "/" + entry_name + "/dimensions/", std::to_string(pivot_x));
  xmlparse::get().get_root()->insert_child("pivot_y", "/" + entry_name + "/dimensions/", std::to_string(pivot_y));
  xmlparse::get().get_root()->insert_child("frames", "/" + entry_name + "/dimensions/", std::to_string(frames));
  xmlparse::get().get_root()->insert_child("frame_delay", "/" + entry_name + "/dimensions/", std::to_string(frame_delay));
  if(blend) {
    std::string mode = "";
    switch(blend) {
      case 1:
        mode = "NONE"; break;
      case 2:
        mode = "BLEND"; break;
      case 3:
        mode = "ADD"; break;
      case 4:
        mode = "MOD"; break;
    }
    xmlparse::get().get_root()->insert_child("blend", "/" + entry_name + "/", mode);
  }
}

//=============================================================================

void add_to_map_tree() {
/*
<map_name>
  <dimensions>
    <x_dim> positive nonzero int </x_dim>
    <y_dim> positive nonzero int </y_dim>
  </dimensions>

  <generation>
    <style>
      "blank", "random", or "userdef" - currently not implemented in-game, use
      blank unless circumstances change
    </style>
  </generation>

  <start_chunk>
    <x_chunk> int in range 0, x_dim-1 </x_chunk>
    <y_chunk> int in range 0, y_dim-1 </y_chunk>
  </start_chunk>

  <background> path to image XML entity - currently "/debug_heart_" </background>

  <special_chunks>
    <chunk_name (nonempty string unique to this map)>
      note: the below 2 should be unique among special chunks and shouldn't clash
      <x_chunk> int in range 0, x_dim-1 </x_chunk>
      <y_chunk> int in range 0, y_dim-1 </y_chunk>

      <type> "default" or "default_impassable" - if it's impassable, should NOT 
             have anything else, like spawning or portals.
      </type>

      <jump_gate>
        OPTIONAL attribute that places a gate to another map.
        <destination> string, map name </destination>
        <body> string, an image xml entity to render for the gate </body>
      </jump_gate>
    </chunk_name>
  </special_chunks>

  <entity_spawning>
    OPTIONAL attribute that encompasses an unlimited number of entity spawning rules

    <rule_name (nonempty string unique to this map)>
      <x_chunk> int in range 0, x_dim-1 </x_chunk>
      <y_chunk> int in range 0, y_dim-1 </y_chunk>
      
      <x_coord> float in rang 1, 999, or -1 for random x </x_coord>
      <y_coord> float in rang 1, 999, or -1 for random y </y_coord>

      <x_dir_component> float representing x fraction of velocity </x_dir_component>
      <y_dir_component> float representing y fraction of velocity </y_dir_component>

      <vel_frac> float representing frac of max velocity to be used - should be between 0 and 1, 
                 -1 gives random vel, higher values may behave strangely on spawn 
      </vel_frac>

      <spawn_type> "initial" or "closet" </spawn_type>

      <max_count> positive nonzero int </max_count>
      <total_count> positive nonzero int, or -1 for infinite entities </total_count>

      <tick_spawn_delay> positive int, IGNORED by "initial" entities </tick_spawn_delay>

      <entity> string, lists a valud entity like "mortals/fishbone" </entity>

      <team> "RED", "ORANGE", "YELLOW", "GREEN", "BLUE", "PURPLE", "BROWN" </team>

      <min_spawn_distance> positive float, preferably above 500 or so </min_spawn_distance>
      <max_spawn_distance> positive float > min_spawn_distance, OR -1 for infinite distance <max_spawn_distance>
    </rule_name>
  </entity_spawning>
</map_name>
*/
  
  std::string input, map_name, background_name = "";
  int x_chunk, y_chunk, x_start_chunk, y_start_chunk, gen_style = 0;
  std::vector<spawn_rule> spawn_rules;
  std::vector<special_chunk> special_chunks;

  std::cout << "map name (nonempty string): ";
  map_name = get_string_input(true);

  std::cout << "background name (nonempty string, existing xml; example: /debug_heart_ ";
  background_name = get_string_input(true);

  std::cout << "map width (positive nonzero int): ";
  x_chunk = get_int_input(true, true);

  std::cout << "map height (positive nonzero int): ";
  y_chunk = get_int_input(true, true);

  std::cout << "starting x chunk (int in range 0, " << x_chunk-1 << "): ";
  x_start_chunk = get_int_input_range(0, x_chunk-1);

  std::cout << "starting y chunk (int in range 0, " << y_chunk-1 << "): ";
  y_start_chunk = get_int_input_range(0, y_chunk-1);


  bool another_sc = false;
  std::cout << "add a new special chunk? y/n: ";
  another_sc = get_yn_input();

  //process the addition of a special chunk here
  while(another_sc) {
    //special chunks are so far passable/impassable, and may have a gate
    //gates have a destination they go to and a body to render to screen
    int x_spec_chunk, y_spec_chunk, type_spec_chunk, has_gate = 0;    
    std::string gate_dest, gate_body = "";

    std::cout << "x chunk (int in range 0, " << x_chunk-1 << "): ";
    x_spec_chunk = get_int_input_range(0, x_chunk-1);

    std::cout << "y chunk (int in range 0, " << y_chunk-1 << "): ";
    y_spec_chunk = get_int_input_range(0, y_chunk-1);

    std::cout << "type DEFAULT/IMPASSIBLE (0/1): ";
    type_spec_chunk = get_int_input_range(0, 1);

    //if it's not impassable, ask about a gate
    if(type_spec_chunk != 1) {
      std::cout << "has gate? y/n (1/0): ";
      has_gate = get_int_input_range(0, 1);

      //if it has a gate, get info on it
      if(has_gate) {
        std::cout << "gate destination (string, name of xml map i.e. debug_1): ";
        gate_dest = get_string_input(true);

        std::cout << "gate body (string, name of xml image i.e. debug_gate): ";
        gate_body = get_string_input(true);
      }
    }

    special_chunks.push_back({
      x_spec_chunk, 
      y_spec_chunk,
      type_spec_chunk,
      has_gate,
      gate_dest,
      gate_body
    });

    std::cout << "another new special chunk? y/n: ";
    another_sc = get_yn_input();
  }



  bool another_sr = false;
  std::cout << "add a new spawn rule? y/n: ";
  another_sr = get_yn_input();
    
  //process the addition of a spawn rule here
  while(another_sr) {
    std::string sr_name, sr_entity = "";
    int sr_x_chunk, sr_y_chunk, sr_spawn_type, sr_max_count, sr_total_count, sr_tsd, sr_team = 0;
    float sr_x_coord, sr_y_coord, sr_x_dir_comp, sr_y_dir_comp, sr_vel_frac, sr_min_sd, sr_max_sd = 0;

    bool sr_valid_name = false;
    while(!sr_valid_name) {
      std::cout << "rule name (nonempty string, unique): ";
      sr_name = get_string_input(true);
      sr_valid_name = true;
      for(spawn_rule sr : spawn_rules) {
        if(sr.rule_name.compare(sr_name) == 0) {
          std::cout << "another rule with name " << sr.rule_name << " already exists" << std::endl;
          sr_valid_name = false;
          break;
        }
      }
    }


    //chunk to put rule in
    std::cout << "x chunk to put rule in (int in range 0, " << x_chunk-1 << "): ";
    sr_x_chunk = get_int_input_range(0, x_chunk-1);

    std::cout << "y chunk to put rule in (int in range 0, " << y_chunk-1 << "): ";
    sr_y_chunk = get_int_input_range(0, y_chunk-1);


    //entity to spawn
    std::cout << "entity to spawn (xml string, i.e. mortals/fishbone): ";
    sr_entity = get_string_input(true);


    //team to put it on
    std::cout << "team? RED/ORANGE/YELLOW/GREEN/BLUE/PURPLE/BROWN (1/2/3/4/5/6/7): ";
    sr_team = get_int_input_range(1, 7);


    //spawn type
    std::cout << "spawn type? INITIAL/CLOSET (1/2): ";
    sr_spawn_type = get_int_input_range(1, 2);
 
    //total count
    std::cout << "total count to spawn? (positive nonzero int): ";
    sr_total_count = get_int_input(true, true);

    //max alive at once
    std::cout << "max alive at one time? (positive nonzero int): ";
    sr_max_count = get_int_input(true, true);

    //delay between spawns
    std::cout << "tick delay between spawns? (positive int, 20 = ~1 sec): ";
    sr_tsd = get_int_input(true, false);
    

    //x and y coord
    std::cout << "x and y coord? (float in range 1, 999; -1 for random)\nx: ";
    sr_x_coord = get_float_input_range(1, 999, true, -1);
    std::cout << "y: ";
    sr_y_coord = get_float_input_range(1, 999, true, -1);


    //x and y vel comp, vel mag
    std::cout << "x and y velocity components? (float, 0 for random)\nx: ";
    sr_x_dir_comp = get_float_input(false, false);
    std::cout << "y: ";
    sr_y_dir_comp = get_float_input(false, false);

    //magnitude
    std::cout << "fraction of max speed to spawn entity at? (positive float, preferably in range 0, 1; -1 for random): ";
    sr_vel_frac = get_float_input(true, false, true);


    //spawning distance
    std::cout << "minimum distance from player required to spawn? (positive float, preferably above 500): ";
    sr_min_sd = get_float_input(true, false);
    std::cout << "maximum distance from player that still allows spawn? (positive float, or -1 for infinite distance): ";
    sr_max_sd = get_float_input(true, false, true);


    spawn_rules.push_back({
      sr_x_chunk, sr_y_chunk,
      sr_spawn_type,
      sr_max_count, sr_total_count,
      sr_tsd,
      sr_team,
      sr_x_coord, sr_y_coord,
      sr_x_dir_comp, sr_y_dir_comp, sr_vel_frac,
      sr_min_sd, sr_max_sd,
      sr_name,
      sr_entity
    });


    std::cout << "another new spawn rule? y/n: ";
    another_sr = get_yn_input();
  }

  //add to tree
  xmlparse::get().get_root->insert_child(sr_name, "/");
  xmlparse::get().get_root->insert_child("dimensions", "/" + sr_name + "/");
  xmlparse::get().get_root->insert_child("x_dim", "/" + sr_name + "/dimensions/", std::to_string(x_chunk));
  xmlparse::get().get_root->insert_child("y_dim", "/" + sr_name + "/dimensions/", std::to_string(y_chunk));
  xmlparse::get().get_root->insert_child("generation", "/" + sr_name + "/");
  xmlparse::get().get_root->insert_child("style", "/" + sr_name + "/generation/", "blank");
  xmlparse::get().get_root->insert_child("start_chunk", "/" + sr_name + "/");
  xmlparse::get().get_root->insert_child("x_chunk", "/" + sr_name + "/start_chunk/", std::to_string(x_start_chunk));
  xmlparse::get().get_root->insert_child("y_chunk", "/" + sr_name + "/start_chunk/", std::to_string(y_start_chunk));
}

//=============================================================================

void print_to_file() {
  std::string file_name = "";
  std::ofstream f;
  
  switch(CURR_TREE) {
    case CHUNKTREE:
      file_name = CHUNKPATH; break;
    case GAMETREE:
      file_name = GAMEPATH; break;
    case IMAGETREE:
      file_name = IMAGEPATH; break;
    case MAPTREE:
      file_name = MAPPATH; break;
    default:
      std::cout << "cannot print to file: no tree loaded." << std::endl;
      return;
  }

  file_name += ".NEW";

  f.open(file_name);
  if(!f.is_open()) { 
    std::cout << "couldn't open file " << file_name << " for printing! S-H-I-T!" << std::endl;
    return;
  }

  xmlparse::get().print_as_file(f);
  f.close();
}
