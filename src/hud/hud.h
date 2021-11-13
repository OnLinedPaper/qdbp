#ifndef HUD_H_
#define HUD_H_

//the heads-up display that will be rendered on top of the screen and shown to
//the player at all times
class hud {

public:
  hud();
  ~hud();

  void update();
  void draw();
private:

  std::string blc_outline;        //outlines of the hud 
  std::string heat_curve;         //curved bar for heat gauge 
  std::string overheat_curve;     //curved bar for overheat gauge
  std::string overheat_warning;   //yellow triangle, warns rising heat
  std::string burnout_warning;    //red diamond, warns imminent burnout
  std::string burnout_sign;       //dark red x, signals burnout  
  

  float opacity;    //how much opacity to render the hud with
  float scale;      //what size to render the hud
};

#endif
