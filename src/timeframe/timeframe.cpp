#include "timeframe.h"
#include <chrono>
#include <iostream>

//master tick rate - may change later
//if it does, go check t_factor() to see if it's still needed
const int t_frame::tickrate = 20;

void t_frame::incr_t() {
  //increment the frame, and recalculate the delay factor
  static double e = get_ms();
  elapsed_ms = get_ms() - e;
  delay_factor = elapsed_ms / tick_delay;
  e = get_ms();

  tick++;

  incr_f();
}

bool t_frame::incr_f() {
  static double e = get_ms();
  double elaps_ms = get_ms() - e;
  if(elaps_ms > (frame_delay)) {
    //time for the next frame
    e = get_ms();

    frame++;
    return true;
  }

  //too soon to go to the next frame
  return false;
}

double t_frame::get_ms() const {
  return(
    std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count()
  );
}
