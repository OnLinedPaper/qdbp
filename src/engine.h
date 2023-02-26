#ifndef ENGINE_H_
#define ENGINE_H_

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <linux/input.h>

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
  void close_SDL();

  void next_tick();

  int debug_swirly_int;
  char const debug_swirly();
  void incr_debug_swirly();
  SDL_Joystick *controller;

  void player_input();
  void player_input_2();

  SDL_Event e;
  const Uint8* keystate;


  FILE *nc_kbd;
  char key_map[KEY_MAX/8 +1];

  bool quit;
  bool pause;
  bool debug_mode;


};

#endif //ENGINE_H_
