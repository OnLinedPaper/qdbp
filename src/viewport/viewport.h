#ifndef VIEWPORT_H_
#define VIEWPORT_H_
#include "src/vec2d/vec2d.h"

class viewport {
public:

  static viewport &get() {
    static viewport instance;
    return instance;
  }

  ~viewport();



  int get_w() const { return view_width; }
  int get_h() const { return view_height; }

  //center of the screen - player position
  float get_x() const { return pos[0]; }
  float get_y() const { return pos[1]; }
  //top left corner
  float get_tlc_x() const { return pos[0] - (view_width / 2); }
  float get_tlc_y() const { return pos[1] - (view_height / 2); }
  //bottom right corner
  float get_brc_x() const { return pos[0] + (view_width / 2); }
  float get_brc_y() const { return pos[1] + (view_height / 2); }

  void set_pos(const vec2d p); 

#if defined RENDER_NC
  //these are special functions for converting between ncurses
  //screen units and world units
  void nc_world_coord_to_view_coord(int &, int &, bool force_onscreen = true);

  //this pinches a line to the viewport. it expects its inputs in world coords,
  //and modifies the given vectors. 
  void nc_pinch_line_to_viewport(vec2d &, vec2d &);

  //given a rectangle, determines if any point of it is currently on the
  //screen. note that this expects the value to be in world coords.
  bool on_screen(const vec2d &, const vec2d &);
#endif

private:
  //dimensions of the viewport, in world units
  int view_width, view_height;
  vec2d pos;

  viewport();
  viewport(const viewport&) = delete;
  viewport &operator=(const viewport&) = delete;

#if defined RENDER_NC
  //these are special values related to ncurses rendering
  int prev_LINES, prev_COLS;  //used to detect changes in window size
  vec2d nc_pix_dims;          //pixels per block (font aspect ratio)
  vec2d nc_world_dims;        //world units per block

  void recalculate_view_dims();

#endif
};

#endif
