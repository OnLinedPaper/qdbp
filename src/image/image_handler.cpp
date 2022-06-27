#include "image_handler.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
#include "src/rect2d/rect2d.h"
#include "src/rect2d/hitbox/hitline.h"
#include <utility>
#include <algorithm>

//does this initialize images?? sure fuckin hope so
image_handler::image_handler() : images() { }

image_handler::~image_handler() {
  //remove the images
  images.clear();
}

void image_handler::draw_rotate_color(const std::string name, float x, float y,
    float frame_bump, float angle, const SDL_Color &c) 
{
  draw_rotate_color_opacity(name, x, y, frame_bump, angle, c, 1);
}
 
void image_handler::draw_rotate_color_opacity(const std::string name, float x, float y,
    float frame_bump, float angle, const SDL_Color &c, float opacity) 
{
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_rotate_color_opacity(x, y, angle, frame_bump, c, opacity);
}

void image_handler::draw_rotate_color_outline(const std::string name, 
    float x, float y, float frame_bump, float angle, 
    const SDL_Color &c, bool outline_color, float opacity)
{
  SDL_Color nc = {255, 255, 255};
  if(outline_color) { nc = c; }
  draw_rotate_color_opacity(name + "_outline", x, y, frame_bump, angle, nc, opacity);
  draw_rotate_color(name, x, y, frame_bump, angle, c);
}

void image_handler::draw_rotate(const std::string name, float x, float y, 
    float frame_bump, float angle) 
{
  //draw with no color mod
  static SDL_Color no_col = {255, 255, 255};
  draw_rotate_color(name, x, y, frame_bump, angle, no_col);
}

void image_handler::draw_tile(const std::string name, float parallax) {
  if(images.find(name) == images.end()) {
    //lazy initialization
    add_image(name);
  }
  images.at(name).draw_tile(parallax);
}

bool image_handler::is_on_screen(const vec2d &tlc, const vec2d &brc) {
  //take "tlc" and "brc", then determine ACTUAL tlc and brc
  float tlcx = (tlc[0] < brc[0] ? tlc[0] : brc[0]);
  float tlcy = (tlc[1] < brc[1] ? tlc[1] : brc[1]);
  float brcx = (tlc[0] < brc[0] ? brc[0] : tlc[0]);
  float brcy = (tlc[1] < brc[1] ? brc[1] : tlc[1]);

  if(viewport::get().get_tlc_x() > brcx || viewport::get().get_tlc_y() > brcy ||
      viewport::get().get_brc_x() < tlcx || viewport::get().get_brc_y() < tlcy) {
    //it's off-screen
    return false;
  }
  return true;
}

bool image_handler::is_on_screen_line(const vec2d &p1, const vec2d &p2) {
  rect2d r{
      viewport::get().get_tlc_x(), 
      viewport::get().get_tlc_y(),
      (float)viewport::get().get_w(), 
      (float)viewport::get().get_h()
  };
  hitline l(p1, p2);
  return r.overlap(l);
}

//cover whole screen if no dimensions specified
void image_handler::nc_draw_bg(int density) {
  nc_draw_bg(0, 0, COLS, LINES, density);
}

//if dimensions come in as vec2d, they're in world units - convert them to
//screen units, check if those aare on viewport, and send them on their way (or
//not, if they're off screen)
void image_handler::nc_draw_bg(const vec2d &tlc, const vec2d &brc, int density) {
  //check if this chunk is on-screen
  if(!is_on_screen(tlc, brc)) { return; }

  //pass these values to viewport and let it convert the coordinate pair
  //to an onscreen value
  int tlcx, tlcy, brcx, brcy;
  tlcx = tlc[0];
  tlcy = tlc[1];
  brcx = brc[0];
  brcy = brc[1];

  viewport::get().convert_to_nc_screen_units(tlcx, tlcy);
  viewport::get().convert_to_nc_screen_units(brcx, brcy);

  nc_draw_bg(tlcx, tlcy, brcx, brcy, density);
}

void image_handler::nc_draw_bg(int tlcx, int tlcy, int brcx, int brcy, int density) {
  //make a background of random data
  std::random_device rd;
  std::uniform_int_distribution<> distrib(0, density);

  char * const arr = render::get().nc_get_dv();
//TODO: standardize brc, do i -1 or not?
  for(int i=tlcx; i<brcx; i++) {
    for(int j=tlcy; j<brcy; j++) {
      char c = '?';
      if(distrib(rd) % density == 0) { c = '.'; } else { c = ' '; }
      arr[j * sizeof(char) * COLS + i] = c;
    }
  }
}

