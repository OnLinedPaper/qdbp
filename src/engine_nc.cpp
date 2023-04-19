//this file exists as a separate entity dsigned to be included if a macro is set at compilation time to disable SDL rendering. it uses ncurses rendering instead.

#if defined RENDER_NC
#include <ctime>
#include <unistd.h>
#include <ncurses.h>
#include <linux/input.h>

#include "engine.h"
#include "src/renders/render_nc.h"
#include "src/xml_parser/xmlparse.h"
#include "src/viewport/viewport.h"
#include "src/timeframe/timeframe.h"
#include "src/environment/map/map_handler.h"
#include "src/utils/message.h"
#include "src/entity_handler/entity_handler.h"
#include "src/text/text_handler.h"
#include "src/movers/drawable/mortal/weapon.h"
#include "src/hud/hud.h"
#include "src/utils/rng.h"



engine::engine() : 
    debug_swirly_int(0)
  , quit(false)
  , pause(false)
  , debug_mode(false)
{ 
try {
  msg::get();
  msg::get().init_log("");
  msg::get().log("loading xml data...");

  //init the singletons
  //note that many require xmlparse to init themselves, so
  //xmlparse builds its trees first
  msg::get().log("- loading gamedata...");
  xmlparse::get().build_tree("resources/gamedata.xml");
  msg::get().log("- loading imagedata...");
  xmlparse::get().build_tree("resources/imagedata.xml");
  msg::get().log("- loading mapdata...");
  xmlparse::get().build_tree("resources/mapdata.xml");
  msg::get().log("- loading chunkdata...");
  xmlparse::get().build_tree("resources/chunkdata.xml");
  msg::get().log("xml loaded.");


  msg::get().log("initializing singletons...");
  msg::get().log("- initializing rng...");
  rng::get().seed(0); //TODO: use std::time(NULL) once done debugging
  msg::get().log("- initializing render...");
  render::get();
  msg::get().log("- initializing viewport...");
  viewport::get();
  msg::get().log("- initializing timeframe...");
  t_frame::get();
  msg::get().log("- initializing entity handler...");
  e_handler::get();
  msg::get().log("- SKIPPING text handler...");
  //text_h::get();
  msg::get().log("- initializing hud...");
  hud::get();
  msg::get().log("singletons initialized.");

  //grab framerate data, can't do this till singletons are created
  msg::get().log("loading framerate data...");
  t_frame::get().set_f_delay(xmlparse::get().get_xml_float("/msdelay"));
  t_frame::get().set_t_delay(t_frame::tickrate);
  msg::get().log("framerate data loaded.");

  //preload weapon data, can't do this till the tree is built
  msg::get().log("loading weapon data...");
  weapon::preload_weapon_data();
  msg::get().log("weapon data loaded.");

  //create the first map and load the player into it
  msg::get().log("loading first map...");
  map_h::get().init_map("/" + xmlparse::get().get_xml_string("/first_map"));
  msg::get().log("first map loaded.");


} catch (std::string e) { msg::print_error(e); msg::get().close_log(); return; }
}

engine::~engine() { 
  msg::get().close_log();
}

//TODO: usleep is deprecated, move to nanosleep later
void engine::next_tick() { 
  //GAME IS SET TO 20 TICKS PER SECOND

  //get the desired delay between frames
  float t_delay = t_frame::get().get_t_delay();
  //get the actual delay
  float elapsed = t_frame::get().get_elapsed_ms();

  //delay some ms
  if(t_delay - elapsed > 0){
    //took some time to compute
    if(t_delay - elapsed <= t_delay) {
      //took less than delay to compute
      usleep((t_delay - elapsed) * 1000);
    }
    else {
      //took "no time" - usually happens on startup. delay max time
      usleep(t_delay * 1000);
    }
  }
  else {
    //took a longtime to compute this round - no delay
    //SDL_Delay(0);
  }

  //go to next tick
  t_frame::get().incr_t();


}

//=============================================================================

void engine::player_input() { 

  //i have no FUCKING idea why but if i put the file pointer into a class
  //member variable it immediately causes stack smashing
  //this is annoying because i have to rely on it closing automatically when it
  //goes out of scope, which doesn't happen until the program terminates. SIGH.
  static FILE *kbd = fopen(
    xmlparse::get().get_xml_string("/ncurses_rendering/keyboard_input_device").c_str(),
    "r"
  );
  
  if(kbd == NULL) {
    if(errno == 2) {
      std::cout << "keyboard doesn't exist" << std::endl;
    }
    if(errno == 13) {
      std::cout << "insufficient permissions" << std::endl;
    }
    throw("S-H-I-T!");
  }


  //TODO: go grab the documentation from the ncurses_rendering branch
  static char key_map[KEY_MAX/8 +1];
 
//-----------------------------------------------------------------------------
//actual input logic here

  //consume characters so they don't clutter input
  while(getch() != ERR) { }

  //wipe char array
  memset(key_map, 0, sizeof(key_map));

  //fill it with keyboard data
  ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map);

  //process the keystrokes here
  if(pause) { }
  else {
    //no keybounce protection in this section

    //handle movement
    if(key_map[KEY_W/8] & (1 << (KEY_W % 8)))
      { e_handler::get().move_plr(e_handler::UP); }
    if(key_map[KEY_A/8] & (1 << (KEY_A % 8)))
      { e_handler::get().move_plr(e_handler::LF); }
    if(key_map[KEY_S/8] & (1 << (KEY_S % 8)))
      { e_handler::get().move_plr(e_handler::DN); }
    if(key_map[KEY_D/8] & (1 << (KEY_D % 8)))
      { e_handler::get().move_plr(e_handler::RT); }

    if(key_map[KEY_LEFTSHIFT/8] & (1 << (KEY_LEFTSHIFT % 8)))
      { e_handler::get().boost_plr(true); }
    else { e_handler::get().boost_plr(false); }

    //handle shooting
    static vec2d angle(0, 0);
    angle[0] = 0;
    angle[1] = 0;

    if(key_map[KEY_H/8] & (1 << (KEY_H % 8))) { angle[0] -= 1; }
    if(key_map[KEY_L/8] & (1 << (KEY_L % 8))) { angle[0] += 1; }
    if(key_map[KEY_K/8] & (1 << (KEY_K % 8))) { angle[1] -= 1; }
    if(key_map[KEY_J/8] & (1 << (KEY_J % 8))) { angle[1] += 1; }

    if(angle.magnitude() > 0) { e_handler::get().plr_shoot(angle); }


    //keybounce protection from this point on
    //quit
    static bool q_keydown = false;
    if(key_map[KEY_ESC/8] & (1 << (KEY_ESC % 8))) {
      if(!q_keydown) {
        q_keydown = true;
        quit = true;
      }
    } else { q_keydown = false; }

    //jump through a gate
    static bool o_keydown = false;
    if(key_map[KEY_O/8] & (1 << (KEY_O %8))) {
      if(!o_keydown) {
        o_keydown = true;
        map_h::get().try_jump();
      }
    } else { o_keydown = false; }
  }

  return;
}


#endif
