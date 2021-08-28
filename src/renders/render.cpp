#include "render.h"
#include "src/utils/message.h"
#include <iostream>
#include "src/viewport/viewport.h"

const uint8_t render::R_SDL = 0;
const uint8_t render::R_NCURSES = 1;

render::render() : graphics_mode(R_SDL), w(nullptr), r(nullptr), w_nc(NULL), draw_vals(NULL) {
  if( SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
    std::cout << ("Couldn't init SDL! Error: " + std::string(SDL_GetError())) << std::endl;
    
    //check if we want to run in ncurses mode instead of sdl mode
    char c = 'n';
    std::cout << "run graphics via ncurses? y/n: ";
    std::cin >> c;

    if(c == 'y') { 
      graphics_mode = R_NCURSES;
      std::cout << "yes" <<std::endl; 
    } 
    else {
      throw(std::string("Couldn't init SDL! Error: ") + SDL_GetError());
    }
  }

  //set graphics mode to run in SDL with these windows
  if(graphics_mode == R_SDL) {
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
      throw(std::string("Couldn't init images! Error: ") + SDL_GetError());
    }

    w = init_SDL_window();
    r = init_SDL_renderer();

    //get size of window and resize it - the renderer apparently resizes itself
    int wd, ht;
    wd = ht = -1;
    SDL_GetRendererOutputSize(r, &wd, &ht);
    SDL_SetWindowSize(w, wd, ht);

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
  }
  else if(graphics_mode == R_NCURSES) {
    init_ncurses_window();
  }
}

render::~render() {
  if(graphics_mode == R_SDL) {
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
  }
  else if(graphics_mode == R_NCURSES) {
    delwin(w_nc);
    endwin();
  }
}

SDL_Window *render::init_SDL_window() {
  std::string title = "qdbp (dev)";
  //TODO: read the window size

  //disable warnings for ints in boolean context, then re-enable them
  #pragma GCC diagnostic ignored "-Wint-in-bool-context"
  Uint32 flags = SDL_WINDOW_SHOWN || SDL_WINDOW_FULLSCREEN_DESKTOP;
  #pragma GCC diagnostic pop

  w = SDL_CreateWindow(title.c_str(), 0, 0, 10000, 10000, flags);
  if(w == NULL) {
    throw(std::string("Couldn't init a window! Error: ") + SDL_GetError());
  }
  return w;
}

SDL_Renderer *render::init_SDL_renderer() {

    //disable warnings for ints in boolean context, then re-enable them
  #pragma GCC diagnostic ignored "-Wint-in-bool-context"
  Uint32 flags = SDL_RENDERER_PRESENTVSYNC || SDL_RENDERER_ACCELERATED;
  #pragma GCC diagnostic pop


  SDL_Renderer *r = SDL_CreateRenderer(w, -1, flags);
  if(r == NULL) {
    throw(std::string("Couldn't init a renderer! Error: ") + SDL_GetError());
  }
  return r;
}

void render::init_ncurses_window() {
  initscr();
  cbreak();
  noecho();

  w_nc = newwin(LINES, COLS, 0, 0);
  keypad(w_nc, TRUE);
  timeout(1);
  curs_set(0);
  
  //"render" once to get the draw_vals data
  nc_render();
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

void render::nc_render() {
  //to render the window, first make a buffer for it; this will need to resize
  //gracefully in the event the screen changes size
  static uint8_t last_lines = -1;
  static uint8_t last_cols = -1;


  //check for window resizing - this will always trigger on first draw
  if(draw_vals == NULL || LINES != last_lines || COLS != last_cols) {
    free(draw_vals);
    draw_vals = (char *)malloc(sizeof(char) * LINES * COLS);
    if(draw_vals == NULL) {
      std::cerr << "malloc failed to create ncurses render buffer! S-H-I-T!" << std::endl;
      throw("malloc failed in ncurses render");
    }
    last_lines = LINES;
    last_cols = COLS;
  }

  //data will have, by now, been loaded into draw_vals
  for(int i=0; i<COLS; i++) {
    for(int j=0; j<LINES; j++) {
      mvwaddch(w_nc, j, i, draw_vals[j*sizeof(char)*COLS*i]);
    }
  }
  
  draw_blinky();

  wrefresh(w_nc);
}

void render::draw_blinky() const {
  char c = 'x';
  static uint8_t i = 0;
  switch(i++ % 4) {
    case 0: c = 'q'; break;
    case 1: c = 'd'; break;
    case 2: c = 'b'; break;
    case 3: c = 'p'; break;
  }

  mvwaddch(w_nc, LINES-1, 0, c | A_REVERSE);
}


