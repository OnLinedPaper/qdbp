#include "render.h"
#include "src/utils/message.h"
#include <iostream>
#include "src/viewport/viewport.h"

#if defined RENDER_NC
#include "render_nc.cpp"
#endif

#if defined RENDER_SDL
render::render() : w(nullptr), r(nullptr) {
  if( SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    std::string e_msg = std::string("Couldn't init SDL! Error: ") + SDL_GetError();
    msg::get().print_error("render::render threw error: " + e_msg);
    throw(e_msg);
  }

  if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::string e_msg = std::string("Couldn't init images! Error: ") + SDL_GetError();
    msg::get().print_error("render::render threw error: " + e_msg);
    throw(e_msg);
  }

  w = init_window();
  r = init_renderer();

  //get size of window and resize it - the renderer apparently resizes itself
  int wd, ht;
  wd = ht = -1;
  SDL_GetRendererOutputSize(r, &wd, &ht);
  SDL_SetWindowSize(w, wd, ht);

  SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
}

render::~render() {
  SDL_DestroyRenderer(r);
  SDL_DestroyWindow(w);
}

SDL_Window *render::init_window() {
  std::string title = "qdbp (dev)";
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);

  //disable warnings for ints in boolean context, then re-enable them
  #pragma GCC diagnostic ignored "-Wint-in-bool-context"
  Uint32 flags = SDL_WINDOW_SHOWN || SDL_WINDOW_FULLSCREEN;
  #pragma GCC diagnostic pop

  w = SDL_CreateWindow(title.c_str(), 0, 0, DM.w, DM.h, flags);
  if(w == NULL) {
    std::string e_msg = std::string("Couldn't init a window! Error: ") + SDL_GetError();
    msg::get().print_error("render::init_window threw error: " + e_msg);
    throw(e_msg);
  }
  return w;
}

SDL_Renderer *render::init_renderer() {

    //disable warnings for ints in boolean context, then re-enable them
  #pragma GCC diagnostic ignored "-Wint-in-bool-context"
  Uint32 flags = SDL_RENDERER_ACCELERATED;
  #pragma GCC diagnostic pop

  SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

  SDL_Renderer *r = SDL_CreateRenderer(w, -1, flags);
  if(r == NULL) {
    std::string e_msg = std::string("Couldn't init a renderer! Error: ") + SDL_GetError();
    msg::get().print_error("render::init_render threw error: " + e_msg);
    throw(e_msg);
  }
  return r;
}

void render::shade_display(float shade) {
  static SDL_Rect r{
    0,
    0,
    viewport::get().get_w(),
    viewport::get().get_h()
  };

  SDL_SetRenderDrawColor(
    render::get().get_r(), 0, 0, 0, 255*shade
  );
  SDL_RenderFillRect(render::get().get_r(), &r);
}
#endif
