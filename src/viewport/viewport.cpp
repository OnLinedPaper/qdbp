#include "viewport.h"
#include "src/renders/render.h"
#include <iostream>

viewport::viewport() {
  view_width = view_height = 0;
  pos[0] = pos[1] = 0;
  nc_pix_dims[0] = nc_pix_dims[1] = 0;
  nc_world_dims[0] = nc_world_dims[1] = 0;
  //TODO: constructor list optimization

  if(render::get().mode() == render::R_SDL) {
    SDL_GetWindowSize(render::get().get_w(), &view_width, &view_height);
  }
  else if(render::get().mode() == render::R_NCURSES) {
    //need to determine "screen size" on crappy, and on frankie, as well
    //should in THEORY be similar... the original world has an aspect ratio of
    //1920x1080, and can fit about two chunks, each of which are 1000x1000. i'm
    //going to use that as the "scale" here and attempt to scale the screen to 
    //two chunks wide, i.e. if the COLS value is 80, each chunk takes up 40. 
    //additionally, i will assume the standard box is... (squints...) 
    //8 pixels wide and 16 pixels tall. 
    //the game will hopefully scale itself appropriately from there.
    //UNFORTUNATELY, this approach will lock the screen size, but perhaps this
    //is for the best, as if the terminal gets too small, it's going to lose 
    //the ability to render things meaningfully anyway.

    //actually, fuck it, i'm just gonna ask.
    std::cout << "what's the aspect ratio of your terminal's font? (crappy is 8 16)\nenter as \"# #\": " << std::flush;
    std::cin >> nc_pix_dims[0] >> nc_pix_dims[1];
    
    //now use COLS (x) and LINES (y) to determine view scale (crappy is 128x37)
    //  determine how many width world units a single block holds, rounding to a whole number
    //  determine view width by multiplying width world unit per block by COLS
    //  use aspect ratio to determine how many height world units a single block holds, again rounding
    //  determine view height by multiplying height world unit per block by LINES
    nc_world_dims[0] = 2000 / COLS;         //get world width units per block
    view_width = nc_world_dims[0] * COLS;   //get total world width units; <2000 due to int rounding
    
    //world height units will scale to world width based on aspect ratio
    nc_world_dims[1] = (nc_world_dims[0] * nc_pix_dims[1]) / nc_pix_dims[0]; //ordered to avoid rounding errors
    view_height = nc_world_dims[1] * LINES; //get total world height units, usually ~half of width
  }
}

viewport::~viewport() { }