//accepts a pair of coordinates in world units, NOT in nc
//screen units
void image_handler::nc_truncate_line_to_screen(int &lx, int &ly, int &rx, int &ry) {

  //handle vertical or horizontal lines
  if(rx - lx == 0) {
    //vertical line
    if(ly < viewport::get().get_tlc_y()) {
      ly = viewport::get().get_tlc_y();
    }
    if(ry > viewport::get().get_brc_y()) {
      ry = viewport::get().get_brc_y() - 1;
    }
    return;
  }
  if(ry - ly == 0) {
    //horizontal line
    if(lx < viewport::get().get_tlc_x()) {
      lx = viewport::get().get_tlc_x();
    }
    if(rx > viewport::get().get_brc_x()) {
      rx = viewport::get().get_brc_x() - 1;
    }
    return;
  }

  //calculate slope
  float slope = (float)(ry-ly)/(float)(rx-lx);
  int offset = 0;

  //check if lx is less than 0; if it is, shift
  //lx to the right till it's 0, and shift ly up or down as
  //appropriate
  if(lx - viewport::get().get_tlc_x() < 0){
    offset = viewport::get().get_tlc_x() - lx;
    lx += offset;
    ly += std::round(offset * slope);
  }

  //same for rx
  if(rx > viewport::get().get_brc_x()) {
    offset = rx - viewport::get().get_brc_x() + 1;
    rx -= offset;
    ry -= std::round(offset * slope);
  }

  //the y values could be either less or greater than
  //viewport limits: check both

  //check ly
  if(ly - viewport::get().get_tlc_y() < 0){
    offset = viewport::get().get_tlc_y() - ly;
    ly += offset;
    lx += std::round(offset * (1/slope));
  }
  if(ly > viewport::get().get_brc_y()) {
    offset = ly - viewport::get().get_brc_y() + 1;
    ly -= offset;
    lx -= std::round(offset * (1/slope));
  }

  //check ry
  if(ry - viewport::get().get_tlc_y() < 0){
    offset = viewport::get().get_tlc_y() - ry;
    ry += offset;
    rx += std::round(offset * (1/slope));
  }
  if(ry > viewport::get().get_brc_y()) {
    offset = ry - viewport::get().get_brc_y() + 1;
    ry -= offset;
    rx -= std::round(offset * (1/slope));
  }
}

void image_handler::nc_draw_line(const vec2d &p1, const vec2d &p2, char c) {
  if(!is_on_screen_line(p1, p2)) { return; }

  //pass these values to viewport and let it convert the coordinate pair
  //to an onscreen value
  int lcx, lcy, rcx, rcy;
  lcx = p1[0];
  lcy = p1[1];
  rcx = p2[0];
  rcy = p2[1];

  if(lcx > rcx) {
    //swap lc and rc so "lc" is leftmost
    int temp = 0;
    temp = rcx; rcx = lcx; lcx = temp;
    temp = rcy; rcy = lcy; lcy = temp;
  }

  //truncate this to the viewport's size
  nc_truncate_line_to_screen(lcx, lcy, rcx, rcy);
  //turn it into screen units
  viewport::get().convert_to_nc_screen_units(lcx, lcy);
  viewport::get().convert_to_nc_screen_units(rcx, rcy);

  char *const arr = render::get().nc_get_dv();
  //draw end point - start point gets drawn anyway
  arr[lcy * sizeof(char) * COLS + lcx] = c;
  arr[rcy * sizeof(char) * COLS + rcx] = c; 

  //instead of trying to create a line and poll for where it exists, instead
  //determine the "rise and run" of the line and render it that way:
  //start at the left point of the line, and then at every pass, try to 
  //determine if it's better to go the right or go up/down depending on where we
  //are right now.
  int run = rcx - lcx;    //guaranteed to be positive, x is sorted
  int rise = rcy - lcy;   //MAY be negative, y is NOT sorted

  //handle easy cases here
  if(run == 0) {
    //vertical line - find lesser y and draw to greater
    for(int i=(lcy<rcy ? lcy : rcy); i<(lcy<rcy ? rcy : lcy); i++) {
      arr[i * sizeof(char) * COLS + lcx] = c;
    }
  }
  if(rise == 0) {
    //horizontal line - x is already sorted lesser-to-greater
    for(int i=lcx; i<rcx; i++) {
      arr[lcy * sizeof(char) * COLS + i] = c;
    }
  }
  if(rise != 0 && run != 0) {
    //some sort of diagonal line
    //figure out the fraction of rise and run: for every right we go, how much down?
    //note that this should accommodate lines that go any direction, not just down and right
    float rioru = (float)rise/(float)run; //for every 1 we run, how far do we rise?
    
    //go from left to right, one column at a time, skipping the already-drawn
    //first and last points of the line
    for(int i=lcx; i<rcx; i++) {
      //calculate how many rows up we have to go
      //for nearly horizontal lines, this may only run once in a while; for nearly vertical lines it
      //may run several times per pass

      //start j at the lcy, then offset it by its rise relative to "I", the 
      //current position of x
      //continue printing for j until we reach the next offset: using the 
      //calculated rise, jump up until we reach the next point at which we
      //should go "over" another point
      //increment j by 1 each time to go up one row at a time

      float jstart = std::round(lcy + (rioru * (i-lcx)));
      float jend = std::round(lcy + (rioru * ((i+1)-lcx)));

      //draw a point right where we want to determine
      arr[(int)jstart * sizeof(char) * COLS + i] = c;

      //the slope is downwards and j is counting up
      if(rioru >= 0) {
        for(int j=jstart; j<jend; j++) {
          arr[j * sizeof(char) * COLS + i] = c;
        }
      }
      //the slope is upwards and j is counting down
      else {
        for(int j=jstart; j>jend; j--) {
          arr[j * sizeof(char) * COLS + i] = c;
        }
      }
    }
  }
}

