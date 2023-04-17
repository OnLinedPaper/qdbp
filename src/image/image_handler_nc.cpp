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
  bool p1_left = p1[0] < p2[0];
  int lp_x = (p1_left ? p1[0] : p2[0]);
  int lp_y = (p1_left ? p1[1] : p2[1]);
  int rp_x = (p1_left ? p2[0] : p1[0]);
  int rp_y = (p1_left ? p2[1] : p1[1]);

  //convert - yes this drops precision, no i don't care
  viewport::get().nc_world_coord_to_view_coord(lp_x, lp_y);
  viewport::get().nc_world_coord_to_view_coord(rp_x, rp_y);

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
  for(int x = 0, y = 0; x < rp_x; x++) {
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
    //TODO: call draw_line 4 times
  }
}

void image_handler::draw_circle(const vec2d &v, float r, bool filled, char c) {
  draw_point(v, c);

  //establish initial vars - start at bottom of circle, and draw towards the right
  float x = 0;
  float y = r;

  //while(x<y) {
    draw_point({v[0]+x, v[1]+y}, c);
    draw_point({v[0]+x, v[1]-y}, c);
    draw_point({v[0]-x, v[1]+y}, c);
    draw_point({v[0]-x, v[1]-y}, c);
    draw_point({v[0]+y, v[1]+x}, c);
    draw_point({v[0]+y, v[1]-x}, c);
    draw_point({v[0]-y, v[1]+x}, c);
    draw_point({v[0]-y, v[1]-x}, c);
    x++;
  //}
}
#endif
