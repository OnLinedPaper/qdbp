#include "drawable.h"

void drawable::update() {
  movable::update(); //recompute the angle the ship will be drawn at, smoothing it out for sudden
  //changes in direction
    if(vel.magnitude() != 0) {
      //smooth-spin the image
      double threshold = 15;
      //if the angle change is greater than threshold, turn threshold at a time
      double a = vel.angle_deg();

      //add 360 so there's no issue with dropping from 0 to 360, this
      //will later be modded out
      a += 360;
      last_angle += 360;

      //check that the angle is greater than some threshold degrees, but also
      //check for wrap-around from 360 to 0
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
