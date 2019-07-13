#include <chrono>

#include "engine.h"
#include "renders/texture.h"
#include "renders/render.h"
#include "movers/movable.h"
#include "movers/debug_movable.h"
#include "movers/drawable/debug_drawable.h"
#include "xml_parser/xmlparse.h"
#include "viewport/viewport.h"
#include "timeframe/timeframe.h"

//looking for the constructors? they're below "play"


//main loop that plays the game
void engine::play() {
  bool quit = false;
  SDL_Event e;


  const Uint8* keystate;

  //moves!
  d_drawable dd;
  dd.set_x(viewport::get().get_w()/2);
  dd.set_y(viewport::get().get_h()/2);

  //doesn't move!
  d_movable post;
  post.adjust_x(1920/2);
  post.adjust_y(1080/2);



  while(!quit) {

//==== PLAYER INPUT here ======================================================
    while(SDL_PollEvent(&e) != 0) {
      //get an event: protect from keybounce
      keystate = SDL_GetKeyboardState(NULL);
      if(e.type == SDL_QUIT) { quit = true; }
      else if(e.type == SDL_KEYDOWN) {
        if(keystate[SDL_SCANCODE_ESCAPE]) { quit = true; }
      }
    }

    //no keybounce protection

    if(keystate[SDL_SCANCODE_W]) { dd.move_up(); }
    if(keystate[SDL_SCANCODE_A]) { dd.move_left(); }
    if(keystate[SDL_SCANCODE_S]) { dd.move_down(); }
    if(keystate[SDL_SCANCODE_D]) { dd.move_right(); }

    if(controller) {
      //process controller input
      vec2d lrud(
        SDL_JoystickGetAxis(controller, LSTICK_LR),
        SDL_JoystickGetAxis(controller, LSTICK_UD)
      );

      if(abs(lrud[0]) > CONTROLLER_DEADZONE) {
        if(lrud[0] < 0) { dd.move_left(); }
        else { dd.move_right(); }
      }
      if(abs(lrud[1]) > CONTROLLER_DEADZONE) {
        if(lrud[1] < 0) { dd.move_up(); }
        else { dd.move_down(); }
      }
    }

//==== UPDATE stuff here ======================================================
    dd.update();
    post.update();
    viewport::get().set_pos(dd.get_pos());


//==== DISPLAY stuff here =====================================================
    SDL_SetRenderDrawColor(render::get().get_r(), 128, 128, 128, 255);
    SDL_RenderClear(render::get().get_r());
    //SDL_RenderCopy(render::get()->get_r(), t->get_texture(), NULL, NULL);s
    SDL_SetRenderDrawColor(render::get().get_r(), 28, 28, 28, 255);

    post.draw();
    dd.draw();
    SDL_RenderPresent(render::get().get_r());


//==== GAME TICK here =========================================================

    fprintf(stdout, "%c\r", debug_swirly());
    fflush(stdout);

    next_frame();
  }


}




engine::engine() : debug_swirly_int(0), controller(NULL) {
  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0) {
    fprintf(stderr, "Couldn't init joysticks subsystem! SDL_Error: %s\n", SDL_GetError());
  }
  else {
    if(SDL_NumJoysticks > 0) {
      controller = SDL_JoystickOpen(0);
      if(controller == NULL) {
        fprintf(stderr, "didn't find a controller. (SDL_Error: %s)\n", SDL_GetError());
      }
      else {
        fprintf(stderr, "found a controller.\n");
      }
    }
  }

  if( SDL_InitSubSystem(SDL_INIT_EVERYTHING) < 0) {
    throw(std::string("Couldn't init SDL! Error: ") + SDL_GetError());
  }

  //init the singletons
  xmlparse::get().build_tree("resources/gamedata.xml");
  xmlparse::get().build_tree("resources/imagedata.xml");
  render::get().get_r();
  viewport::get();
  t_frame::get();

  //grab framerate data, can't do this till singletons are created
  t_frame::get().set_delay(xmlparse::get().get_xml_double("/msdelay"));
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

  //debugging: artificial lag spike
  if(int(t_frame::get().get_f()) % 100 < 10) {
    SDL_Delay(100);
  }

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
