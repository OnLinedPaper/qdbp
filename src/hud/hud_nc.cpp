#if defined RENDER_NC
#include "src/hud/hud.h"
#include "src/viewport/viewport.h"
#include "src/renders/render_nc.h"
#include "src/xml_parser/xmlparse.h"
#include "src/image/image_handler_nc.h"
#include "src/entity_handler/entity_handler.h"

#include <algorithm>

hud::hud() { }
hud::~hud() { }
void hud::update() { }

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

  the shield gauge is split into 2 parts, with segment count on the left
  and recharge progress on the right. if the width is an odd number, the
  extra bar goes to the segment count. 
  dimensions are identical to health, less 1 in all aspects.
  it takes up 1/4 of the screen height, but is also one less than health, with
  a minimum of 5 blocks of height: 2 for the borders, and 3 for the shield 
  disply.
  it takes up 1/18 of the screen width, but is also one less than health, with a
  minimum of 5 blocks width: 2 for the borders, 2 for the segments, and 1 for 
  the charge.
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

  int hl_buf = he_wi;
  int hl_ht = L/4;
  int hl_ht_min = 6;
  //round up for current seg
  int hl_cs_wi = (C + 36 - 1) / 36;
  //round down for total segs
  int hl_bs_wi = C/36;
  int hl_wi_min = 6;

  int sh_buf = hl_buf + hl_cs_wi + hl_bs_wi + 2;
  bool hl_wi_eq = hl_cs_wi == hl_bs_wi;
  int sh_ht = hl_ht - 1;
  int sh_ht_min = hl_ht_min - 1;
  int sh_ts_wi = (hl_wi_eq ? hl_cs_wi : hl_cs_wi - 1);
  int sh_ch_wi = (hl_wi_eq ? hl_bs_wi - 1 : hl_bs_wi);
  int sh_wi_min = hl_wi_min - 1;

  bool full_size = (
      he_ht >= he_ht_min &&
      he_wi >= he_wi_min &&
      hl_ht >= hl_ht_min &&
      hl_cs_wi + hl_bs_wi + 2 > hl_wi_min &&
      sh_ht >= sh_ht_min &&
      sh_ts_wi + sh_ch_wi + 2 > sh_wi_min
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
    //heat name
    image_handler::get().draw_fixed_word(
        1, L-he_ht, "HEAT"
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
    std::string heat_str = std::to_string((int)(heat_frac * 100)) + "%";
    std::string ovrh_str = std::to_string((int)(ovrh_frac * 100)) + "%";

    if(is_burnout) {
      if(blink < 32) { ovrh_str = "BRNT"; heat_str = "OUT!"; }
    }
    else if(is_vent) {
      if(blink < 32) { ovrh_str = "VENT"; }
    }
    else if(heat_frac == 1) {
      if(blink < 32) { heat_str = "!!!!"; }     
      else {           heat_str = "WARN"; }
    }

    std::string s = "HEAT\n";
    s += std::string(4 - std::min((size_t)4, ovrh_str.length()), ' ') + ovrh_str;
    s += "\n";
    s += std::string(4 - std::min((size_t)4, heat_str.length()), ' ') + heat_str;

    image_handler::get().draw_fixed_word(0, L-4, s);    
  }


//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//draw health

  int backup_segs = e_handler::get().get_plr_full_health_segs();
  int health_total_segs = e_handler::get().get_plr_total_health_segs();
  float cs_frac = e_handler::get().get_plr_seg_frac();
  float bs_frac = (float)backup_segs / (float)(health_total_segs - 1);
  bool is_regen_h = e_handler::get().get_plr_is_regenerating();

  if(full_size) {
    char cs_c = '=';
    if(cs_frac == 1) { cs_c = '.'; }
    else if(is_regen_h) { cs_c = '+'; }
    char bs_c = (char)(backup_segs + 48);

    //health border
    image_handler::get().draw_fixed_box(
        hl_buf, L-hl_ht, 
        hl_buf + hl_cs_wi + hl_bs_wi + 2, L-1, 
        false, 'x'
    );
    //health name
    image_handler::get().draw_fixed_word(
        hl_buf + 1, L-hl_ht, "HLTH"
    );
     //current segment
    image_handler::get().draw_fixed_box(
        hl_buf + 1, L - ((hl_ht-1) * cs_frac),
        hl_buf + 1 + hl_cs_wi, L-2,
        true, cs_c
    );
    //backup segments
    image_handler::get().draw_fixed_box(
        //always draw at least 1 line for high seg counts that round to zero
        hl_buf + 1 + hl_cs_wi + 1, L - std::max(((hl_ht-1) * bs_frac), (float)2),
        hl_buf + 1 + hl_cs_wi + hl_bs_wi, L-2,
        true, bs_c
    );
  }
  else {
    std::string cs_str = std::to_string((int)(cs_frac * 100)) + "%";
    std::string bs_str = "x" + std::to_string(backup_segs);

    if(is_regen_h) {
      if(blink < 32) { bs_str = "REGN"; }
    }

    std::string s = "HLTH\n";
    s += std::string(4 - std::min((size_t)4, bs_str.length()), ' ') + bs_str;
    s += "\n";
    s += std::string(4 - std::min((size_t)4, cs_str.length()), ' ') + cs_str;

    image_handler::get().draw_fixed_word(5, L-3, s);
  }

//-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -
//draw shield

  int shield_total_segs = e_handler::get().get_plr_shields();
  float shield_total_seg_frac = (float)shield_total_segs / (float)e_handler::get().get_plr_total_shield_segs();
  float shield_charge = e_handler::get().get_plr_shield_frac();
  float shield_seg_frac = e_handler::get().get_plr_next_shield_seg_frac();
  bool is_regen_s = e_handler::get().get_plr_is_regenerating_shields();

  if(full_size) {
    char ts_c = (char)(shield_total_segs + 48);
    char ch_c = '=';
    if(shield_seg_frac == 1) { ch_c = '.'; }
    else if(is_regen_s) { ch_c = '+'; }

    //shield border
    image_handler::get().draw_fixed_box(
        sh_buf, L-sh_ht,
        sh_buf + sh_ts_wi + sh_ch_wi + 2, L-1,
        false, 'x'
    );
    //shield name
    image_handler::get().draw_fixed_word(
        sh_buf + 1, L-sh_ht, "SHLD"
    );
     //total segs
    image_handler::get().draw_fixed_box(
      //always draw at least 1 line for high seg counts that round to zero
      sh_buf + 1, L - std::max(((sh_ht-1) * shield_total_seg_frac), (float)2),
      sh_buf + 1 + sh_ts_wi, L-2,
      true, ts_c
    );
    //seg charge
    image_handler::get().draw_fixed_box(
      sh_buf + 1 + sh_ts_wi + 1, L - ((sh_ht-1) * shield_seg_frac),
      sh_buf + 1 + sh_ts_wi + sh_ch_wi, L-2,
      true, ch_c
    );
  }
  else {
    std::string ts_str = "x" + std::to_string(shield_total_segs);
    std::string ch_str = std::to_string((int)(shield_seg_frac * 100)) + "%";

    if(shield_charge == 1) {
      ch_str = "FULL";
    }
    else if(is_regen_s) {
      if(blink < 24) { ch_str = "CHRG"; }
    }


    std::string s = "SHLD\n";
    s += std::string(4 - std::min((size_t)4, ch_str.length()), ' ') + ch_str;
    s += "\n";
    s += std::string(4 - std::min((size_t)4, ts_str.length()), ' ') + ts_str;

    image_handler::get().draw_fixed_word(10, L-3, s);
  }

  return;
}
void hud::draw_overheat_warning() { }
#endif
