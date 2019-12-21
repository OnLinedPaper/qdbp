#ifndef TIMEFRAME_H_
#define TIMEFRAME_H_


class t_frame {
public:

  ~t_frame() { }

  static t_frame &get() {
    static t_frame instance;
    return instance;
  }

  float get_delay() const { return frame_delay; }
  void set_delay(float d) { frame_delay = d; }

  float get_f() const { return frame; }
  void incr_f();

  //the game was originally coded at 50fps, a 20 ms delay.
  //adjust other fps to reflect this
  float f_factor() const { return frame_delay / 20; }

  //deltatime for lag
  float d_factor() const { return delay_factor; }

  //time adjust: lag compensation and fps modification
  float t_adjust() const { return d_factor() * f_factor(); }

  float get_elapsed() { return elapsed; }

  double get_ms() const;


private:
  float frame;
  float frame_delay;
  float delay_factor;
  float fps_factor;
  float elapsed;

  t_frame() : frame(0), frame_delay(0), delay_factor(1), elapsed(0) { }
  t_frame(const t_frame&) = delete;
  t_frame &operator=(const t_frame&) = delete;
};

#endif
