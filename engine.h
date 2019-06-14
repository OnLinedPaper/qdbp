#ifndef ENGINE_H_
#define ENGINE_H_

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <iostream>

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

  int debug_swirly_int;
  char debug_swirly();
  SDL_Joystick *controller;
};

#endif //ENGINE_H_
