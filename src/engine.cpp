#include "engine.h"
#include "renders/texture.h"
#include "renders/render.h"
#include "movers/movable.h"
#include "movers/debug_movable.h"
#include "movers/drawable/debug_drawable.h"
#include "xml_parser/xmlparse.h"

//looking for the constructors? they're below "play"


//main loop that plays the game
void engine::play() {
  bool quit = false;
  SDL_Event e;


  const Uint8* keystate;

  //moves!
  d_drawable dd;
  dd.set_x(1920/2);
  dd.set_y(1080/2);

  //doesn't move!
  d_movable post;
  post.adjust_x(1920/4);
  post.adjust_y(1080/4);



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


//==== DISPLAY stuff here =====================================================
    SDL_SetRenderDrawColor(render::get().get_renderer(), 128, 128, 128, 255);
    SDL_RenderClear(render::get().get_renderer());
    //SDL_RenderCopy(render::get()->get_renderer(), t->get_texture(), NULL, NULL);s
    SDL_SetRenderDrawColor(render::get().get_renderer(), 28, 28, 28, 255);

    post.draw();
    dd.draw();
    SDL_RenderPresent(render::get().get_renderer());


//==== GAME TICK here =========================================================

    fprintf(stdout, "%c\r", debug_swirly());
    fflush(stdout);

    SDL_Delay(20);
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

  xmlparse::get().build_tree("resources/gamedata.xml");

}

engine::~engine() {
  SDL_JoystickClose(controller);
  close_SDL();
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
