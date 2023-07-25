#if defined RENDER_NC
#include "src/image/image_handler_nc.h"
#include "src/viewport/viewport.h"
#include "src/renders/render_nc.h"

image_handler::image_handler() { }
image_handler::~image_handler() { }

void image_handler::draw_point(const vec2d &v, char c) {
  //first check if the point is on screen
  if(!viewport::get().on_screen(v, v)) { return; }
  
  //now check if it's safe to darw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }
  
  //convert the point from world coords to view coords
  int x = v[0]; 
  int y = v[1];
  viewport::get().nc_world_coord_to_view_coord(x, y);

  r[y*C +x] = c;

  return;
}

void image_handler::draw_word(const vec2d &v, const std::string &s) {
  //check if it's safe to draw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  //translate top left corner to view coords
  int x = v[0];
  int y = v[1];
  viewport::get().nc_world_coord_to_view_coord(x, y, false);


  //get "dimensions" of word to determine if it's on screen
  int w = 0;
  int h = 0;
  int count = 0;
  for(uint i=0; i<s.length(); i++) {
    if(s[i] == '\n') {
      w = (count > w ? count : w);
      h++;
      count = 0;
    }
    else {
      count++;
    }
  }

  w = (count > w ? count : w);
  if(w > 0) { h++; }

  //check if it's on screen
  if(
      x >= C ||
      y >= L ||
      x + w < 0 ||
      y + h < 0
  ) { return; }
  
  draw_fixed_word(x, y, s);
}

void image_handler::draw_fixed_word(int x, int y, const std::string &s) {
  //check if it's safe to draw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  //x and y are both screen coords, not world coords. this function is notable
  //as being one of the only ones that allows negative coords in, for the 
  //purpose of word triming. 

  //draw word on screen as best as possible: "fit" it to the screen if it would
  //be otherwise ofscreen. also interpret newlines.

  int i = 0; 
  int j = 0;
  for(auto c : s) {
    //check of we're below the screen, and quit if so
    if(y+j >= L) { return; }

    //check for newline
    if(c == '\n') { j++; i=0; continue; }

    //check if we're above the screen, to the left of it, or to the right of it
    //and skip this letter if so
    if(y+j < 0 || x+i < 0 || x+i >= C) { i++; continue; }


    r[(y+j)*C + (x+i)] = c;

    i++;
  }
  
}

void image_handler::draw_line(const vec2d &p1_in, const vec2d &p2_in, char c) {
  //check if the line is on screen
  if(!viewport::get().on_screen(p1_in, p2_in)) { return; }

  //now check if it's safe to darw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  //pinch the line such that it safely appears on screen
  vec2d p1, p2;
  p1[0] = p1_in[0]; p1[1] = p1_in[1];
  p2[0] = p2_in[0]; p2[1] = p2_in[1];
  viewport::get().nc_pinch_line_to_viewport(p1, p2);

  //lp is left point; rp is right point
  //TODO: this probably isn't strictly necessary anymore
  bool p1_left = p1[0] < p2[0];
  int lp_x = (p1_left ? p1[0] : p2[0]);
  int lp_y = (p1_left ? p1[1] : p2[1]);
  int rp_x = (p1_left ? p2[0] : p1[0]);
  int rp_y = (p1_left ? p2[1] : p1[1]);

  //convert - yes this drops precision, no i don't care
//TODO: i do in fact care. come back once everything is ints
  viewport::get().nc_world_coord_to_view_coord(lp_x, lp_y);
  viewport::get().nc_world_coord_to_view_coord(rp_x, rp_y);

  draw_fixed_line(lp_x, lp_y, rp_x, rp_y, c);
}

void image_handler::draw_fixed_line(int x_p1, int y_p1, int x_p2, int y_p2, char c) {
  //x_p1, y_p1, x_p2, y_p2 are all related directly to screen, not to world coords

  //check if it's safe to draw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  //make sure none of these coords are out of bounds
  if(
    (x_p1 < 0 || x_p1 >= C) ||
    (x_p2 < 0 || x_p2 >= C) ||
    (y_p1 < 0 || y_p1 >= L) ||
    (y_p2 < 0 || y_p2 >= L)
  ) { return; }

  /*
  adopting a somewhat naive implementation of bresenham's algorithm for this
  one. i'll tidy it up when i eventually get around to converting all floats
  to ints. 
  https://www.geeksforgeeks.org/bresenhams-line-generation-algorithm/

  assuming the top-left corner is (0, 0) there can be considered 4 kinds of
  lines: 
  - soft positive slope: positive slope less than 1; line points downward and
    to the right at an angle less than 45 degrees from the x axis. these are  
    the ideal lines, and all others will be altered to match them.
  - hard positive slope: positive slope greater than 1; line points downwards 
    and to the right at an angle more than 45 degrees from the x axis. can be 
    converted to soft-positive by switching the x,y of the endpoint.
  - soft negative slope: negative slope between 0 and -1; line points upwards 
    and to the right at an angle less than 45 degrees from the x axis. can be 
    converted to soft-positive by flipping the sign of the endpoint's y 
    coordinate.
  - hard negative slope: negative slope less than -1; line points upwards and 
    to the right at an angle more than 45 degrees from the x axis. can be 
    converted to soft-positive by flipping the sign of the endpoint's y 
    coordinate and then switching the x,y of the endpoint.

  after converting the line to its ideal form (and remembering the
  conversions), calculate its slope and an initial "error" (0.5).
  draw x, add slope to error, and if it's > 1, subtract 1 and increment y.
  then, draw y and increment x. 
  */

 //lp is left point; rp is right point
  bool p1_left = x_p1 < x_p2;
  int lp_x = (p1_left ? x_p1 : x_p2);
  int lp_y = (p1_left ? y_p1 : y_p2);
  int rp_x = (p1_left ? x_p2 : x_p1);
  int rp_y = (p1_left ? y_p2 : y_p1);

  //offset all coordinates such that lp is 0, 0; this makes subsequent 
  //calculations much easier
  int offset_x = lp_x;
  int offset_y = lp_y;
  rp_x -= lp_x;
  rp_y -= lp_y;
  lp_x = 0;
  lp_y = 0;


  bool m_neg = false;
  bool m_hard = false;

  //determine if slope is positive or negative
  if(rp_y < 0) {
    m_neg = true;
    //mirror it over the x axis
    rp_y = -rp_y;
  }

  //determine of slope is hard or soft
  if(rp_y > rp_x) {
    m_hard = true;
    //flip the x and y
    int temp = rp_x;
    rp_x = rp_y;
    rp_y = temp;
  }


  //iterate through the line from lp_x to rp_x, and increment y and error 
  //as we go
  int m = 2 * rp_y;
  int m_error = m - rp_x;

  int x_render, y_render = 0;
  for(int x = 0, y = 0; x <= rp_x; x++) {
    x_render = x;
    y_render = y;

    if(m_hard) {
      int temp = x_render;
      x_render = y_render;
      y_render = temp;
    }
    if(m_neg) {
      y_render = -y_render;
    }

    x_render += offset_x;
    y_render += offset_y;

    //due to int rounding errors and what i assume is an update issue
    //somewhere, these values occasionally stray past the bounds by 1
    if(x_render >= C) { x_render = C-1; }
    if(x_render < 0 ) { x_render = 0;   }
    if(y_render >= L) { y_render = L-1; }
    if(y_render < 0 ) { y_render = 0;   }

    r[y_render * C + x_render] = c;

    m_error += m;
    if(m_error >= 0) {
      y++;
      m_error -= 2 * rp_x;
    }
  }
}

