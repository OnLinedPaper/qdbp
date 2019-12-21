#include "timeframe.h"
#include <chrono>


void t_frame::incr_f() {
  //increment the frame, and recalculate the delay factor
  static double e = get_ms();
  elapsed = get_ms() - e;
  delay_factor = elapsed / frame_delay;
  e = get_ms();

  frame++;
}

double t_frame::get_ms() const {
  return(
    std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch()
    ).count()
  );
}
