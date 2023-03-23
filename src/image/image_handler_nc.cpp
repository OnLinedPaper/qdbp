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
    //TODO: call draw_line 4 times
  }

  
}
#endif
