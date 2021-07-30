#include <random>
#include <ctime>

#include "engine.h"
#include "src/renders/render.h"
#include "src/xml_parser/xmlparse.h"
#include "src/viewport/viewport.h"
#include "src/timeframe/timeframe.h"
#include "src/environment/map/map_handler.h"
#include "src/utils/message.h"
#include "src/entity_handler/entity_handler.h"
#include "src/text/text_handler.h"
#include "src/movers/drawable/mortal/weapon.h"

#include <SDL2/SDL_ttf.h>
#include "src/text/text.h"

//looking for the constructors? they're below "play"


//main loop that plays the game
void engine::play() {
try{
  bool quit = false;
  bool pause = false;
  bool debug_mode = false;
  SDL_Event e;

  std::srand(std::time(0));

//- debug stuff  -    -    -    -    -    -    -    -    -    -    -    -    -        

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



  e_handler::get().create_plr("mortal/gunner/player/debug_player");
/*
  e_handler::get()
      .add_npe("hittable/debug_follower");

  e_handler::get()
      .add_npe("hittable/debug_stationary", {1500, 500}, {0, 10});

  e_handler::get()
      .add_npe("hittable/debug_stationary", {2500, 500}, {0, 125});

  e_handler::get().add_npe("killable/fishbone", {500, 2500}, {0, 0});
*/
//-    -    -    -    -    -    -    -    -    -    -    -    -    -    -    -        

  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  
  while(!quit) {

//==== PLAYER INPUT here ======================================================

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

      if(keystate[SDL_SCANCODE_SPACE] || true) { //TODO: reenable?
        static vec2d angle(0, 0);
        angle[0] = 0;
        angle[1] = 0;

        //handle shooting
        if(keystate[SDL_SCANCODE_H]) { angle[0] -= 1; }
        if(keystate[SDL_SCANCODE_L]) { angle[0] += 1; }
        if(keystate[SDL_SCANCODE_K]) { angle[1] -= 1; }
        if(keystate[SDL_SCANCODE_J]) { angle[1] += 1; }
        
        if(angle.magnitude() > 0) { e_handler::get().plr_shoot(angle); }
      }

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
      if(e.type == SDL_QUIT) { quit = true; }
      else if(e.type == SDL_KEYDOWN) {

        //pause unpause
        if(keystate[SDL_SCANCODE_P]) { pause = !pause; }
          if(pause || !pause) { //TODO: remove this / update it
            //pause menu
            if(keystate[SDL_SCANCODE_ESCAPE]) { quit = true; }
        }

        if(keystate[SDL_SCANCODE_O]) {
          //TODO: make "tryjump" in map handler
          if(map_h::get().debug_jump(e_handler::get().get_plr_pos())) {
            //jumped
            e_handler::get().teleport_plr_new_map();
          }
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
            e_handler::get().DEBUG_get_plr()->take_damage(20, -1, false);
          }
        }

      }
    }

//==== UPDATE stuff here ======================================================

    if(pause) {
    }
    else {
      e_handler::get().update_entities();
      viewport::get().set_pos(e_handler::get().get_plr_pos());

    }

//==== DISPLAY stuff here =====================================================

    //if the correct amount of time between frames has elapsed, show it
    //TODO: decide if this is actually necessary given that the program "waits"
   //between game ticks, resulting in a failure to render
    if(t_frame::get().incr_f() || true) {
      map_h::get().draw();
      e_handler::get().draw_entities();


      if(pause) { render::get().shade_display(0.7); }

//---- DRAW DEBUG STUFF here --------------------------------------------------


      t6a.set_msg(std::to_string(e_handler::get().get_plr_heat_frac()));
      t7a.set_msg(std::to_string(e_handler::get().get_plr_overheat_frac()));
      t8a.set_msg(std::to_string(e_handler::get().get_plr_is_overheat()));
      t9a.set_msg(std::to_string(e_handler::get().get_plr_is_burnout()));
      t10a.set_msg(std::to_string(e_handler::get().get_plr_health_frac()));
      t11a.set_msg(std::to_string(e_handler::get().get_plr_is_regenerating()));
      t12a.set_msg(std::to_string(e_handler::get().get_plr_shield_segs()));
      t13a.set_msg(std::to_string(e_handler::get().get_plr_shield_frac()));

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



      //l1.set_start(l1.get_start() + vec2d(0, .4));
      //l1.set_end(l1.get_end() + vec2d(0, .4));

//-----------------------------------------------------------------------------

      SDL_RenderPresent(render::get().get_r());
    }

//==== DEBUG STUFF here =======================================================

//  std::cout << e_handler::get().get_entity_count_by_name("fishbone") << std::endl;
/*
  int d_i = 1;
  if(e_handler::get().get_entity_count_by_name("fishbone") < 1) {
    e_handler::get().add_npe("killable/fishbone", {500, 2500 - 250 * d_i}, {0, 0});
  }
*/

//==== GAME TICK here =========================================================

    incr_debug_swirly();
    fprintf(stdout, "%c\r", debug_swirly());
    fflush(stdout);

    next_tick();
  }
  msg::get().close_log();
  SDL_Quit();
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


engine::engine() : debug_swirly_int(0), controller(NULL) {
try{
  //init the singletons
  //note that many require xmlparse to init themselves, so
  //xmlparse builds its trees first
  xmlparse::get().build_tree("resources/gamedata.xml");
  xmlparse::get().build_tree("resources/imagedata.xml");
  xmlparse::get().build_tree("resources/mapdata.xml");
  xmlparse::get().build_tree("resources/chunkdata.xml");


  render::get().get_r();
  viewport::get();
  image_handler::get();
  t_frame::get();
  map_h::get().set_map("/" + xmlparse::get().get_xml_string("/first_map"));
  e_handler::get();
  text_h::get();

  msg::get();
  msg::get().init_log("");

  //grab framerate data, can't do this till singletons are created
  t_frame::get().set_f_delay(xmlparse::get().get_xml_float("/msdelay"));
  t_frame::get().set_t_delay(t_frame::tickrate);

  //preload weapon data, can't do this till the tree is built
  weapon::preload_weapon_data();

  //prelaod entity ids and names, can't do this till the tree is built
  e_handler::get().preload_entity_data();

  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
    msg::print_error("couldn't init joysticks subsystem! SDL_Error: " + std::string(SDL_GetError()));
  }
  else {
    if(SDL_NumJoysticks > 0) {
      controller = SDL_JoystickOpen(0);
      if(controller == NULL) {
        msg::print_alert("didn't find a controller. (SDL_Error: " + std::string(SDL_GetError()) + ")");
      }
      else {
        msg::print_good("found a controller.");
      }
    }
  }

  if( SDL_InitSubSystem(SDL_INIT_EVERYTHING) < 0) {
    msg::print_error("Couldn't init SDL! Error: " + std::string(SDL_GetError()));
    throw("couldn't start SDL!");
  }
} catch (std::string e) { msg::print_error(e); msg::get().close_log(); return; }
}

engine::~engine() {
  msg::get().close_log();
  close_SDL();
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

void engine::close_SDL() {

  SDL_Quit();
}

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
