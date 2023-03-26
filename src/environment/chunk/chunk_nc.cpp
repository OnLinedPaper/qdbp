#if defined RENDER_NC
#include "chunk.h"
#include "src/renders/render_nc.h"
#include "src/viewport/viewport.h"
#include "src/image/image.h"
#include "src/image/image_handler.h"
#include "src/xml_parser/xmlparse.h"
#include "src/entity_handler/entity_handler.h"
#include "src/utils/rng.h"
#include "src/utils/message.h"
#include "src/text/text.h"
#include "src/image/image_handler_nc.h"

void chunk::draw() const { 
  int x_tlc = tlc[0];
  int y_tlc = tlc[1];
  int x_mid = tlc[0] + (length / 2);
  int y_mid = tlc[1] + (length / 2);
  int x_brc = tlc[0] + length;
  int y_brc = tlc[1] + length;

  //check to see if this chunk is even visible on screen
  if(viewport::get().on_screen(tlc, {tlc[0] + length, tlc[1] + length})) {
    char *r = NULL; int L, C = 0;
    render::get().get_r(r, L, C);

    //check if it's safe to draw
    if(r == NULL) { return; }

    viewport::get().nc_world_coord_to_view_coord(x_tlc, y_tlc);
    viewport::get().nc_world_coord_to_view_coord(x_mid, y_mid);
    viewport::get().nc_world_coord_to_view_coord(x_brc, y_brc);


    //interior space
    if(in_bounds) {
      image_handler::get().draw_box(tlc, {tlc[0] + length, tlc[1] + length}, true, ' ');
    }

    //boundaries
    if(border[0]) {
      //top is a border
      image_handler::get().draw_line(tlc, {tlc[0] + length, tlc[1]}, 'X');
    }
    if(border[1]) {
      //bottom is a border
      image_handler::get().draw_line({tlc[0], tlc[1] + length}, {tlc[0] + length, tlc[1] + length}, 'X');
    }
    if(border[2]) {
      //left is a border
      image_handler::get().draw_line(tlc, {tlc[0], tlc[1] + length}, 'X');
    }
    if(border[3]) {
      //right is a border
      image_handler::get().draw_line({tlc[0] + length, tlc[1]}, {tlc[0] + length, tlc[1] + length}, 'X');
    }

    if(has_gate) {
      image_handler::get().draw_point({tlc[0] + (length/2), tlc[1] + (length/2)}, 'G');
    }

  }
}
void chunk::debug_draw() const { }


#endif
