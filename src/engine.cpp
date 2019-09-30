#include <chrono>
#include <random>

#include "engine.h"
#include "renders/render.h"
#include "movers/movable.h"
#include "movers/drawable/debug_drawable.h"
#include "xml_parser/xmlparse.h"
#include "viewport/viewport.h"
#include "timeframe/timeframe.h"
#include "environment/chunk/chunk.h"
#include "environment/map/map.h"
#include "environment/map/map_handler.h"
#include "utils/message.h"
#include "src/rect2d/rect2d.h"
#include "src/movers/drawable/hittable/debug_hittable.h"
#include "src/rect2d/hitbox/hitline.h"

//looking for the constructors? they're below "play"


//main loop that plays the game
void engine::play() {
  bool quit = false;
  SDL_Event e;


  const Uint8* keystate;

  //TODO: entity handler
  d_hittable dd("/movers/hittable/debug_hittable");
  dd.set_pos(map_h::get().get_start_pos());


  while(!quit) {

//==== PLAYER INPUT here ======================================================

    //disable warnings for uninitialized use, keystate keeps throwing them
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    while(SDL_PollEvent(&e) != 0) {
      //get an event: protect from keybounce
      keystate = SDL_GetKeyboardState(NULL);
      if(e.type == SDL_QUIT) { quit = true; }
      else if(e.type == SDL_KEYDOWN) {
        if(keystate[SDL_SCANCODE_ESCAPE]) { quit = true; }
        if(keystate[SDL_SCANCODE_J]) {
          if(map_h::get().debug_jump(dd.get_pos())) {
            //jumped
            dd.stop();
            dd.set_pos(map_h::get().get_start_pos());
          }
        }
      }
    }

    //no keybounce protection

    if(keystate[SDL_SCANCODE_W]) { dd.move_up(); }
    if(keystate[SDL_SCANCODE_A]) { dd.move_lf(); }
    if(keystate[SDL_SCANCODE_S]) { dd.move_dn(); }
    if(keystate[SDL_SCANCODE_D]) { dd.move_rt(); }

    #pragma GCC diagnostic pop

    if(controller) {
      //process controller input
      vec2d lrud(
        SDL_JoystickGetAxis(controller, LSTICK_LR),
        SDL_JoystickGetAxis(controller, LSTICK_UD)
      );

      if(abs(lrud[0]) > CONTROLLER_DEADZONE) {
        if(lrud[0] < 0) { dd.move_lf(); }
        else { dd.move_rt(); }
      }
      if(abs(lrud[1]) > CONTROLLER_DEADZONE) {
        if(lrud[1] < 0) { dd.move_up(); }
        else { dd.move_dn(); }
      }
    }

//==== UPDATE stuff here ======================================================
    dd.update();
    viewport::get().set_pos(dd.get_pos());


//==== DISPLAY stuff here =====================================================
    SDL_SetRenderDrawColor(render::get().get_r(), 32, 32, 32, 255);
    SDL_RenderClear(render::get().get_r());
    //SDL_RenderCopy(render::get()->get_r(), t->get_texture(), NULL, NULL);s
    SDL_SetRenderDrawColor(render::get().get_r(), 28, 28, 28, 255);

    map_h::get().draw();
    dd.draw();
    SDL_RenderPresent(render::get().get_r());

//==== DEBUG STUFF here =======================================================

//dd.move_up();
//dd.move_lf();
//dd.move_dn();
//dd.move_rt();

//==== GAME TICK here =========================================================

    fprintf(stdout, "%c\r", debug_swirly());
    fflush(stdout);

    next_frame();
  }
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

  //grab framerate data, can't do this till singletons are created
  t_frame::get().set_delay(xmlparse::get().get_xml_double("/msdelay"));


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
}

engine::~engine() {
  SDL_JoystickClose(controller);
  close_SDL();
}

void engine::next_frame() {

  //get the desired delay between frames
  double f_delay = t_frame::get().get_delay();
  //get the actual delay
  double elapsed = t_frame::get().get_elapsed();

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

char engine::debug_swirly() {

  debug_swirly_int = (debug_swirly_int+1) % 4;
  char c = '+';

  switch(debug_swirly_int) {
    case 0: c = 'p'; break;
    case 1: c = 'q'; break;
    case 2: c = 'd'; break;
    case 3: c = 'b'; break;
  }
  return c;
}
