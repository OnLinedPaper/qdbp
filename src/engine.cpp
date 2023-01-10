#include "engine.h"

#if defined RENDER_NC
#include "engine_nc.cpp"
#endif

#include <ctime>

#include "src/renders/render.h"
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
#include "src/text/text.h"

#if defined RENDER_SDL
#include <SDL2/SDL_ttf.h>
#endif

//looking for the constructors? they're below "play"


//main loop that plays the game
void engine::play() {
try{
//- debug stuff  -    -    -    -    -    -    -    -    -    -    -    -    -        

#if defined RENDER_SDL
  //SDL DEBUGGING
  text t1("pos:", 10, 30);
  text t1a("", 280, 30);
  text t6("heat:", 10, 30);
  text t6a("", 280, 30);
  text t7("overheat:", 10, 60);
  text t7a("", 280, 60);
  text t8("overheated?", 10, 90);
  text t8a("", 280, 90);
  text t9("burnt out?", 10, 120);
  text t9a("", 280, 120);
  text t10("health:", 10, 150);
  text t10a("", 280, 150);
  text t11("regenerating?", 10, 180);
  text t11a("", 280, 180);
  text t12("shields:", 10, 210);
  text t12a("", 280, 210);
  text t13("shield %:", 10, 240);
  text t13a("", 280, 240);
  text t14("venting?", 10, 270);
  text t14a("", 280, 270);
#endif

#if defined RENDER_NC
  //NCURSES DEBUGGING
#endif


  e_handler::get()
      .add_npe("mortal/mortals/debug_follower");

//-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -        

  while(!quit) {

//==== PLAYER INPUT here ======================================================

  quit = player_input();

//==== UPDATE stuff here ======================================================

    if(pause) { }
    else {
      map_h::get().update();
      e_handler::get().update_entities();
      viewport::get().set_pos(e_handler::get().get_plr_pos());
      hud::get().update();
    }

//==== DISPLAY stuff here =====================================================

    //if the correct amount of time between frames has elapsed, show it
    //TODO: decide if this is actually necessary given that the program "waits"
    //between game ticks, resulting in a failure to render
    if(t_frame::get().incr_f() || true) {
      map_h::get().draw();
      e_handler::get().draw_entities();
      hud::get().draw();


      //if(pause) { render::get().shade_display(0.7); }

//---- DRAW DEBUG STUFF here --------------------------------------------------

#if defined RENDER_SDL
      //SDL DEBUGGING
      t1a.set_msg(e_handler::get().get_plr_pos().to_string());
/*
      t6a.set_msg(std::to_string(e_handler::get().get_plr_heat_frac()));
      t7a.set_msg(std::to_string(e_handler::get().get_plr_overheat_frac()));
      t8a.set_msg(std::to_string(e_handler::get().get_plr_is_overheat()));
      t9a.set_msg(std::to_string(e_handler::get().get_plr_is_burnout()));
      t10a.set_msg(std::to_string(e_handler::get().get_plr_health_frac()));
      t11a.set_msg(std::to_string(e_handler::get().get_plr_is_regenerating()));
      t12a.set_msg(std::to_string(e_handler::get().get_plr_shields()));
      t13a.set_msg(std::to_string(e_handler::get().get_plr_shield_frac()));
      t14a.set_msg(std::to_string(e_handler::get().get_plr_is_vent()));

      t6.draw();
      t6a.draw();
      t7.draw();
      t7a.draw();
      t8.draw();
      t8a.draw();
      t9.draw();
      t9a.draw();
      t10.draw();
      t10a.draw();
      t11.draw();
      t11a.draw();
      t12.draw();
      t12a.draw();
      t13.draw();
      t13a.draw();
      t14.draw();
      t14a.draw();
*/
      t1.draw();
      t1a.draw();
#endif

#if defined RENDER_NC
      //NCURSES DEBUGGING
#endif
    
//-----------------------------------------------------------------------------

#if defined RENDER_SDL
      SDL_RenderPresent(render::get().get_r());
#endif

#if defined RENDER_NC
      //TODO: nc rendering
#endif
    }

//==== DEBUG STUFF here =======================================================


//==== GAME TICK here =========================================================

    incr_debug_swirly();
    fprintf(stdout, "%c\r", debug_swirly());
    fflush(stdout);

    next_tick();
  }
  msg::get().close_log();
}
catch(std::string e) { msg::print_error(e); msg::get().close_log(); return; }
}

/*#############################################################################
###############################################################################
#############################                 #################################
#############################   OTHER STUFF   #################################
#############################                 #################################
###############################################################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
####################################   ########################################
##########################    ######   #######    #############################
##########################     #####   ######     #############################
###########################     ####   #####     ##############################
############################     ###   ####     ###############################
#############################     ##   ###     ################################
##############################     #   ##     #################################
###############################        #     ##################################
################################            ###################################
#################################          ####################################
##################################        #####################################
###################################      ######################################
####################################    #######################################
#####################################  ########################################
#############################################################################*/


