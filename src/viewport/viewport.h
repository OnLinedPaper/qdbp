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

  void set_pos(const vec2d p) { pos = p; }

  void convert_to_nc_screen_units(int &x, int &y);

private:
  int view_width, view_height;
  int prev_LINES, prev_COLS;
  vec2d pos;

  vec2d nc_pix_dims;    //how many pixels are in one block (font aspect ratio)
  vec2d nc_world_dims;  //how many world units are in one block

  void recalculate_view_dims();

  viewport();
  viewport(const viewport&) = delete;
  viewport &operator=(const viewport&) = delete;
};

#endif
