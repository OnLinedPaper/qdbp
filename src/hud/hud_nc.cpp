#if defined RENDER_NC
#include "src/hud/hud.h"
#include "src/viewport/viewport.h"
#include "src/renders/render_nc.h"
#include "src/xml_parser/xmlparse.h"
#include "src/image/image_handler_nc.h"
#include "src/entity_handler/entity_handler.h"


void hud::draw() {
/*
+------------------------------------+
|
|
|
|
|                 +
|
|HH
|HHhs
|HHhs
+------------------------------------+

  the heat gauge is twice as wide as the health and shield gauges, and is 3/2
  as tall. 
  it takes up 1/3 of the screen height, with a minimum of 7 blocks height: 2
  for the borders, and 5 for the heat display.
  it takes up 1/12 of the screen width, with a minimum of 8 blocks width: 2
  for the borders, and 6 for the heat display.
  heat is a solid block, and overheat is either non-solid, or flashing.
  its "minimalized" form is a pair of 3-digit numbers: heat on the bottom and
  overheat on the top, as percent representations of the values. when overheat
  is above 000, it flashes. 
*/


//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//initial setup

  //check if it's safe to darw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  int he_ht = L/3;
  int he_ht_min = 7;
  int he_wi = C/12;
  int he_wi_min = 8;

  bool full_size = (
      he_ht >= he_ht_min &&
      he_wi >= he_wi_min
  );


//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//draw heat

  float heat_frac = e_handler::get().get_plr_heat_frac();
  float ovrh_frac = e_handler::get().get_plr_overheat_frac();
 
  if(full_size) {
    //heat border
    image_handler::get().draw_fixed_box(
        0, L-he_ht, 
        he_wi, L-1, 
        false, 'x'
);
    //heat
    image_handler::get().draw_fixed_box(
        1, L - ((he_ht-1) * heat_frac),
        he_wi-1, L-2,
        true, 'H'
    );
    //overheat
    image_handler::get().draw_fixed_box(
        1, L - ((he_ht-1) * ovrh_frac),
        he_wi-1, L-2,
        true, '!'
    );
  }
  else {
    //TODO: fixed width
    std::string s = "";

    s+= std::to_string((int)(ovrh_frac * 100));
    s+= "%\n";
    s += std::to_string((int)(heat_frac * 100));
    s += "%";

    image_handler::get().draw_fixed_word(0, L-3, s);    
  }

  return;
}
void hud::draw_overheat_warning() { }
#endif
