#include "src/hud/hud.h"
#include "src/viewport/viewport.h"
#include "src/xml_parser/xmlparse.h"
#include "src/image/image_handler.h"
#include "src/entity_handler/entity_handler.h"

hud::hud() :
  blc_outline("/" + xmlparse::get().get_xml_string(path + "/textures/blc_outline")),
  blc_outline_ht(xmlparse::get().get_xml_int(blc_outline + "/dimensions/height")),

  heat_curve("/" + xmlparse::get().get_xml_string(path + "/textures/heat_curve/body")),
  heat_curve_ht(xmlparse::get().get_xml_int(heat_curve + "/dimensions/height")),
  heat_curve_hide_angle(xmlparse::get().get_xml_int(path + "/textures/heat_curve/hide_angle")),

  health_curve("/" + xmlparse::get().get_xml_string(path + "/textures/health_curve/body")),
  health_curve_ht(xmlparse::get().get_xml_int(health_curve + "/dimensions/height")),
  health_curve_x_offset(xmlparse::get().get_xml_int(path + "/textures/health_curve/x_offset")),
  health_curve_y_offset(xmlparse::get().get_xml_int(path + "/textures/health_curve/y_offset")),
  health_curve_hide_angle(xmlparse::get().get_xml_int(path + "/textures/health_curve/hide_angle")),

  health_divider("/" + xmlparse::get().get_xml_string(path + "/textures/health_divider/body")),
  health_divider_ht(xmlparse::get().get_xml_int(health_divider + "/dimensions/height")),
  health_divider_x_offset(xmlparse::get().get_xml_int(path + "/textures/health_divider/x_offset")),
  health_divider_y_offset(xmlparse::get().get_xml_int(path + "/textures/health_divider/y_offset")),
  health_divider_hide_angle(xmlparse::get().get_xml_int(path + "/textures/health_divider/hide_angle")),

  
  shield_curve("/" + xmlparse::get().get_xml_string(path + "/textures/shield_curve")),
  shield_divider("/" + xmlparse::get().get_xml_string(path + "/textures/shield_divider")),

  overheat_warning("/" + xmlparse::get().get_xml_string(path + "/textures/overheat_warning")),
  burnout_warning("/" + xmlparse::get().get_xml_string(path + "/textures/burnout_warning")),
  burnout_sign("/" + xmlparse::get().get_xml_string(path + "/textures/burnout_sign")) 
{ }

hud::~hud() { }

void hud::update() { }

void hud::draw() { 
//TODO
  //used for flicker effects
  static float i=0;
  i += 0.2;

  //values for painting - reused for each draw
  uint8_t r, g, b = 255;

//---- draw the heat bar ------------------------------------------------------
  //- if heat is 0, spends its time chillin' under the screen
  //- as heat rises, it rotates upwards and fills the bar
  //- starts pastel yellow, becomes orange after hitting
  //  more than 50% heat frac
  float heat_frac = 1 - e_handler::get().get_plr_heat_frac();
  float display_frac = heat_curve_hide_angle * heat_frac; 
  float overheat_frac = 1 - e_handler::get().get_plr_overheat_frac();
  bool is_burnout = e_handler::get().get_plr_is_burnout();

  //change r, g, b based on heat
  if(heat_frac > 0 && !is_burnout) {
    //normal use
    r = 255;
    g = heat_frac > 0.5 ? 256 : 192 * (2 * heat_frac) + 64;
    b = heat_frac > 0.5 ? 128 : 96 * (2 * heat_frac) + 32;
  } else if (!is_burnout) {
    //overheating
    r = 255 * overheat_frac;
    g = 64 * overheat_frac;
    b = 32 * overheat_frac;
  } else {
    //burnout
    r = 128 + 64 * std::sin(i);
    g = 96 + 64 * std::sin(i);
    b = 0;
  }

  image_handler::get().draw_r_c_o_relative(
    heat_curve,
    0,
    viewport::get().get_h() - heat_curve_ht,
    0,
    display_frac,
    {r, g, b},
    1
  );

//---- draw the overheat bar --------------------------------------------------
  //identical to heat bar, but colored differently, and responds to overheat
  display_frac = heat_curve_hide_angle * overheat_frac;

  //change r, g, b based on heat
  if(!is_burnout) {
    r = 255; //overheat_frac > 0.5 ? 128 : 96 * (2 - (2 * overheat_frac)) + 64;
    g = 192 * (overheat_frac - 0.2 > 0 ? overheat_frac - 0.2 : 0) + 64 * overheat_frac;
    b = 0;
  } else {
    r = 128 + 64 * std::sin(i);
    g = 0;
    b = 0;
  }

  image_handler::get().draw_r_c_o_relative(
    heat_curve,
    0,
    viewport::get().get_h() - heat_curve_ht,
    0,
    display_frac,
    {r, g, b},
    1
  );

//---- draw the health bar ----------------------------------------------------
  //first, draw the "missing health" as yellowish green
  //then, draw existing health on top
  //then, draw the health dividers on top of that


  //draw missing health
  r = 64; g = 192; b = 0;
  float health_seg_angle = 0;

  //only show missing health up to recoverable portion
  if(e_handler::get().get_plr_health_frac() > 0) {
    health_seg_angle = 
        (float)(
        //show 1 bar if health is available to regen
        (e_handler::get().get_plr_h_available_to_regen() > 0 ? 1 : 0)  
        //get the fraction of player health available
        + e_handler::get().get_plr_full_health_segs()) 
        / (float)e_handler::get().get_plr_total_health_segs();
  }

  image_handler::get().draw_r_c_o_relative(
    health_curve,
    health_curve_x_offset,
    viewport::get().get_h() - heat_curve_ht - health_curve_y_offset,
    0,
    health_curve_hide_angle * (1 - health_seg_angle),
    {r, g, b},
    1
  );

  
  //draw health bar
  r = 64; g = 255; b = 96;

  image_handler::get().draw_r_c_o_relative(
    health_curve,
    health_curve_x_offset,
    viewport::get().get_h() - heat_curve_ht - health_curve_y_offset,
    0,
    (health_curve_hide_angle * (1 - e_handler::get().get_plr_health_frac())),
    {r, g, b},
    1
  );


  //draw health divider(s)
  r = g = b = 255;
  for(int i=1; i<e_handler::get().get_plr_total_health_segs(); i++) {
    //print a bar to divide each health segment

    image_handler::get().draw_r_c_o_relative(
      health_divider,
      health_divider_x_offset,
      viewport::get().get_h() - health_divider_ht - health_divider_y_offset,
      0,
      health_divider_hide_angle * ((float)i / e_handler::get().get_plr_total_health_segs()),
      {r, g, b},
      1
    );
  }

//TODO: shield draw

//---- draw the outline of the hud --------------------------------------------

  //needs no special data
  r = g = b = 255;  

  image_handler::get().draw_r_c_o_relative(
    blc_outline, 
    0, 
    viewport::get().get_h() - blc_outline_ht, 
    0, 
    0, 
    {r, g, b}, 
    1
  );

} 
