#include "engine.h"
#include "texture.h"
#include "render.h"
#include "movable.h"
#include "xml_parser/xmlparse.h"

//looking for the constructors? they're below "play"


//main loop that plays the game
void engine::play() {
  bool quit = false;
  SDL_Event e;


    const Uint8* keystate;

  texture *t = new texture("./file.png");
  movable *m = new movable();



  while(!quit) {

    while(SDL_PollEvent(&e) != 0) {
      //get an event
      keystate = SDL_GetKeyboardState(NULL);
      if(e.type == SDL_QUIT) {
        quit = true;
      }
      else if(e.type == SDL_KEYDOWN) {
        if(keystate[SDL_SCANCODE_ESCAPE]) {
          quit = true;
        }

      }
    }

    if(keystate[SDL_SCANCODE_W]) { m->adjust_y(-4); }
    if(keystate[SDL_SCANCODE_A]) { m->adjust_x(-4); }
    if(keystate[SDL_SCANCODE_S]) { m->adjust_y(4); }
    if(keystate[SDL_SCANCODE_D]) { m->adjust_x(4); }

    if(controller) {
      //process controller input

    }

    fprintf(stdout, "%c\r", debug_swirly());
    fflush(stdout);

    //TODO; display stuff here
    SDL_SetRenderDrawColor(render::get().get_renderer(), 128, 128, 128, 255);
    SDL_RenderClear(render::get().get_renderer());
    //SDL_RenderCopy(render::get()->get_renderer(), t->get_texture(), NULL, NULL);s
    SDL_SetRenderDrawColor(render::get().get_renderer(), 28, 28, 28, 255);

    m->draw();
    SDL_RenderPresent(render::get().get_renderer());

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
        fprintf(stderr, "Couldn't init a controller! SDL_Error: %s\n", SDL_GetError());
      }
    }
  }


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
