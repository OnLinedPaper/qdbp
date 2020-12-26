#include "rect2d.h"
#include "src/vec2d/vec2d.h"
#include "src/renders/render.h"
#include "src/viewport/viewport.h"

void rect2d::draw() const {
  coord_draw(
    (tlc[0] - viewport::get().get_tlc_x()),
    (tlc[1] - viewport::get().get_tlc_y())
  );
}

void rect2d::tlc_draw() const {
  coord_draw(
    tlc[0] - viewport::get().get_tlc_x(),
    tlc[1] - viewport::get().get_tlc_y()
  );
}

void rect2d::coord_draw(float x, float y) const {
  SDL_Rect r;
  r.x = x;
  r.y = y;
  r.w = dims[0];
  r.h = dims[1];


  SDL_Color c;
  SDL_GetRenderDrawColor(render::get().get_r(), &(c.r), &(c.g), &(c.b), &(c.a));

  SDL_SetRenderDrawColor(render::get().get_r(), 200, 200, 200, 128);

  //draw rect
  SDL_RenderFillRect(render::get().get_r(), &r);
  SDL_SetRenderDrawColor(render::get().get_r(), 64, 255, 64, 255);
  SDL_RenderDrawRect(render::get().get_r(), &r);
  //restore color
  SDL_SetRenderDrawColor(render::get().get_r(), c.r, c.g, c.b, c.a);
}

void rect2d::draw(int red, int green, int blue) const {
  SDL_Rect r;
  r.x = (tlc[0] - viewport::get().get_tlc_x());
  r.y = (tlc[1] - viewport::get().get_tlc_y());
  r.w = dims[0];
  r.h = dims[1];


  SDL_Color c;
  SDL_GetRenderDrawColor(render::get().get_r(), &(c.r), &(c.g), &(c.b), &(c.a));

  SDL_SetRenderDrawColor(render::get().get_r(), (red/2)+128, (green/2)+128, (blue/2)+128, 128);

  //draw rect
  SDL_RenderFillRect(render::get().get_r(), &r);
  SDL_SetRenderDrawColor(render::get().get_r(), red, green, blue, 255);
  SDL_RenderDrawRect(render::get().get_r(), &r);
}

bool rect2d::overlap(const hitline &l) const {

  //first, make a "box" of the rect and check it - this is about
  //3x faster than the next part of the algorithm, and can detect 
  //when the line is completely enclosed in the box
  rect2d line_rect(
    std::min(l.get_start()[0], l.get_end()[0]),
    std::min(l.get_start()[1], l.get_end()[1]),
    std::abs(l.get_start()[0] - l.get_end()[0]),
    std::abs(l.get_start()[0] - l.get_end()[0])
  );

  if(this->overlap(line_rect)) { return true; }
    vec2d trc(get_brc()[0], get_tlc()[1]);
    vec2d blc(get_tlc()[0], get_brc()[1]);

  return(
    l.collides({get_tlc(), trc}) ||
    l.collides({get_tlc(), blc}) ||
    l.collides({get_brc(), trc}) ||
    l.collides({get_brc(), blc})
  );

  return false;
}
/*
bool rect2d::overlap(const hitline &l) const {

  //check for hitbox collision first - it's about 3x faster

  //check the four corners of the rectangle - if they're all on the same "side"
  //of the rectangle, there's no collision

  vec2d trc(get_brc()[0], get_tlc()[1]);
  vec2d blc(get_tlc()[0], get_brc()[1]);

  //result is 0 if it's above, 1 if it's below
  //doing this because minge doesn't like std::signbit
  int sum = (check_point_side(get_tlc(), l) < 0 ? 1 : 0) +
            (check_point_side(trc, l) < 0 ? 1 : 0) +
            (check_point_side(get_brc(), l) < 0 ? 1 : 0) +
            (check_point_side(blc, l) < 0 ? 1 : 0);

  if(sum % 4 != 0) {
    //the box is within the line's axis - check if it touches

    //use the previous collision algorithm to see if the "box" of the
    //line overlaps

    rect2d line_rect(
      std::min(l.get_start()[0], l.get_end()[0]),
      std::min(l.get_start()[1], l.get_end()[1]),
      std::abs(l.get_start()[0] - l.get_end()[0]),
      std::abs(l.get_start()[0] - l.get_end()[0])
    );

    line_rect.draw();

    return this->overlap(line_rect);
  }

  return false;
}*/

float rect2d::check_point_side(const vec2d &in_point, const hitline &l) const {
  //i won't pretend i know how this works, it's from
  //https://stackoverflow.com/a/293052/7431860
  //uses implicit equation of line to determine what side the point is on

  vec2d start = l.get_start();
  vec2d end = l.get_end();

  return (end[1]-start[1])*in_point[0] + (start[0]-end[0])*in_point[1] + (end[0]*start[1] - start[0] * end[1]);
}

bool rect2d::overlap(const rect2d &r) const {

  /*
   *consider the rectangles below. they project "shadows" onto the
   *x-axis (below) and y-axis (beside). If both of the shadows
   *overlap, the rectangles overlap. If one of the shadows doesn't
   *overlap, the rectangles don't overlap.
   *
   *if either end of the # rectangle's shadow falls between the % shadow,
   * they collide. use this to calculate whether rectangles collide or not.
   *
   * tlc[1] > %        %---------%          tlc > %---------%
   *          |        |         |                |         |
   *          | #      |    #---------#           |         |   #---------#
   * brc[1] > % |      %----|----%    |           %---------%   |         |
   *            |           |         |    vs           brc ^   |         |
   *            |           |         |                         |         |
   *            #           #---------#                         #---------#
   *
   *                        #---------#                         #---------#
   *                   %---------%                %---------%
   *            tlc[0] ^  brc[0] ^         tlc[0] ^  brc[0] ^
  */



  //this rect
  vec2d ttlc = this->get_tlc();
  vec2d tbrc = this->get_brc();

  //other rect
  vec2d rtlc = r.get_tlc();
  vec2d rbrc = r.get_brc();


  //check if one of the rectangle's corners falls in the "shadow" of the other
  if(
    //leftmost x of other rect between least and greatest x of this rect
    (rtlc[0] >= ttlc[0] && rtlc[0] <= tbrc[0]) ||
    //rightmost x of other rect between least and greatest x of this rect
    (rbrc[0] >= ttlc[0] && rbrc[0] <= tbrc[0]) ||
    //leftmost x of this rect between least and greatest x of other rect
    (ttlc[0] >= rtlc[0] && ttlc[0] <= rbrc[0]) ||
    //rightmost x of this rect between least and greatest x of other rect
    (tbrc[0] >= rtlc[0] && tbrc[0] <= rbrc[0])
  ) {
    //the x-shadows overlap, now see if the y-shadows overlap
    if(
      (rtlc[1] >= ttlc[1] && rtlc[1] <= tbrc[1]) ||
      (rbrc[1] >= ttlc[1] && rbrc[1] <= tbrc[1]) ||
      (ttlc[1] >= rtlc[1] && ttlc[1] <= rbrc[1]) ||
      (tbrc[1] >= rtlc[1] && tbrc[1] <= rbrc[1])
    ) {
      //both shadows overlap
      return true;
    }
  }
  return false;
}
