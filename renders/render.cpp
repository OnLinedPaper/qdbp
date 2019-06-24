#include "render.h"

render::render() : w(nullptr), r(nullptr) {
  if( SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    throw(std::string("Couldn't init SDL! Error: ") + SDL_GetError());
  }

  if(!IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) {
    throw(std::string("Couldn't init images! Error: ") + SDL_GetError());
  }

  w = init_window();
  r = init_renderer();
}

render::~render() {
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
  SDL_Quit();
}

SDL_Window *render::init_window() {
  std::string title = "TODO: new title";
  //TODO: read the window size
  w = SDL_CreateWindow(title.c_str(), 0, 0, 1920, 1080, SDL_WINDOW_SHOWN || SDL_WINDOW_FULLSCREEN_DESKTOP);
  if(w == NULL) {
    throw(std::string("Couldn't init a window! Error: ") + SDL_GetError());
  }
  return w;
}

SDL_Renderer *render::init_renderer() {
  SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_PRESENTVSYNC || SDL_RENDERER_ACCELERATED);
  if(r == NULL) {
    throw(std::string("Couldn't init a renderer! Error: ") + SDL_GetError());
  }
  return r;
}
