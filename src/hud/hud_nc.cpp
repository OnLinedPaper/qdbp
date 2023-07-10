#if defined RENDER_NC
#include "src/hud/hud.h"
#include "src/viewport/viewport.h"
#include "src/renders/render_nc.h"
#include "src/xml_parser/xmlparse.h"
#include "src/image/image_handler_nc.h"
#include "src/entity_handler/entity_handler.h"

#include <algorithm>

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

  it takes up 1/3 of the screen height, with a minimum of 7 blocks height: 2
  for the borders, and 5 for the heat display.
  it takes up 1/12 of the screen width, with a minimum of 8 blocks width: 2
  for the borders, and 6 for the heat display.
  heat is a solid block, and overheat is either non-solid, or flashing.
  its "minimalized" form is a pair of 3-digit numbers: heat on the bottom and
  overheat on the top, as percent representations of the values. when overheat
  is above 000, heat becomes "XXX" and flashes. 

  the health gauge is split into 2 parts, with current segment health on the
  left and total segment count on the right. if the width is an odd number, the
  extra bar goes to the current segment health.
  it takes up 1/4 of the screen height, with a minimum of 6 blocks height:
  2 for the borders, and 4 for the health display.
  it takes up 1/18 of the screen width, with a minimum of 6 blocks width:
  2 for the borders, 2 for the health display, and 2 for the segments.
*/


//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//initial setup

  //check if it's safe to darw
  char *r = NULL; int L, C = 0;
  render::get().get_r(r, L, C);
  if(r == NULL) { return; }

  static uint8_t blink = 0;
  blink++;
  blink = blink % 64;

  int he_ht = L/3;
  int he_ht_min = 7;
  int he_wi = C/12;
  int he_wi_min = 8;

  int hl_ht = L/4;
  int hl_ht_min = 6;
  //round up for current seg
  int hl_cs_wi = (C + 36 - 1) / 36;
  //round down for total segs
  int hl_bs_wi = C/36;
  int hl_wi_min = 6;

  bool full_size = (
      he_ht >= he_ht_min &&
      he_wi >= he_wi_min &&
      hl_ht >= hl_ht_min &&
      hl_cs_wi + hl_bs_wi + 2 > hl_wi_min
  );


//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//draw heat

  float heat_frac = e_handler::get().get_plr_heat_frac();
  float ovrh_frac = e_handler::get().get_plr_overheat_frac();

  bool is_burnout = e_handler::get().get_plr_is_burnout();
  bool is_vent = e_handler::get().get_plr_is_vent();
  char heat_c = 'H';
  char ovrh_c = 'O';
 
  if(full_size) {
    if(is_burnout) {
        if(blink < 32) { heat_c = 'x'; }
        else {           heat_c = 'X'; }
    }
    else if(is_vent) {
        ovrh_c = 'V';
        heat_c = 'v';
    }
    else if(heat_frac == 1) {
        heat_c = '!';
        if(blink < 32) { ovrh_c = ' '; }
    }

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
        true, heat_c
    );
    //overheat
    image_handler::get().draw_fixed_box(
        1, L - ((he_ht-1) * ovrh_frac),
        he_wi-1, L-2,
        true, ovrh_c
    );
  }
  else {
    //stackoverflow.com/a/26343947
    std::string heat_str = std::to_string((int)(heat_frac * 100));
    std::string ovrh_str = std::to_string((int)(ovrh_frac * 100));

    if(is_burnout) {
      if(blink < 32) { ovrh_str = "OVR"; heat_str = "HTD"; }
    }
    else if(is_vent) {
      if(blink < 32) { ovrh_str = "VNT"; }
    }
    else if(heat_frac == 1) {
      if(blink < 32) { heat_str = "!!!"; }     
      else {           heat_str = "WRN"; }
    }

    std::string s = "HEAT\n";
    s += std::string(3 - std::min((size_t)3, ovrh_str.length()), ' ') + ovrh_str;
    s += "%\n";
    s += std::string(3 - std::min((size_t)3, heat_str.length()), ' ') + heat_str;
    s += "%";

    image_handler::get().draw_fixed_word(0, L-4, s);    
  }


//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//draw health

  int backup_segs = e_handler::get().get_plr_full_health_segs();
  int total_segs = e_handler::get().get_plr_total_health_segs();
  float cs_frac = e_handler::get().get_plr_seg_frac();
  float bs_frac = (float)backup_segs / (float)(total_segs - 1);
  bool is_regen = e_handler::get().get_plr_is_regenerating();

  if(full_size) {
    char cs_c = '+';
    char bs_c = (char)(backup_segs + 48);

    //health border
    image_handler::get().draw_fixed_box(
        he_wi, L-hl_ht, 
        he_wi + hl_cs_wi + hl_bs_wi + 2, L-1, 
        false, 'x'
    );
    //current segment
    image_handler::get().draw_fixed_box(
        he_wi + 1, L - ((hl_ht-1) * cs_frac),
        he_wi + 1 + hl_cs_wi, L-2,
        true, cs_c
    );
    //backup segments
    image_handler::get().draw_fixed_box(
        //always draw at least 1 line for high seg counts that round to zero
        he_wi + 1 + hl_cs_wi + 1, L - std::max(((hl_ht-1) * bs_frac), (float)2),
        he_wi + 1 + hl_cs_wi + hl_bs_wi, L-2,
        true, bs_c
    );
  }
  else {
    std::string cs_str = std::to_string((int)(cs_frac * 100));
    std::string bs_str = "x" + std::to_string(backup_segs);

    if(is_regen) {
      if(blink < 32) { bs_str = "REGN"; }
    }

    std::string s = "HLTH\n";
    s += std::string(4 - std::min((size_t)4, bs_str.length()), ' ') + bs_str;
    s += "\n";
    s += std::string(3 - std::min((size_t)3, cs_str.length()), ' ') + cs_str;
    s += "%";

    image_handler::get().draw_fixed_word(5, L-3, s);
  }

  return;
}
void hud::draw_overheat_warning() { }
#endif
