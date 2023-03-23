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


    //TODO: borders and such

    //tlc
    r[y_tlc*C + x_tlc] = 'C';

    //interior space
    for(int j=y_tlc; j<=y_brc; j++) {
      for(int i=x_tlc; i<=x_brc; i++) {
        if(in_bounds) {
          r[j*C + i] = ' ';
        }
      }
    }

    if(has_gate) {
      r[y_mid * C + x_mid] = 'O';
    }

  }
}
void chunk::debug_draw() const { }


#endif
