#ifndef TIMEFRAME_H_
#define TIMEFRAME_H_


//note that this game ticks at 20 times per second. rendering may be
//faster; the tickrate will be adjusted in the future. this value is hardcoded
//and cannot be adjusted by the user, unlike framerate

class t_frame {
public:
  static const int tickrate;

  ~t_frame() { }

  static t_frame &get() {
    static t_frame instance;
    return instance;
  }

  float get_t_delay() const { return tick_delay; }
  void set_t_delay(float d) { tick_delay = d; }

  float get_t() const { return tick; }
  void incr_t();

  float get_f_delay() const { return frame_delay; }
  void set_f_delay(float d) { frame_delay = d; }

  float get_f() const { return frame; }
  bool incr_f();

  //the game was originally coded at 20 ticks per second
  //IF the tickrate is changed, change the motion of other entities
  //to reflect this
  //note that this may not be necessary? check later if tickrate changes
  //at present, always returns 1
  float t_factor() const { return tick_delay / 20; }

  //deltatime for lag
  //note that for any game stuff, t_adjust is a better pick
  float d_factor() const { return delay_factor; }

  //time adjust: lag compensation and fps modification
  float t_adjust() const { return d_factor() * t_factor(); }

  float get_elapsed_ms() { return elapsed_ms; }
  float get_elapsed_t() { return t_factor(); }

  double get_ms() const;


private:
  float tick;
  float tick_delay;

  float elapsed_ms;  

  float frame;
  float frame_delay;
  float delay_factor;
  float fps_factor;

  t_frame() : tick(0), tick_delay(0), 
      elapsed_ms(0), 
      frame(0), frame_delay(0), delay_factor(1) { }
  t_frame(const t_frame&) = delete;
  t_frame &operator=(const t_frame&) = delete;
};

#endif
