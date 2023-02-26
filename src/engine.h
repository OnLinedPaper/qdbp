#ifndef ENGINE_H_
#define ENGINE_H_

#include <stdio.h>
#include <string>
#include <iostream>

#if defined RENDER_SDL
#include <SDL2/SDL.h>
#endif

#if defined RENDER_NC
#include <linux/input.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#endif

#define CONTROLLER_DEADZONE 8000

#define LSTICK_LR 0
#define LSTICK_UD 1
#define LTRIGGER 2
#define RSTICK_LR 3
#define RSTICK_UD 4
#define RTRIGGER 5

class engine {

public:
  engine();
  engine(const engine&) = delete;
  engine& operator=(const engine&) = delete;
  ~engine();

  void play();

private:

  bool init();
  void load_media();

  void next_tick();


  int debug_swirly_int;
  char const debug_swirly();
  void incr_debug_swirly();
  bool quit, pause, debug_mode;

#if defined RENDER_SDL
  SDL_Joystick *controller;
  bool player_input(const Uint8*, SDL_Event &); 
#endif

#if defined RENDER_NC
  FILE *kbd;
  char key_map[KEY_MAX/8 +1];
  bool player_input(static char*); 
#endif
};

#endif //ENGINE_H_
