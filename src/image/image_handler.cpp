#include "image_handler.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"
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

//cover whole screen if no dimensions specified
void image_handler::draw_nc_bg(int density) {
  draw_nc_bg(0, 0, COLS, LINES, density);
}

//if dimensions come in as vec2d, they're in world units - convert them to
//screen units, check if those aare on viewport, and send them on their way (or
//not, if they're off screen)
void image_handler::draw_nc_bg(const vec2d &tlc, const vec2d &brc, int density) {
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

  draw_nc_bg(tlcx, tlcy, brcx, brcy, density);
}

void image_handler::draw_nc_bg(int tlcx, int tlcy, int brcx, int brcy, int density) {
  //make a background of random data
  std::random_device rd;
  std::uniform_int_distribution<> distrib(0, density);

  char * const arr = render::get().nc_get_dv();
  for(int i=tlcx; i<brcx; i++) {
    for(int j=tlcy; j<brcy; j++) {
      char c = '?';
      if(distrib(rd) % density == 0) { c = '.'; } else { c = ' '; }
      arr[j * sizeof(char) * COLS + i] = c;
    }
  }
}

void image_handler::draw_nc_line(const vec2d &p1, const vec2d &p2, char c) {
  if(!is_on_screen(p1, p2)) { return; }

  //pass these values to viewport and let it convert the coordinate pair
  //to an onscreen value
  int lcx, lcy, rcx, rcy;
  lcx = p1[0];
  lcy = p1[1];
  rcx = p2[0];
  rcy = p2[1];

  //"tlc" is a bit of a misnomer - this end doesn't necessarily need to be
  //the top left corner, just the leftmost point of the line
  if(lcx > rcx) {
    //swap lc and rc so "lc" is leftmost
    int temp = 0;
    temp = rcx; rcx = lcx; lcx = temp;
    temp = rcy; rcy = lcy; lcy = temp;
  }

  viewport::get().convert_to_nc_screen_units(lcx, lcy);
  viewport::get().convert_to_nc_screen_units(rcx, rcy);

  char *const arr = render::get().nc_get_dv();
  arr[lcy * sizeof(char) * COLS + lcx] = c;
  arr[rcy * sizeof(char) * COLS + rcx] = c; 

  //instead of trying to create a line and poll for where it exists, instead
  //determine the "rise and run" of the line and render it that way:
  //start at the top left corner of the line, and then at every pass, try to 
  //determine if it's better to go down or to the right depending on where we
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
    float frise = rise;
    float frun = run;
    float rioru = frise/frun; //for every 1 we run, how far do we rise?
    
    //go from left to right, one column at a time
    for(int i=lcx; i<rcx; i++) {
      //calculate how many rows up we have to go
      //for nearly horizontal lines, this may only run once in a while; for vertical lines it
      //may run several times per pass
//TODO: continue from here
      for(int j=lcy + (rioru * (i-lcx)); j<lcy + (rioru * ((i-lcx) + 1)); j++) {
        arr[(j+1) * sizeof(char) * COLS + i] = c;
      }
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
