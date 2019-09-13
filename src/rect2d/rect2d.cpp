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

void rect2d::coord_draw(double x, double y) const {
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
   *                                       tlc[0] ^  brc[0] ^
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
    (rbrc[0] >= ttlc[0] && rbrc[0] <= tbrc[0])
  ) {
    //the x-shadows overlap, now see if the y-shadows overlap
    if(
      (rtlc[1] >= ttlc[1] && rtlc[1] <= tbrc[1]) ||
      (rbrc[1] >= ttlc[1] && rbrc[1] <= tbrc[1])
    ) {
      //both shadows overlap
      return true;
    }
  }
  return false;
}
