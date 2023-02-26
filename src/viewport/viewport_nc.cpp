#if defined RENDER_NC
#include "viewport.h"
#include "src/renders/render.h"
#include "src/xml_parser/xmlparse.h"
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

//viewport generally tries to keep 2 chunks (2000 world units) as the larger of
//its two dimensions. 
//any time the window is resized, this function runs to re-determine the 
//ratio of world units to "screen units", which is to say, how many world 
//units fit into one terminal character.
void viewport::recalculate_view_dims() { 
  if(COLS == prev_COLS && LINES == prev_LINES) { return; }

  //determine if the terminal is currently wide or tall
  bool wider = COLS * nc_pix_dims[0] > LINES * nc_pix_dims[1];
}


void viewport::nc_world_coord_to_view_coord(int &x, int &y) { }
#endif
