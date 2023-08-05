#if defined RENDER_NC
#include "viewport.h"
#include "src/renders/render_nc.h"
#include "src/xml_parser/xmlparse.h"
#include "src/environment/chunk/chunk.h"
#include "src/rect2d/hitbox/hitline.h"
#include <iostream>
#include <ncurses.h>
#include <cmath>
#include <limits.h>

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

//viewport generally tries to keep 4 chunks as the larger of
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
    nc_world_dims[0] = (chunk::length * 4) / prev_COLS;
    view_width = nc_world_dims[0] * prev_COLS;

    nc_world_dims[1] = (nc_world_dims[0] * nc_pix_dims[1]) / nc_pix_dims[0];
    view_height = (nc_world_dims[0] * nc_pix_dims[1] * prev_LINES) / nc_pix_dims[0];
  }
  else {
    nc_world_dims[1] = (chunk::length * 4) / prev_LINES;
    view_height = nc_world_dims[1] * prev_LINES;

    nc_world_dims[0] = (nc_world_dims[1] * nc_pix_dims[0]) / nc_pix_dims[1];
    view_width = (nc_world_dims[1] * nc_pix_dims[0] * prev_COLS) / nc_pix_dims[1];
  }

}

//x and y are points in world units that need to be converted to screen 
//units - note that they may be offscreen, in which case the max value
//for the screen dim is returned
//TODO: figure out eventually how to do this without floating point division
void viewport::nc_world_coord_to_view_coord(int &x_in, int &y_in, bool force_onscreen) { 
  recalculate_view_dims();
  cint x = x_in;
  cint y = y_in;
  x = x - get_tlc_x();
  y = y - get_tlc_y();

  if(force_onscreen) {
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
  else {
    x = (x * (prev_COLS - 1)) / view_width;
    y = (y * (prev_LINES - 1)) / view_height;
  }

  x_in = std::round(x);
  y_in = std::round(y);
}

//p1 and p2 are points in world units that need to be pinched to the dimensions
//of the screen.
//the given vectors are modified. 
//TODO: move this somewhere else, like pinch_to_box?
void viewport::nc_pinch_line_to_viewport(vec2d &p1, vec2d &p2) {
  if(!viewport::on_screen(p1, p2)) { return; }

  //TAKE TWO - use intersection of lines to determine if intercept exists, and
  //if it does, replace it

  cint x_collide, y_collide = CINT_MAX;
  cint *x_ptr; x_ptr = &x_collide;
  cint *y_ptr; y_ptr = &y_collide;
  hitline l_in(p1, p2);
  hitline l_chk({0, 0}, {0, 0});

  //check up
  l_chk.set_start({get_tlc_x(), get_tlc_y()});
  l_chk.set_end(  {get_brc_x(), get_tlc_y()});
  if(l_in.collides(l_chk, x_ptr, y_ptr) && x_collide != CINT_MAX) {
    //collision - truncate line segment to up line
    if(p1[1] < get_tlc_y()) { p1[0] = x_collide; p1[1] = get_tlc_y(); }
    else {                    p2[0] = x_collide; p2[1] = get_tlc_y(); }
  }

  //check dn
  l_chk.set_start({get_tlc_x(), get_brc_y()});
  l_chk.set_end(  {get_brc_x(), get_brc_y()});
  if(l_in.collides(l_chk, x_ptr, y_ptr) && x_collide != CINT_MAX) {
    //collision - truncate line segment to dn line
    if(p1[1] > get_brc_y()) { p1[0] = x_collide; p1[1] = get_brc_y(); }
    else {                    p2[0] = x_collide; p2[1] = get_brc_y(); }
  }

  //check lf
  l_chk.set_start({get_tlc_x(), get_tlc_y()});
  l_chk.set_end(  {get_tlc_x(), get_brc_y()});
  if(l_in.collides(l_chk, x_ptr, y_ptr) && x_collide != CINT_MAX) {
    //collision - truncate line segment to lf line
    if(p1[0] < get_tlc_x()) { p1[0] = get_tlc_x(); p1[1] = y_collide; }
    else {                    p2[0] = get_tlc_x(); p2[1] = y_collide; }
  }

  //check rt
  l_chk.set_start({get_brc_x(), get_tlc_y()});
  l_chk.set_end(  {get_brc_x(), get_brc_y()});
  if(l_in.collides(l_chk, x_ptr, y_ptr) && x_collide != CINT_MAX) {
    //collision - truncate line segment to rt line
    if(p1[0] > get_brc_x()) { p1[0] = get_brc_x(); p1[1] = y_collide; }
    else {                    p2[0] = get_brc_x(); p2[1] = y_collide; }
  }

/*

  //determine which point is on the left
  bool p1_left = p1[0] < p2[0];
  if(!p1_left) {
    //x
    float tmp = p1[0];
    p1[0] = p2[0];
    p2[0] = tmp;

    //y
    tmp = p1[1];
    p1[1] = p2[1];
    p2[1] = tmp;
  }

  //determine if both points of the line are in the box defined by the screen
  //if they are: return line
  //if not:
  //- define box as 4 lines
  //- if one point is outside box:
  //- - determine intersection of line and box, and replace oob point with intersection
  //- if two are outside:
  //- - do above for both


  //see if it's above the screen
  if(p1[1] < get_tlc_y()) { 
    float m = (p2[0]-p1[0])/(p2[1]-p1[1]);
    p1[0] -= m * (p1[1] - get_tlc_y());
    p1[1] = get_tlc_y();
  }
  if(p2[1] < get_tlc_y()) { 
    float m = (p1[0]-p2[0])/(p1[1]-p2[1]);
    p2[0] -= m * (p2[1] - get_tlc_y());
    p2[1] = get_tlc_y();
  }

  //see if it's below the screen
  if(p1[1] > get_brc_y()) { 
    float m = (p2[0]-p1[0])/(p2[1]-p1[1]);
    p1[0] -= m * (p1[1] - get_brc_y());
    p1[1] = get_brc_y();
  }
  if(p2[1] > get_brc_y()) { 
    float m = (p1[0]-p2[0])/(p1[1]-p2[1]);
    p2[0] -= m * (p2[1] - get_brc_y());
    p2[1] = get_brc_y();
  }

  //see if it's to the left of the screen
  if(p1[0] < get_tlc_x()) {
    float m = (p2[1]-p1[1])/(p2[0]-p1[0]);
    p1[1] -= m * (p1[0] - get_tlc_x()); 
    p1[0] = get_tlc_x();
  }
  if(p2[0] < get_tlc_x()) {
    float m = (p1[1]-p2[1])/(p1[0]-p2[0]);
    p2[1] -= m * (p2[0] - get_tlc_x()); 
    p2[0] = get_tlc_x();
  }

  //see if it's to the right of the screen
  if(p1[0] > get_brc_x()) { 
    float m = (p2[1]-p1[1])/(p2[0]-p1[0]);
    p1[1] -= m * (p1[0] - get_brc_x());
    p1[0] = get_brc_x();
  }
  if(p2[0] > get_brc_x()) { 
    float m = (p2[1]-p1[1])/(p2[0]-p1[0]);
    p2[1] -= m * (p2[0] - get_brc_x());
    p2[0] = get_brc_x();
  }
*/  
  
  return;

}

//tlc and brc are points in world units
//TODO: validate this against bottom right side of screen
bool viewport::on_screen(const vec2d &v1, const vec2d &v2) {
  //x_tlc, y_tlc holds tlc; x_brc, y_brc holds brc
  //smaller of the two values
  int x_tlc = std::round((v1[0] < v2[0] ? v1[0] : v2[0]));
  int y_tlc = std::round((v1[1] < v2[1] ? v1[1] : v2[1]));
  //other values
  int x_brc = std::round((v1[0] < v2[0] ? v2[0] : v1[0]));
  int y_brc = std::round((v1[1] < v2[1] ? v2[1] : v1[1]));

  if(  
    x_tlc >= get_brc_x() ||
    y_tlc >= get_brc_y() ||
    x_brc < get_tlc_x() ||
    y_brc < get_tlc_y()
  ) { return false; }
  return true;
}
#endif
