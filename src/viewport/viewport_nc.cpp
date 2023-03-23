#if defined RENDER_NC
#include "viewport.h"
#include "src/renders/render_nc.h"
#include "src/xml_parser/xmlparse.h"
#include "src/environment/chunk/chunk.h"
#include <iostream>
#include <ncurses.h>

viewport::viewport() { 
  view_width = view_height = 0;
  prev_LINES = prev_COLS = -1;
  pos[0] = pos[1] = 0;
  nc_pix_dims[0] = nc_pix_dims[1] = 0;
  nc_world_dims[0] = nc_world_dims[1] = 0;

  //load aspect ratio
  nc_pix_dims[0] = xmlparse::get().get_xml_int("/ncurses_rendering/aspect_ratio/width");
  nc_pix_dims[1] = xmlparse::get().get_xml_int("/ncurses_rendering/aspect_ratio/height");

  recalculate_view_dims();
}

void viewport::set_pos(const vec2d p) {
  recalculate_view_dims();
  pos = p;
}

//viewport generally tries to keep 2 chunks as the larger of
//its two dimensions. 
//any time the window is resized, this function runs to re-determine the 
//ratio of world units to "screen units", which is to say, how many world 
//units fit into one terminal character.
void viewport::recalculate_view_dims() { 

  //first check if it's even acceptable to update - if r is null, the window
  //recently changed size, and it's not safe to update
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  //now check against the returned LINES and COLS values
  if(C == prev_COLS && L == prev_LINES) { return; }

  prev_LINES = L;
  prev_COLS = C;

  //now use COLS (x) and LINES (y) to determine view scale (crappy is 128x37)
  // (for width; height is similar):
  //  determine how many width world units a single block holds, rounding to a whole number
  //  determine view width by multiplying width world unit per block by COLS
  //  use aspect ratio to determine how many height world units a single block holds, again rounding
  //  determine view height by multiplying height world unit per block by LINES

  //determine if the terminal is currently wide or tall
  bool wider = prev_COLS * nc_pix_dims[0] > prev_LINES * nc_pix_dims[1];
  if(wider) {
    nc_world_dims[0] = (chunk::length * 2) / prev_COLS;
    view_width = nc_world_dims[0] * prev_COLS;

    nc_world_dims[1] = (nc_world_dims[0] * nc_pix_dims[1]) / nc_pix_dims[0];
    view_height = nc_world_dims[1] * prev_LINES;
  }
  else {
    nc_world_dims[1] = (chunk::length * 2) / prev_LINES;
    view_height = nc_world_dims[1] * prev_LINES;

    nc_world_dims[0] = (nc_world_dims[1] * nc_pix_dims[0]) / nc_pix_dims[1];
    view_width = nc_world_dims[0] * prev_COLS;
  }

}

//x and y are points in world units that need to be converted to screen 
//units - note that they may be offscreen, in which case the max value
//for the screen dim is returned
void viewport::nc_world_coord_to_view_coord(int &x, int &y) { 
  recalculate_view_dims();
  x = x - get_tlc_x();
  y = y - get_tlc_y();

  if(x < 0) { x = 0; }
  else if(x >= view_width) { x = prev_COLS - 1; }
  else {
    x = (x * (prev_COLS - 1)) / view_width;
  }

  if(y < 0) { y = 0; }
  else if(y >= view_height) { y = prev_LINES - 1; }
  else {
    y = (y * (prev_LINES - 1)) / view_height;
  }
}
#endif

//tlc and brc are points in world units
//TODO: validate this against bottom right side of screen
bool viewport::on_screen(const vec2d &tlc_in, const vec2d &brc_in) {
  if(  
    tlc_in[0] > get_brc_x() ||
    tlc_in[1] > get_brc_y() ||
    brc_in[0] < get_tlc_x() ||
    brc_in[1] < get_tlc_y()
  ) { return false; }
  return true;
}
