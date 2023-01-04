#include "drawable.h"

drawable::drawable(const std::string path) :
  drawable(path, vec2d(0,0), vec2d(0,0))
{ }

drawable::drawable(const std::string path, const vec2d vel, const vec2d pos) :
  movable(path, vel, pos),
  image_name("/" + xmlparse::get().get_xml_string(path + "/textures/body")),
  team_name(xmlparse::get().get_xml_string(path + "/team")),
  team_col({255, 255, 255}),
  frame_bump(rand()),
  last_angle(0)
{ 
#if defined RENDER_SDL
  image_handler::get_col_from_team(team_name, team_col); 
  
  //don't apply color jitter to WHITE team, that's for debugging and it helps
  //to keep it visible
  if(strcmp(team_name.c_str(), "WHITE") != 0) {
    image_handler::jitter_col(40, team_col);
  }
#endif
}
 
void drawable::update() {
  movable::update(); //recompute the angle the ship will be drawn at, smoothing it out for sudden
  //changes in direction
    if(vel.magnitude() != 0) {
      //smooth-spin the image
      float threshold = 15;
      //if the angle change is greater than threshold, turn threshold at a time
      float a = vel.angle_deg();

      //add 360 so there's no issue with dropping from 0 to 360, this
      //will later be modded out
      a += 360;
      last_angle += 360;

      //check that the angle is greater than some threshold degrees, but also
      //check for wrap-around from 360 to 0
      //TODO: rotation speed drops with lower tick rate - this also affects hitboxes. scale it up?
      if(abs(a - last_angle) > threshold &&
        abs(abs(a- last_angle) - 360) > threshold)
      {
        //steep turn - change it by finding which way to turn
        if((int(a - last_angle) + 360) % 360 > 180) {
          last_angle -= threshold;
        }
        else {
          last_angle += threshold;
        }
      }
      else {
        last_angle = a;
      }
    }
    last_angle = int(last_angle) % 360;
}

void drawable::draw() const {
  movable::draw();
}
