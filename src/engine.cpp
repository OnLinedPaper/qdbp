#include <chrono>
#include <random>

#include "engine.h"
#include "src/renders/render.h"
#include "src/xml_parser/xmlparse.h"
#include "src/viewport/viewport.h"
#include "src/timeframe/timeframe.h"
#include "src/environment/map/map_handler.h"
#include "src/utils/message.h"
#include "src/entity_handler/entity_handler.h"
#include "src/text/text_handler.h"

#include <SDL2/SDL_ttf.h>
#include "src/text/text.h"

//looking for the constructors? they're below "play"


//main loop that plays the game
void engine::play() {
try{
  bool quit = false;
  bool pause = false;
  SDL_Event e;



  static double iii = 0;
  static double avggenericms = 0;
  static double genericms = 0;
  static double avgupdatems = 0;
  static double updatems = 0;
  static double avgdrawms = 0;
  static double drawms = 0;
  static double avgrenderms = 0;
  static double renderms = 0;

  static double avgfps = 0;

  genericms = t_frame::get().get_ms();

  text t("debug info", 10, 10);
  text ta("", 280, 10);
  text t1("avg fps:", 10, 40);
  text t1a("", 280, 40);
  text t2("avg ms/tick:", 10, 70);
  text t2a("", 280, 70);
  text t3("avg ms/update:", 10, 100);
  text t3a("", 280, 100);
  text t4("avg ms/draw:", 10, 130);
  text t4a("", 280, 130);
  text t5("avg ms/render:", 10, 160);
  text t5a("", 280, 160);



  const Uint8* keystate;

  e_handler::get().create_player("debug_hittable");

  e_handler::get()
    .add_npe("debug_follower");

  while(!quit) {

//==== PLAYER INPUT here ======================================================

    //disable warnings for uninitialized use, keystate keeps throwing them
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    while(SDL_PollEvent(&e) != 0) {
      //get an event: protect from keybounce
      keystate = SDL_GetKeyboardState(NULL);
      if(e.type == SDL_QUIT) { quit = true; }
      else if(e.type == SDL_KEYDOWN) {

        //pause unpause
        if(keystate[SDL_SCANCODE_P]) { pause = !pause; }
        if(pause || !pause) { //TODO: remove this / update it
          //pause menu
          if(keystate[SDL_SCANCODE_ESCAPE]) { quit = true; }
        }
        else {

        }

      }
    }


    if(pause) {
      //pause menu
    }
    else {

      //no keybounce protection

      if(keystate[SDL_SCANCODE_J]) {
        //TODO: make "tryjump" in map handler
        if(map_h::get().debug_jump(e_handler::get().get_player_pos())) {
          //jumped
          e_handler::get().teleport_player_new_map();
        }
      }


      if(keystate[SDL_SCANCODE_W])
        { e_handler::get().move_player(e_handler::UP); }
      if(keystate[SDL_SCANCODE_A])
        { e_handler::get().move_player(e_handler::LF); }
      if(keystate[SDL_SCANCODE_S])
        { e_handler::get().move_player(e_handler::DN); }
      if(keystate[SDL_SCANCODE_D])
        { e_handler::get().move_player(e_handler::RT); }

      #pragma GCC diagnostic pop

      if(controller) {
        //process controller input
        vec2d lrud(
          SDL_JoystickGetAxis(controller, LSTICK_LR),
          SDL_JoystickGetAxis(controller, LSTICK_UD)
        );

        if(abs(lrud[0]) > CONTROLLER_DEADZONE) {
          if(lrud[0] < 0) { e_handler::get().move_player(e_handler::LF); }
          else { e_handler::get().move_player(e_handler::RT); }
        }
        if(abs(lrud[1]) > CONTROLLER_DEADZONE) {
          if(lrud[1] < 0) { e_handler::get().move_player(e_handler::UP); }
          else { e_handler::get().move_player(e_handler::DN); }
        }
      }
    }

//==== UPDATE stuff here ======================================================
updatems = t_frame::get().get_ms();
    if(pause) {
    }
    else {
      e_handler::get().update_entities();
      viewport::get().set_pos(e_handler::get().get_player_pos());
    }
updatems = t_frame::get().get_ms() - updatems;

//==== DISPLAY stuff here =====================================================

drawms = t_frame::get().get_ms();
    SDL_SetRenderDrawColor(render::get().get_r(), 32, 32, 32, 255);
    SDL_RenderClear(render::get().get_r());
    //SDL_RenderCopy(render::get()->get_r(), t->get_texture(), NULL, NULL);s
    SDL_SetRenderDrawColor(render::get().get_r(), 28, 28, 28, 255);

    map_h::get().draw();
    e_handler::get().draw_entities();

    if(pause) { render::get().shade_display(0.7); }

drawms = t_frame::get().get_ms() - drawms;
//---- DRAW DEBUG STUFF here --------------------------------------------------

    iii++;

    avggenericms = avggenericms + (1.0/iii) * ((t_frame::get().get_ms() - genericms) - avggenericms);

    avgfps = avgfps + (1.0/iii) * ((1000 / (t_frame::get().get_ms() - genericms)) - avgfps);

    avgupdatems = avgupdatems + (1.0/iii) * (updatems - avgupdatems);
    avgdrawms = avgdrawms + (1.0/iii) * (drawms - avgdrawms);
    avgrenderms = avgrenderms + (1.0/iii) * (renderms - avgrenderms);

    ta.set_msg(std::string(1, debug_swirly()));
    t1a.set_msg(std::to_string(avgfps));
    t2a.set_msg(std::to_string(avggenericms));
    t3a.set_msg(std::to_string(avgupdatems));
    t4a.set_msg(std::to_string(avgdrawms));
    t5a.set_msg(std::to_string(avgrenderms));

    t.draw();
    ta.draw();
    t1.draw();
    t1a.draw();
    t2.draw();
    t2a.draw();
    t3.draw();
    t3a.draw();
    t4.draw();
    t4a.draw();
    t5.draw();
    t5a.draw();

    genericms = t_frame::get().get_ms();

//-----------------------------------------------------------------------------
renderms = t_frame::get().get_ms();

    SDL_RenderPresent(render::get().get_r());

renderms = t_frame::get().get_ms() - renderms;
//==== DEBUG STUFF here =======================================================

//dd.move_up();
//dd.move_lf();
//dd.move_dn();
//dd.move_rt();

//==== GAME TICK here =========================================================

    incr_debug_swirly();
    fprintf(stdout, "%c\r", debug_swirly());
    fflush(stdout);

    next_frame();
  }
  SDL_Quit();
}
catch(std::string e) { msg::print_error(e); throw; }
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
#############################     ##   ###    #################################
##############################     #   ##     #################################
###############################        #     ##################################
################################            ###################################
#################################          ####################################
##################################        #####################################
###################################      ######################################
####################################   ########################################
###############################################################################
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


  map_h::get().set_map("/" + xmlparse::get().get_xml_string("/first_map"));
  render::get().get_r();
  viewport::get();
  t_frame::get();
  text_h::get();

  //grab framerate data, can't do this till singletons are created
  t_frame::get().set_delay(xmlparse::get().get_xml_float("/msdelay"));


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
} catch (std::string e) { msg::print_error(e); throw; }
}

engine::~engine() {
  SDL_JoystickClose(controller);
  close_SDL();
}

void engine::next_frame() {

  //get the desired delay between frames
  float f_delay = t_frame::get().get_delay();
  //get the actual delay
  float elapsed = t_frame::get().get_elapsed();

  //delay some ms
  if(f_delay - elapsed > 0){
    //took some time to compute
    if(f_delay - elapsed <= f_delay) {
      //took less than delay to compute
      SDL_Delay(f_delay - elapsed);
    }
    else {
      //took "no time" - usually happens on startup. delay max time
      SDL_Delay(f_delay);
    }
  }
  else {
    //took a longtime to compute this round - no delay
    //SDL_Delay(0);
  }

  //get the next millisecond
  elapsed = t_frame::get().get_ms();

  //go to next frame
  t_frame::get().incr_f();

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