void image_handler::draw_box(const vec2d &v1, const vec2d &v2, bool filled, char c) {
  //x_tlc, y_tlc holds tlc; x_brc, y_brc holds brc
  //smaller of the two values
  int x_tlc = (v1[0] < v2[0] ? v1[0] : v2[0]);
  int y_tlc = (v1[1] < v2[1] ? v1[1] : v2[1]);
  //other values
  int x_brc = (x_tlc == v1[0] ? v2[0] : v1[0]);
  int y_brc = (y_tlc == v1[1] ? v2[1] : v1[1]);

  //check if the box is on screen
  if(!viewport::get().on_screen({x_tlc, y_tlc}, {x_brc, y_brc})) { return; }

  //now check if it's safe to darw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  
  //convert world coords to view coords
  viewport::get().nc_world_coord_to_view_coord(x_tlc, y_tlc);
  viewport::get().nc_world_coord_to_view_coord(x_brc, y_brc);
  
  //draw
  if(filled) {
    for(int j=y_tlc; j<=y_brc; j++) {
      for(int i=x_tlc; i<=x_brc; i++) {
        r[j*C + i] = c;
      }
    }
  }
  else {
    //top, bottom, left, right
    draw_line({v1[0], v1[1]}, {v2[0], v1[1]}, c);
    draw_line({v1[0], v2[1]}, {v2[0], v2[1]}, c);
    draw_line({v1[0], v1[1]}, {v1[0], v2[1]}, c);
    draw_line({v2[0], v1[1]}, {v2[0], v2[1]}, c);
  }
}

void image_handler::draw_fixed_box(int x_tlc, int y_tlc, int x_brc, int y_brc, bool filled, char c) {
  //x_tlc, y_tlc, x_brc, and y_brc all relate directly to the screen, not to world coords

  //check if it's safe to draw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  //make sure none of these coords are out of bounds
  if(
    (x_tlc < 0 || x_tlc >= C) ||
    (x_brc < 0 || x_brc >= C) ||
    (y_tlc < 0 || y_tlc >= L) ||
    (y_brc < 0 || y_brc >= L)
  ) { return; }

  if(filled) {
    for(int j=y_tlc; j<=y_brc; j++) {
      for(int i=x_tlc; i<=x_brc; i++) {
        r[j*C + i] = c;
      }
    }
  }
  else {
    draw_fixed_line(x_tlc, y_tlc, x_brc, y_tlc, c);
    draw_fixed_line(x_brc, y_tlc, x_brc, y_brc, c);
    draw_fixed_line(x_tlc, y_brc, x_brc, y_brc, c);
    draw_fixed_line(x_tlc, y_tlc, x_tlc, y_brc, c);
  }
}

void image_handler::draw_circle(const vec2d &v, float r, bool filled, char c) {
  //using bresenham's circle here. god the derivation made my hand hurt.

  //establish initial vars - start at bottom of circle, and draw towards the right
  int x = 0;
  int y = r;
  int error = 3 - 2*r;

  while(x<=y) {

    if(error > 0) {
      y--;
      error += 2 * (5 - 2 * y + 2 * x);
    }
    else {
      error += 2 * (3 + 2 * x);
    }

    draw_point({v[0]+x, v[1]+y}, c);
    draw_point({v[0]+x, v[1]-y}, c);
    draw_point({v[0]-x, v[1]+y}, c);
    draw_point({v[0]-x, v[1]-y}, c);
    draw_point({v[0]+y, v[1]+x}, c);
    draw_point({v[0]+y, v[1]-x}, c);
    draw_point({v[0]-y, v[1]+x}, c);
    draw_point({v[0]-y, v[1]-x}, c);
    x++;
  }
}
#endif
