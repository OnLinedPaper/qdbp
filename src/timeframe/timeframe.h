#ifndef TIMEFRAME_H_
#define TIMEFRAME_H_

#include <chrono>

class t_frame {
public:

  ~t_frame() { }

  static t_frame &get() {
    static t_frame instance;
    return instance;
  }

  double get_delay() const { return frame_delay; }
  void set_delay(double d) { frame_delay = d; }

  double get_f() const { return frame; }
  void incr_f() {
    //increment the frame, and recalculate the delay factor
    static double e = get_ms();
    elapsed = get_ms() - e;
    delay_factor = elapsed / frame_delay;
    e = get_ms();

    frame++;
  }

  //the game was originally coded at 50fps, a 20 ms delay.
  //adjust other fps to reflect this
  double f_factor() const { return frame_delay / 20; }

  //deltatime for lag
  double d_factor() const { return delay_factor; }

  //time adjust: lag compensation and fps modification
  double t_adjust() const { return d_factor() * f_factor(); }

  double get_elapsed() { return elapsed; }

  double get_ms() const {
    return(
      std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
      ).count()
    );
  }


private:
  double frame;
  double frame_delay;
  double delay_factor;
  double fps_factor;
  double elapsed;

  t_frame() : frame(0), frame_delay(0), delay_factor(1), elapsed(0) { }
  t_frame(const t_frame&) = delete;
  t_frame &operator=(const t_frame&) = delete;
};

#endif
