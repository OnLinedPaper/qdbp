#include "viewport.h"
#include "src/renders/render.h"

viewport::viewport() {
  view_width = view_height = 0;
  pos[0] = pos[1] = 0;

  SDL_GetWindowSize(render::get().get_w(), &view_width, &view_height);
}

viewport::~viewport() { }