#if defined RENDER_SDL
engine::engine() : 
    debug_swirly_int(0)
  , controller(NULL) 
  , quit(false)
  , pause(false)
  , debug_mode(false)
{
try{
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
  render::get().get_r();
  msg::get().log("- initializing viewport...");
  viewport::get();
  msg::get().log("- initializing image handler...");
  image_handler::get();
  msg::get().log("- initializing timeframe...");
  t_frame::get();
  msg::get().log("- initializing entity handler...");
  e_handler::get();
  msg::get().log("- initializing text handler...");
  text_h::get();
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

  msg::get().log("searching for joysticks...");
  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
    msg::print_error("couldn't init joysticks subsystem! SDL_Error: " + std::string(SDL_GetError()));
  }
  else {
    if(SDL_NumJoysticks() > 0) {
      controller = SDL_JoystickOpen(0);
      if(controller == NULL) {
        msg::print_alert("didn't find a controller. (SDL_Error: " + std::string(SDL_GetError()) + ")");
      }
      else {
        msg::print_good("found a controller.");
      }
    }
  }

  msg::get().log("initializing SDL...");
  if( SDL_InitSubSystem(SDL_INIT_EVERYTHING) < 0) {
    msg::print_error("Couldn't init SDL! Error: " + std::string(SDL_GetError()));
    throw("couldn't start SDL!");
  }
  msg::get().log("SDL initialized.");
  msg::get().log("---- engine loaded successfully! ----");
  
} catch (std::string e) { msg::print_error(e); msg::get().close_log(); return; }
}

engine::~engine() {
  msg::get().close_log();
  SDL_Quit();
}

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
      SDL_Delay(t_delay - elapsed);
    }
    else {
      //took "no time" - usually happens on startup. delay max time
      SDL_Delay(t_delay);
    }
  }
  else {
    //took a longtime to compute this round - no delay
    //SDL_Delay(0);
  }

  //go to next tick
  t_frame::get().incr_t();

}

bool engine::player_input() {
  const static Uint8* keystate = SDL_GetKeyboardState(NULL);
  SDL_Event e;

    //get the keystate
    SDL_PumpEvents();

    if(pause) { /*pause menu*/ }
    else {
      //no keybounce protection

      //handle movement
      if(keystate[SDL_SCANCODE_W])
        { e_handler::get().move_plr(e_handler::UP); }
      if(keystate[SDL_SCANCODE_A])
        { e_handler::get().move_plr(e_handler::LF); }
      if(keystate[SDL_SCANCODE_S])
        { e_handler::get().move_plr(e_handler::DN); }
      if(keystate[SDL_SCANCODE_D])
        { e_handler::get().move_plr(e_handler::RT); }

      if(keystate[SDL_SCANCODE_LSHIFT])
        { e_handler::get().boost_plr(true); }
      else { e_handler::get().boost_plr(false); }

      //handle shooting
      static vec2d angle(0, 0);
      angle[0] = 0;
      angle[1] = 0;

      //handle shooting
      if(keystate[SDL_SCANCODE_H]) { angle[0] -= 1; }
      if(keystate[SDL_SCANCODE_L]) { angle[0] += 1; }
      if(keystate[SDL_SCANCODE_K]) { angle[1] -= 1; }
      if(keystate[SDL_SCANCODE_J]) { angle[1] += 1; }
        
      if(angle.magnitude() > 0) { e_handler::get().plr_shoot(angle); }
      


      if(controller) {
        //process controller input
        vec2d lrud(
          SDL_JoystickGetAxis(controller, LSTICK_LR),
          SDL_JoystickGetAxis(controller, LSTICK_UD)
        );

        if(abs(lrud[0]) > CONTROLLER_DEADZONE) {
          if(lrud[0] < 0) { e_handler::get().move_plr(e_handler::LF); }
          else { e_handler::get().move_plr(e_handler::RT); }
        }
        if(abs(lrud[1]) > CONTROLLER_DEADZONE) {
          if(lrud[1] < 0) { e_handler::get().move_plr(e_handler::UP); }
          else { e_handler::get().move_plr(e_handler::DN); }
        }
      }
    }


    while(SDL_PollEvent(&e) != 0) {
      //get an event: protect from keybounce
      if(e.type == SDL_QUIT || keystate[SDL_SCANCODE_ESCAPE]) { return true; }
      else if(e.type == SDL_KEYDOWN) {

        //pause unpause
        if(keystate[SDL_SCANCODE_P]) { pause = !pause; }

        if(keystate[SDL_SCANCODE_O]) {
          map_h::get().try_jump();
        }

        if(keystate[SDL_SCANCODE_V]) {
          //toggle venting
          e_handler::get().toggle_plr_vent();
        }

        if(keystate[SDL_SCANCODE_R]) {
          e_handler::get().toggle_plr_regen();
        }

        //draw debug things
        if(keystate[SDL_SCANCODE_COMMA]) {
          e_handler::get().set_draw_debug_info(
            !e_handler::get().get_draw_debug_info()
          );
          
          debug_mode = !debug_mode;
        }

 
        //debugging section
        if(debug_mode) {
          if(keystate[SDL_SCANCODE_X]) {
            e_handler::get().DEBUG_get_plr()->take_damage(1, -1, false);
          }
        }

      }
    }


  return false;
}
#endif

void engine::incr_debug_swirly() {
    debug_swirly_int = (debug_swirly_int+1) % 4;
}

char const engine::debug_swirly() {
  char c = '+';

  switch(debug_swirly_int) {
    case 0: c = 'p'; break;
    case 1: c = 'q'; break;
    case 2: c = 'd'; break;
    case 3: c = 'b'; break;
  }
  return c;
}