void image_handler::nc_draw_box(const vec2d &tlc, const vec2d &brc, char c) {
  if(!is_on_screen(tlc, brc)) { return; }

  //pass these values to viewport and let it convert the coordinate pair
  //to an onscreen value - it's safe to do so as a truncated box
  //doesn't distort like a line would

  int tlcx, tlcy, brcx, brcy;
  tlcx = std::round(tlc[0]);
  tlcy = std::round(tlc[1]);
  brcx = std::round(brc[0]);
  brcy = std::round(brc[1]);

  //this is expecting corners to be passed in properly, but check it 
  //just to be safe
  if(brcy < tlcy) {
    int tmp = brcy; brcy = tlcy; tlcy = tmp;
  }
  if(brcx < tlcx) {
    int tmp = brcx; brcx = tlcx; tlcx = tmp;
  }

  //truncate this to viewport's size and turn it into screen units all
  //at once
  viewport::get().convert_to_nc_screen_units(tlcx, tlcy);
  viewport::get().convert_to_nc_screen_units(brcx, brcy);

  char *const arr = render::get().nc_get_dv();

  //draw entirety of rectangle
  for(int i=tlcy; i<brcy; i++) {
    for(int j=tlcx; j<brcx; j++) {
      arr[i * sizeof(char) * COLS + j] = c;
    }
  }
}

void image_handler::add_image(std::string name) {
  //used emplace so i could avoid a default constructor call to image
  images.emplace(name, name);
}

void image_handler::get_col_from_team(const std::string team, SDL_Color &c) {
  if(strcmp(team.c_str(), "RED") == 0) {
    c.r = 160; c.g = 0; c.b = 0;
  }
  else if(strcmp(team.c_str(), "ORANGE") == 0) {
    c.r = 160; c.g = 96; c.b = 0;
  }
  else if(strcmp(team.c_str(), "YELLOW") == 0) {
    c.r = 192; c.g = 192; c.b = 0;
  }
  else if(strcmp(team.c_str(), "GREEN") == 0) {
    c.r = 0; c.g = 192; c.b = 0;
  }
  else if(strcmp(team.c_str(), "BLUE") == 0) {
    c.r = 64; c.g = 64; c.b = 192;
  }
  else if(strcmp(team.c_str(), "PURPLE") == 0) {
    c.r = 128; c.g = 0; c.b = 96;
  }
  else if(strcmp(team.c_str(), "BROWN") == 0) {
    c.r = 96; c.g = 64; c.b = 0;
  }
  else { c.r = 255; c.g = 255; c.b = 255; }
}

void image_handler::jitter_col(int strength, SDL_Color &c) {
  std::random_device rd;
  std::uniform_int_distribution<> distrib(-strength, strength);

  c.r += std::clamp((distrib(rd) * c.r) / 255, 0, 255);
  c.g += std::clamp((distrib(rd) * c.g) / 255, 0, 255);
  c.b += std::clamp((distrib(rd) * c.b) / 255, 0, 255);


}
