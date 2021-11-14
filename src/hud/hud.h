#ifndef HUD_H_
#define HUD_H_

#include <string>

//the heads-up display that will be rendered on top of the screen and shown to
//the player at all times
class hud {

public:

  static hud &get() {
    static hud instance;
    return instance;
  }
  ~hud();

  void update();
  void draw();

private:
  std::string path = "/hud";

  std::string blc_outline;        //outlines of the hud 
  int blc_outline_ht;             //height of the hud

  std::string heat_curve;         //curved bar for heat gauge 
  int heat_curve_ht;              //height of heat curve
  int heat_curve_hide_angle;      //angle at which to display heat curve
                                  //to keep it just barely off-screen

  std::string health_curve;       //curved bar for health gauge
  int health_curve_ht;            //height of health curve
  int health_curve_x_offset;      //distance from left side of screen
  int health_curve_y_offset;      //distance from bottom side of screen
  int health_curve_hide_angle;    //angle at which to display health curve

  std::string health_divider;     //line to indicate health segments
  int health_divider_ht;
  int health_divider_x_offset;
  int health_divider_y_offset;
  int health_divider_hide_angle;

  std::string shield_curve;       //curved line for shield gauge
  std::string shield_divider;     //line for indicating shield segments

  std::string overheat_warning;   //yellow triangle, warns rising heat
  std::string burnout_warning;    //red diamond, warns imminent burnout
  std::string burnout_sign;       //dark red x, signals burnout  
  

  float opacity;    //how much opacity to render the hud with
  float scale;      //what size to render the hud

  hud();
  hud(const hud&) = delete;
  hud &operator=(const hud&) = delete;
};

#endif
